
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
            camComb->valid = true;
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
}

void CalibrationFile::calculateFov() {
    for (int i = 0; i < camCombs.size(); i++) {
        if (!camCombs[i]->valid) continue;
        for (int cam = 0; cam < 3; cam++) {
            TrackPoint::Camera* thisCam = camCombs[i]->cameras[cam];
            if (thisCam == nullptr) continue;
            thisCam->fovWidth = (atan(thisCam->pixelWidth / (2 * thisCam->cameraConstant)) * 2);
            thisCam->fovHeight = (atan(thisCam->pixelHeight / (2 * thisCam->cameraConstant)) * 2);
        }
    }
}

CalibrationFileWidget::CalibrationFileWidget(CalibrationFile* calibFile) : QWidget() {
    this->calibFile = calibFile;
    initUI();
}

void CalibrationFileWidget::initUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout();
    QVBoxLayout* leftLayout = new QVBoxLayout();
    QGridLayout* gridLayout = new QGridLayout();

    combinationPreviewWidget = new CalibrationFileOpenGLWidget();
    leftLayout->addWidget(combinationPreviewWidget);

    filterList = new QTreeWidget();
    filterList->setHeaderLabels(QStringList() << "" << "Warning value" << "Max value");
    QTreeWidgetItem* s0Filter = new QTreeWidgetItem();
    s0Filter->setText(0, "S0");
    s0Filter->setText(1, "0.13");
    s0Filter->setText(2, "10");
    filterList->addTopLevelItem(s0Filter);
    QTreeWidgetItem* cStdDevFilter = new QTreeWidgetItem();
    cStdDevFilter->setText(0, "C std. dev.");
    cStdDevFilter->setText(1, "2");
    cStdDevFilter->setText(2, "5");
    filterList->addTopLevelItem(cStdDevFilter);
    filterList->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    leftLayout->addWidget(filterList);

    cameraTable = new QTreeWidget(this);
    cameraTable->setHeaderLabels(QStringList() << "" << "Cam 0" << "Cam 1" << "Cam 2");
    //cameraTable->setContentsMargins(0, 0, 20, 0);
    leftLayout->addWidget(cameraTable);


    //QStandardItemModel* combinationListModel = new QStandardItemModel(2, 3, this);
    combinationList = new QTreeWidget(this);
    combinationList->setHeaderLabels(QStringList() << "Combination" << "S0" << "#. of frames");
    combinationList->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Expanding);

    //combinationListModel->setHorizontalHeaderLabels(QStringList() << "Combination" << "S0" << "#. of frames");
    vector<TrackPoint::CameraCombination*> camComb = calibFile->getCameraCombinations();

    for (int i = 0; i < camComb.size(); i++) {
        QString combination = QString::number(camComb[i]->camNumbers[0]) + "_" + QString::number(camComb[i]->camNumbers[1]) + "_" + QString::number(camComb[i]->camNumbers[2]);
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, combination);
        item->setText(1, QString::number(camComb[i]->s0));
        item->setText(2, QString::number(camComb[i]->numFrames));
        item->setData(0, Qt::UserRole, QVariant::fromValue((quintptr) camComb[i]));
        if (camComb[i]->numFrames < 55) {
            item->setBackgroundColor(0, QColor(255, 0, 0, 150));
            item->setBackgroundColor(1, QColor(255, 0, 0, 150));
            item->setBackgroundColor(2, QColor(255, 0, 0, 150));
        }
        else if (camComb[i]->s0 > 0.13) {
            item->setBackgroundColor(0, QColor(255, 255, 0, 150));
            item->setBackgroundColor(1, QColor(255, 255, 0, 150));
            item->setBackgroundColor(2, QColor(255, 255, 0, 150));
        }
        if (camComb[i]->numFrames < 55) item->setDisabled(true);
        combinationList->addTopLevelItem(item);
    }
    combinationList->resizeColumnToContents(0);
    combinationList->resizeColumnToContents(1);
    combinationList->resizeColumnToContents(2);

    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(combinationList);
    setLayout(mainLayout);

    combinationPreviewWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    
    connect(combinationList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(combinationClicked(QTreeWidgetItem*, int)));
}

void CalibrationFileWidget::combinationClicked(QTreeWidgetItem* item, int column) {
    if (item->isDisabled()) {
        cameraTable->clear();
        return;
    }
    TrackPoint::CameraCombination* camComb = (TrackPoint::CameraCombination*) item->data(0, Qt::UserRole).value<quintptr>();
    updateCameraTable(camComb);
}

void CalibrationFileWidget::updateCameraTable(TrackPoint::CameraCombination* camComb) {
    cameraTable->clear();

    QTreeWidgetItem* cameraNoItem = new QTreeWidgetItem();
    cameraNoItem->setText(0, "Camera No");
    cameraNoItem->setText(1, QString::number(camComb->cameras[0]->camNo));
    cameraNoItem->setText(2, QString::number(camComb->cameras[1]->camNo));
    cameraNoItem->setText(3, QString::number(camComb->cameras[2]->camNo));
    cameraTable->addTopLevelItem(cameraNoItem);

    QTreeWidgetItem* cameraConstItem = new QTreeWidgetItem();
    cameraConstItem->setText(0, "Camera Constant");
    cameraConstItem->setText(1, QString::number(camComb->cameras[0]->cameraConstant));
    cameraConstItem->setText(2, QString::number(camComb->cameras[1]->cameraConstant));
    cameraConstItem->setText(3, QString::number(camComb->cameras[2]->cameraConstant));
    cameraTable->addTopLevelItem(cameraConstItem);

    QTreeWidgetItem* cameraStdDevItem = new QTreeWidgetItem();
    cameraStdDevItem->setText(0, "C Std. Dev.");
    cameraStdDevItem->setText(1, QString::number(camComb->cameras[0]->cameraConstantStdDev));
    cameraStdDevItem->setText(2, QString::number(camComb->cameras[1]->cameraConstantStdDev));
    cameraStdDevItem->setText(3, QString::number(camComb->cameras[2]->cameraConstantStdDev));
    cameraTable->addTopLevelItem(cameraStdDevItem);

    QTreeWidgetItem* xhStdDevItem = new QTreeWidgetItem();
    xhStdDevItem->setText(0, "XH Std. Dev.");
    xhStdDevItem->setText(1, QString::number(camComb->cameras[0]->XHStdDev));
    xhStdDevItem->setText(2, QString::number(camComb->cameras[1]->XHStdDev));
    xhStdDevItem->setText(3, QString::number(camComb->cameras[2]->XHStdDev));
    cameraTable->addTopLevelItem(xhStdDevItem);

    QTreeWidgetItem* yhStdDevItem = new QTreeWidgetItem();
    yhStdDevItem->setText(0, "YH Std. Dev.");
    yhStdDevItem->setText(1, QString::number(camComb->cameras[0]->YHStdDev));
    yhStdDevItem->setText(2, QString::number(camComb->cameras[1]->YHStdDev));
    yhStdDevItem->setText(3, QString::number(camComb->cameras[2]->YHStdDev));
    cameraTable->addTopLevelItem(yhStdDevItem);

    QTreeWidgetItem* f1StdDevItem = new QTreeWidgetItem();
    f1StdDevItem->setText(0, "F1 Std. Dev.");
    f1StdDevItem->setText(1, QString::number(camComb->cameras[0]->F1StdDev));
    f1StdDevItem->setText(2, QString::number(camComb->cameras[1]->F1StdDev));
    f1StdDevItem->setText(3, QString::number(camComb->cameras[2]->F1StdDev));
    cameraTable->addTopLevelItem(f1StdDevItem);

    QTreeWidgetItem* f2StdDevItem = new QTreeWidgetItem();
    f2StdDevItem->setText(0, "F2 Std. Dev.");
    f2StdDevItem->setText(1, QString::number(camComb->cameras[0]->F2StdDev));
    f2StdDevItem->setText(2, QString::number(camComb->cameras[1]->F2StdDev));
    f2StdDevItem->setText(3, QString::number(camComb->cameras[2]->F2StdDev));
    cameraTable->addTopLevelItem(f2StdDevItem);

    QTreeWidgetItem* p1StdDevItem = new QTreeWidgetItem();
    p1StdDevItem->setText(0, "P1 Std. Dev.");
    p1StdDevItem->setText(1, QString::number(camComb->cameras[0]->P1StdDev));
    p1StdDevItem->setText(2, QString::number(camComb->cameras[1]->P1StdDev));
    p1StdDevItem->setText(3, QString::number(camComb->cameras[2]->P1StdDev));
    cameraTable->addTopLevelItem(p1StdDevItem);

    QTreeWidgetItem* p2StdDevItem = new QTreeWidgetItem();
    p2StdDevItem->setText(0, "P2 Std. Dev.");
    p2StdDevItem->setText(1, QString::number(camComb->cameras[0]->P2StdDev));
    p2StdDevItem->setText(2, QString::number(camComb->cameras[1]->P2StdDev));
    p2StdDevItem->setText(3, QString::number(camComb->cameras[2]->P2StdDev));
    cameraTable->addTopLevelItem(p2StdDevItem);

    cameraTable->resizeColumnToContents(0);
    cameraTable->resizeColumnToContents(1);
    cameraTable->resizeColumnToContents(2);
    cameraTable->resizeColumnToContents(3);
}

CalibrationFileOpenGLWidget::CalibrationFileOpenGLWidget() : QOpenGLWidget() {
    bounding.setLeft(2000);
}

void CalibrationFileOpenGLWidget::initializeGL() {
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-bounding.left() * ((double) width() / height()), bounding.left() * ((double) width() / height()), -bounding.left(), bounding.left(), -1000000, 1000000);
}

void CalibrationFileOpenGLWidget::paintGL() {
    const double gridSize = 500;
    // X Axis
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(gridSize, 0, 0);
    glEnd();

    // Y Axis
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, gridSize, 0);
    glEnd();

    // Z Axis
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, gridSize);
    glEnd();
}

void CalibrationFileOpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-bounding.left() * ((double) width() / height()), bounding.left() * ((double) width() / height()), -bounding.left(), bounding.left(), -1000000, 1000000);
}