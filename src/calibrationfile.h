
// Written by: Lars Aksel Tver?mo
// 3D Motion Technologies AS

#ifndef CALIBRATION_FILE_H
#define CALIBRATION_FILE_H

#include <iostream>
#include <sstream>
#include <QtCore/QtCore>
#include <qwidget.h>
#include <qtreeview.h>
#include <qtableview.h>
#include <qlayout.h>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <qtreewidget.h>
#include <qstandarditemmodel.h>
#include <qplaintextedit.h>
#include <vector>
#include "datastructs.h"
#include "calibrationfileopenglwidget.h"

// Only supporting calibration_summary-files...

class CalibrationFile {
public:
    CalibrationFile(QString filePath);
    ~CalibrationFile();

    inline int getNumCameras() { return this->numCameras; }
    inline int getNumCombinations() { return this->numCombinations; }
    inline const std::vector<TrackPoint::CameraCombination*>& getCameraCombinations() { return this->camCombs; }
    inline const std::vector<TrackPoint::Camera*>& getCameras() { return this->cams; }
    inline const QString& getFileContains() { return this->fileContain; }
    inline const QString& getFileName() { return this->fileName; }
    inline QPlainTextEdit* getTextEdit() { return &this->textEdit; }
    inline bool isFailed() { return this->failed; }

private:
    QString filePath;
    QString fileName;
    QString fileContain;
    QPlainTextEdit textEdit;
    std::vector<TrackPoint::CameraCombination*> camCombs;
    std::vector<TrackPoint::Camera*> cams;
    int numCameras;
    int numCombinations;
    bool failed;

    void readCalibrationFile();
    bool parseCalibrationData(QString& data);

    void calculateFov();
};

#endif
