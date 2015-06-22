
#ifndef CALIBRATIONFILE_OPENGLWIDGET_H
#define CALIBRATIONFILE_OPENGLWIDGET_H

#include <qopenglwidget.h>
#include <qpainter.h>
#include <vector>
#include "calibrationfile.h"

class CalibrationFile;

class CalibrationFileOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    CalibrationFileOpenGLWidget(CalibrationFile* calibFile);

    void setSelectedCamera(int selectedCam);
    void setSelectedCameraCombination(TrackPoint::CameraCombination* selectedCamComb);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

public slots:

private:
    std::vector<QRectF> cameraArea;
    TrackPoint::CameraCombination* selectedCameraCombination = nullptr;
    int selectedCamera = -1;
    CalibrationFile* calibFile = nullptr;
    QRectF screenArea;
    QRectF calibrationCameraArea;

    void createCoordinateGrid(QRectF area, double gridSize, const QColor& color);
    void initCameraArea();
    void addCameraAreaMargin(double margin);
    void adjustCameraAreaForCoordinateOrigin(); // Make sure that CameraArea includes the Coordinate Origin...
};

#endif