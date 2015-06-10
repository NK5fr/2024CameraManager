
#ifndef CALIBRATIONFILE_OPENGLWIDGET_H
#define CALIBRATIONFILE_OPENGLWIDGET_H

#include <qopenglwidget.h>
#include <qpainter.h>
#include "calibrationfile.h"

class CalibrationFile;

class CalibrationFileOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    CalibrationFileOpenGLWidget(CalibrationFile* calibFile);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

public slots:
    void setSelectedCameraCombination(TrackPoint::CameraCombination* selectedCamComb);

private:
    TrackPoint::CameraCombination* selectedCameraCombination = nullptr;
    CalibrationFile* calibFile = nullptr;
    QRect bounding;
};

#endif