
#include "calibrationfile.h"

static const double PI = 3.1415926535;

CalibrationFile::CalibrationFile(QString filePath) : filePath(filePath) {
    numCameras = 0;
    numCombinations = 0;
    readCalibrationFile();
    failed = !parseCalibrationData(fileContain);
    if (failed) return;
    QFileInfo file(filePath);
    fileName = file.fileName();
    calculateFov();

    textEdit.setFont(QFont("Courier", 9));
    textEdit.setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    textEdit.setPlainText(fileContain);

    for (int i = 0; i < camCombs.size(); i++) {
        if (!camCombs[i]->status == TrackPoint::CalibrationStatus::OK) continue;
        //printf("Combination: %u_%u_%u\n", camCombs[i]->camNumbers[0], camCombs[i]->camNumbers[1], camCombs[i]->camNumbers[2]);
        for (int cam = 0; cam < 3; cam++) {
            TrackPoint::Camera* thisCam = camCombs[i]->cameras[cam];
            //printf("Camno: %u\tFovW: %.2f\tFovH: %.2f\n", thisCam->camNo, thisCam->fovWidth * (180 / PI), thisCam->fovHeight * (180 / PI));
        }
        //printf("\n");
    }
}

CalibrationFile::~CalibrationFile() {
    for (int i = 0; i < camCombs.size(); i++) {
        if (!camCombs[i]->status == TrackPoint::CalibrationStatus::OK) continue;
        for (int j = 0; j < 3; j++) {
            delete camCombs[i]->cameras[j];
        }
    }
    camCombs.clear();
}

void CalibrationFile::readCalibrationFile() {
    QFile myFile(filePath);
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    fileContain = QString(myFile.readAll());
    myFile.close();
}

bool CalibrationFile::parseCalibrationData(QString& data) {
    int atLine = 0;
    QStringList dataList = data.split("\n");
    int numLines = dataList.size();

    // Grabs camera number, and serialnumber
    vector<TrackPoint::Camera*> cameras;
    QRegularExpression numberMatchRegEx("-?\\d+\\.?\\d*\\e?-?\\d*");
    QRegularExpression camLinesRegEx("\\d+");
    while (!dataList[atLine].isEmpty()) {
      QRegularExpressionMatchIterator camLineIter = camLinesRegEx.globalMatch(dataList[atLine]);
      TrackPoint::Camera* newCam = new TrackPoint::Camera();
      if (camLineIter.hasNext()) {
        QRegularExpressionMatch match = camLineIter.next();
        if (match.hasMatch()) {
          newCam->camNo = match.captured(0).toInt();
        }
      }
      if (camLineIter.hasNext()) {
        QRegularExpressionMatch match = camLineIter.next();
        if (match.hasMatch()) {
            newCam->serialNo = match.captured(0).toUInt();
        }
      }
      if (camLineIter.hasNext()) {
        QRegularExpressionMatch match = camLineIter.next();
        if (match.hasMatch()) {
          newCam->pixelWidth = match.captured(0).toInt();
        }
      }
      if (camLineIter.hasNext()) {
        QRegularExpressionMatch match = camLineIter.next();
        if (match.hasMatch()) {
          newCam->pixelHeight = match.captured(0).toInt();
        }
      }
      cameras.push_back(newCam);
      atLine++;
    }
    numCameras = cameras.size();
    cams = cameras;

    // Find first line where camera combinations are mentioned...
    QRegularExpression combLinesRegEx("([0-9]+_[0-9]+_[0-9]+):\\s+(.+)");
    while (!combLinesRegEx.match(dataList[atLine]).hasMatch() && atLine < numLines - 1) atLine++;

    // Find both camera-group combination and the string with S0, mean, max and number of frames detected...
    QRegularExpression combNumbersRegEx("([0-9]+)_([0-9]+)_([0-9]+)");
    QRegularExpression combStringRegEx("(-?\\d+\\.?\\d*)(?:.+Mean.+)(-?\\d+\\.?\\d*)(?:.+max.+)(-?\\d+\\.?\\d*)(?:.+: )(\\d+)");
    QRegularExpressionMatch combLineMatch = combLinesRegEx.match(dataList[atLine]);
    while (combLineMatch.hasMatch() && atLine < numLines - 1) {
        TrackPoint::CameraCombination* camComb = new TrackPoint::CameraCombination();
        QString combination = combLineMatch.captured(1);
        QString string = combLineMatch.captured(2);

        QRegularExpressionMatch combNumberMatch = combNumbersRegEx.match(combination);
        camComb->camNumbers[0] = combNumberMatch.captured(1).toInt();
        camComb->camNumbers[1] = combNumberMatch.captured(2).toInt();
        camComb->camNumbers[2] = combNumberMatch.captured(3).toInt();

        QRegularExpressionMatch combStringMatch = combStringRegEx.match(string);
        if (combStringMatch.hasMatch()) {
            double sO = combStringMatch.captured(1).toDouble();
            double mean = combStringMatch.captured(2).toDouble();
            double max = combStringMatch.captured(3).toDouble();
            int noFrames = combStringMatch.captured(4).toInt();

            camComb->s0 = sO;
            camComb->mean = mean;
            camComb->max = max;
            camComb->numFrames = noFrames;
            camComb->status = TrackPoint::CalibrationStatus::OK;
        } else {
            // TODO Do handling when 'NO CONVERGENCE' or other...
            //printf("ERROR: Could not parse: %s\n", string.toLocal8Bit().data());
            if (dataList[atLine].contains("NaN")) camComb->textStatus = "NaN";
            if (dataList[atLine].contains("CONVERGENCE")) camComb->textStatus = "No Conv.";
            if (dataList[atLine].contains("Prep")) camComb->textStatus = "Prep. failed";
            camComb->s0 = 0;
            camComb->numFrames = 0;
            camComb->cameras[0] = nullptr;
            camComb->cameras[1] = nullptr;
            camComb->cameras[2] = nullptr;
            camComb->status = TrackPoint::CalibrationStatus::Failed;
        }
        camCombs.push_back(camComb);
        //printf("Camera Combination: \'%s\'\n", combination.toLocal8Bit().data());
        //printf("String: \'%s\'\n\n", string.toLocal8Bit().data());

        combLineMatch = combLinesRegEx.match(dataList[++atLine]);
    };
    //printf("\n");

    // Find camera information for every camera in every camera-group/camera-combination...
    int numGroupsDone = 0;
    QRegularExpression camGroupRegEx("([0-9]+)_([0-9]+)_([0-9]+)\\.dat");
    QRegularExpression camIdRegEx("(?:Camno\\s+)(\\d+)(?:.+\\s)(\\d+)");

    while (numGroupsDone < camCombs.size() && atLine < numLines - 1) {
        QRegularExpressionMatch camGroupMatch = camGroupRegEx.match(dataList[atLine]);
        while (!camGroupMatch.hasMatch() && atLine < numLines - 1) camGroupMatch = camGroupRegEx.match(dataList[++atLine]);
        if (atLine >= numLines - 1) break;
        if (!dataList[++atLine].isEmpty()) continue;
        TrackPoint::CameraCombination* atCamComb = nullptr;
        int cam01 = camGroupMatch.captured(1).toInt();
        int cam02 = camGroupMatch.captured(2).toInt();
        int cam03 = camGroupMatch.captured(3).toInt();

        // Find correct camera-combination for the found group...
        for (int i = 0; i < camCombs.size(); i++) {
            if (camCombs[i]->camNumbers[0] == cam01 && camCombs[i]->camNumbers[1] == cam02 && camCombs[i]->camNumbers[2] == cam03) {
                //if (!camCombs[i]->valid) break;
                atCamComb = camCombs[i];
            }
        }
        if (atCamComb == nullptr) {
            //printf("ERROR: Could not find correct camera combination in calibration-file!\n");
            atLine++;
            continue;;
        }

        for (int i = 0; i < 3; i++) {
            QRegularExpressionMatch camIdMatch = camIdRegEx.match(dataList[atLine]);
            while (!camIdMatch.hasMatch() && atLine < numLines - 1) camIdMatch = camIdRegEx.match(dataList[++atLine]);


            //QRegularExpressionMatch camPosMatch = camPosRegEx.match(dataList[++atLine]);
            //QRegularExpressionMatch camOrientMatch = camOrientRegEx.match(dataList[++atLine]);

            TrackPoint::Camera* cam = new TrackPoint::Camera();
            int camNo = camIdMatch.captured(1).toInt();
            if (camNo >= numCameras) return false;
            *cam = *cameras[camNo];

            QRegularExpression numberMatchRegEx("-?\\d+(?:\\.?\\d*[eE]?\\-?\\d*)?");
            QRegularExpressionMatchIterator camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            // Camera position...
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->camPos.x;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->camPos.y;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->camPos.z;
                }
            }

            // Camera orientation...
            camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->orient.alpha;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->orient.beta;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->orient.kappa;
                }
            }

            atLine++; // Skip empty line...
            camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->cameraConstant;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->cameraConstantStdDev;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->XH;
                }
            }

            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->XHStdDev;
                }
            }

            // YH, YH Std. Dev., AF and AF Std. Dev.
            camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->YH;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->YHStdDev;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->AF;
                }
            }

            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->AFStdDev;
                }
            }

            // ORT and ORT Std. Dev.
            camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->ORT;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->ORTStdDev;
                }
            }

            // F1, F1 Std.Dev., F2, F2 Std.Dev
            camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->F1;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->F1StdDev;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->F2;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->F2StdDev;
                }
            }

            camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->F3;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->F3StdDev;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->P1;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->P1StdDev;
                }
            }

            camLineIter = numberMatchRegEx.globalMatch(dataList[++atLine]);
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->P2;
                }
            }
            if (camLineIter.hasNext()) {
                camLineIter.next(); // This match is ignored...
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->P2StdDev;
                }
            }
            if (camLineIter.hasNext()) {
                QRegularExpressionMatch match = camLineIter.next();
                if (match.hasMatch()) {
                    istringstream os(match.captured(0).toStdString());
                    os >> cam->RO;
                }
            }
            atCamComb->cameras[i] = cam;
        }
        numGroupsDone++;
    }
    return true;
}

void CalibrationFile::calculateFov() {
    for (int i = 0; i < camCombs.size(); i++) {
        if (!camCombs[i]->status == TrackPoint::CalibrationStatus::OK) continue;
        for (int cam = 0; cam < 3; cam++) {
            TrackPoint::Camera* thisCam = camCombs[i]->cameras[cam];
            if (thisCam == nullptr) continue;
            thisCam->fovWidth = (atan(thisCam->pixelWidth / (2 * thisCam->cameraConstant)) * 2);
            thisCam->fovHeight = (atan(thisCam->pixelHeight / (2 * thisCam->cameraConstant)) * 2);
        }
    }
}