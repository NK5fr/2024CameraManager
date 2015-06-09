
#ifndef CALIBRATION_FILE_H
#define CALIBRATION_FILE_H

#include <iostream>
#include <sstream>
#include <QtCore/QtCore>
#include <qwidget.h>
#include <qtreeview.h>
#include <qtableview.h>
#include <qlayout.h>
#include <qopenglwidget.h>
#include <qtreewidget.h>
#include <qstandarditemmodel.h>
#include <vector>
#include "datastructs.h"

using namespace std;

// Only supporting calibration_summary-files...

class CalibrationFile {
public:
    CalibrationFile(QString filePath);
    ~CalibrationFile();
    
    inline int getNumCameras() { return this->numCameras; }
    inline int getNumCombinations() { return this->numCombinations; }
    inline const vector<TrackPoint::CameraCombination*>& getCameraCombinations() { return this->camCombs; }
    inline const QString& getFileContains() { return this->fileContain; }

private:
    QString filePath;
    QString fileContain;
    int numCameras;
    int numCombinations;
    vector<TrackPoint::CameraCombination*> camCombs;

    void readCalibrationFile();
    void parseCalibrationData(QString& data);

    void calculateFov();
};

class CalibrationFileOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    CalibrationFileOpenGLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
    QRect bounding;
};

class CalibrationFileWidget : public QWidget {
    Q_OBJECT
public:
    CalibrationFileWidget(CalibrationFile* file);

    void initUI();

private slots:
    void combinationClicked(QTreeWidgetItem* item, int column);

private:
    CalibrationFile* calibFile;
    CalibrationFileOpenGLWidget* combinationPreviewWidget;
    QTreeWidget* filterList;
    QTreeWidget* combinationList;
    QTreeWidget* cameraTable;

    void updateCameraTable(TrackPoint::CameraCombination*);
};

#endif
