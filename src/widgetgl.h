
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef WIDGETGL_H
#define WIDGETGL_H

#include <QtOpenGL/QtOpenGL>
#include <QThread>
#include <QtCore/qmath.h>
#include <QMouseEvent>
#include <QtWidgets/QMenu>

#include <vector>
#include <iostream>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#endif

#include <GL/glu.h>

#include "socketviewerwidget.h"
#include "calibrationfile.h"
#include "datastructs.h"

class SocketViewerWidget;

class WidgetGL : public QOpenGLWidget {
    Q_OBJECT
public:
    WidgetGL(SocketViewerWidget* socket, std::vector<std::vector<Vector3d*>>* points, QString calibrationPath);
    WidgetGL(SocketViewerWidget* socket);
    ~WidgetGL();

    void showView(int viewTime);
    void showXYPlane();
    void showXZPlane();
    void showYZPlane();

    void appendPoints(std::vector<Vector3d*> points) { this->pointData.push_back(points); }
    
    inline void setShowFovCone(bool onOff) { this->showFovCone = onOff; }
    inline void setShowPreceedingPoints(bool onOff) { this->showPreceedingPoints = onOff; }
    inline void setShowLines(bool onOff) { this->showLines = onOff; }
    inline void setShowCameras(bool onOff) { this->showCameras = onOff; }
    inline void setShowCameraLegs(bool onOff) { this->showCameraLegs = onOff; }
    inline void setShowCoordinateSystem(bool onOff) { this->showCoordinateSystem = onOff; }
    inline void setShowFloorLines(bool onOff) { this->showFloorLines = onOff; }
    inline void setShowOrtho(bool onOff) { 
        this->showOrtho = onOff;
        makeCurrent();
        updateProjection(width(), height(), 60);
        update();
        doneCurrent();
    }
    
    inline void setCamDistance(double distance) { this->camDistance = distance; }
    inline void setFOVConeSize(double size) { this->coneSize = size; }

    inline bool isShowFovCone() { return this->showFovCone; }
    inline bool isShowPreceedingPoints() { return this->showPreceedingPoints; }
    inline bool isShowLines() { return this->showLines; }
    inline bool isShowCameras() { return this->showCameras; }
    inline bool isShowCameraLegs() { return this->showCameraLegs; }
    inline bool isShowCoordinateSystem() { return this->showCoordinateSystem; }
    inline bool isShowFloorLines() { return this->showFloorLines; }
    inline bool isShowOrtho() { return this->showOrtho; }

    inline double getCamDistance() { return this->camDistance; }
    inline double getFOVConeSize() { return this->coneSize; }

public slots:
    void clickOnMenu(QAction *);

protected:
    bool eventFilter(QObject *, QEvent *);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mouseMoveEvent(QMouseEvent *event);
    
private:
    SocketViewerWidget* svw;
    CalibrationFile* calibFile;
    std::vector<std::vector<Vector3d*>> pointData;
    std::vector<TrackPoint::Camera*> camerasData;
    QPoint lastPos;
    int coordinatesShown;
    int keyPressed;
    int camViewIndex;
    int lastMouseX = -1;
    int lastMouseY = -1;
    int numShowedPreceedingPoints;
    double rotX;
    double rotY;
    double camDistance;
    double coneSize;
    bool initialScale;
    bool adjustCamDistance = false;
    
    bool showFovCone;
    bool showPreceedingPoints;
    bool showLines;
    bool showCameras;
    bool showCoordinateSystem;
    bool showFloorLines;
    bool showCameraLegs;
    bool showOrtho;
    bool showFromCamera;

    void initializingCameras();
    void updateProjection(int width, int height, double fov);
    void setViewFromCamera(int index);
};

#endif // WIDGETGL_H
