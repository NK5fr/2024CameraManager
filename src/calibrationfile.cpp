
#include "calibrationfile.h"

static const double PI = 3.1415926535;

CalibrationFile::CalibrationFile(QString filePath) : filePath(filePath) {
    numCameras = 0;
    numCombinations = 0;
    readCalibrationFile();
    parseCalibrationData(fileContain);
    calculateFov();

    for (int i = 0; i < camCombs.size(); i++) {
        if (!camCombs[i]->valid) continue; 
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
        if (!camCombs[i]->valid) continue;
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

void CalibrationFile::parseCalibrationData(QString& data) {
    int atLine = 0;
    QStringList dataList = data.split("\n");
    int numLines = dataList.size();

    // Grabs camera number, and serialnumber
    vector<TrackPoint::Camera*> cameras;
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
          newCam->serialNo = match.captured(0).toInt();
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
            camComb->valid = (noFrames == 55);
        } else {
            // TODO Do handling when 'NO CONVERGENCE' or other...
            //printf("ERROR: Could not parse: %s\n", string.toLocal8Bit().data());
            camComb->valid = false;
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
    QRegularExpression camPosRegEx("(?:\\s+X[0,O]: )(-?\\d+\\.?\\d*)(?:\\s+Y[0,O]: )(-?\\d+\\.?\\d*)(?:\\s+Z[0,O]: )(-?\\d+\\.?\\d*)");
    QRegularExpression camOrientRegEx("(?:\\s+AL: )(-?\\d+\\.?\\d*)(?:\\s+BE: )(-?\\d+\\.?\\d*)(?:\\s+KA: )(-?\\d+\\.?\\d*)");
    QRegularExpression camExtra01RegEx("(?:C:\\s*)(\\d+\\.?\\d*)(?:\\s+C\\sstd\\.dev\\.:\\s+)(\\d+\\.?\\d*)");

    while (numGroupsDone < camCombs.size() && atLine < numLines - 1) {
        QRegularExpressionMatch camGroupMatch = camGroupRegEx.match(dataList[atLine]);
        while (!camGroupMatch.hasMatch() && atLine < numLines - 1) camGroupMatch = camGroupRegEx.match(dataList[++atLine]);
        TrackPoint::CameraCombination* atCamComb = nullptr;
        int cam01 = camGroupMatch.captured(1).toInt();
        int cam02 = camGroupMatch.captured(2).toInt();
        int cam03 = camGroupMatch.captured(3).toInt();

        // Find correct camera-combination for the found group...
        for (int i = 0; i < camCombs.size(); i++) {
            if (camCombs[i]->camNumbers[0] == cam01 && camCombs[i]->camNumbers[1] == cam02 && camCombs[i]->camNumbers[2] == cam03) {
                if (!camCombs[i]->valid) break;
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

            QRegularExpressionMatch camPosMatch = camPosRegEx.match(dataList[++atLine]);
            QRegularExpressionMatch camOrientMatch = camOrientRegEx.match(dataList[++atLine]);
            atLine += 2;
            QRegularExpressionMatch camExtra01Match = camExtra01RegEx.match(dataList[atLine]);
            TrackPoint::Camera* cam = new TrackPoint::Camera();
            cam->camNo = camIdMatch.captured(1).toInt();
            cam->serialNo = camIdMatch.captured(2).toInt();

            cam->camPos.x = camPosMatch.captured(1).toDouble();
            cam->camPos.y = camPosMatch.captured(2).toDouble();
            cam->camPos.z = camPosMatch.captured(3).toDouble();

            cam->orient.alpha = camOrientMatch.captured(1).toDouble();
            cam->orient.beta = camOrientMatch.captured(2).toDouble();
            cam->orient.kappa = camOrientMatch.captured(3).toDouble();

            cam->cameraConstant = camExtra01Match.captured(1).toDouble();
            cam->cameraConstantStdDev = camExtra01Match.captured(2).toDouble();

            atCamComb->cameras[i] = cam;
        }
        numGroupsDone++;
    }
}

void CalibrationFile::calculateFov() {
    for (int i = 0; i < camCombs.size(); i++) {
        if (!camCombs[i]->valid) continue;
        for (int cam = 0; cam < 3; cam++) {
            TrackPoint::Camera* thisCam = camCombs[i]->cameras[cam];
            thisCam->fovWidth = (atan(thisCam->pixelWidth / (2 * thisCam->cameraConstant)) * 2);
            thisCam->fovHeight = (atan(thisCam->pixelHeight / (2 * thisCam->cameraConstant)) * 2);
        }
    }
}