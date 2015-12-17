
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef CALIBRATIONFILE_OPENGL_WIDGET_H
#define CALIBRATIONFILE_OPENGL_WIDGET_H

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
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

public slots:

private:
    std::vector<QRectF> cameraArea;
    TrackPoint::CameraCombination* selectedCameraCombination = nullptr;
    int selectedCamera = -1;
    int mouseOverCameraArea = -1;
    int enteredCameraArea = -1;
    CalibrationFile* calibFile = nullptr;
    QRectF screenArea;
    QRectF calibrationCameraArea;
    QPointF mousePos;
    QPointF mouseOpenGLPos;
    bool mouseInside = false;
    bool enteredCameraWindow = false;

    const float cameraAreaMargin = 50; // 5 cm
    const double gridSize = 500;

    void createCoordinateGrid(QRectF area, double gridSize, const QColor& color);
    void initCameraArea();
    void addCameraAreaMargin(double margin);
    void adjustCameraAreaForCoordinateOrigin(); // Make sure that CameraArea includes the Coordinate Origin...
    void updateMouseOverCameraArea();
    void keepAspectRatio(QRectF& topWindowRect, QRectF windowRect, int screenwidth, int screenheight, double margin = 0);
};

#endif // CALIBRATIONFILE_OPENGL_WIDGET_H