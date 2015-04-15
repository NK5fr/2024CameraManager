#ifndef WIDGETGL_H
#define WIDGETGL_H

#include <QtOpenGL/qgl.h>
#include <QThread>
#include <QtCore/qmath.h>
#include <QMouseEvent>
#include <QMenu>

#include <vector>
#include <iostream>
#include <gl\GLU.h>

#include "socketviewerwidget.h"
#include "calibrationfile.h"
#include "datastructs.h"

using namespace std;

class SocketViewerWidget;

class WidgetGL : public QGLWidget {
    Q_OBJECT
public:
    WidgetGL(vector<vector<Vector3d>>* points, vector<float> minmax, SocketViewerWidget* socket, QString calibrationPath);
    ~WidgetGL();

    void showView(int viewTime);
    void showXYPlane();
    void showXZPlane();
    void showYZPlane();
    
    inline void setShowFovCone(bool onOff) { this->showFovCone = onOff; }
    inline void setShowPreceedingPoints(bool onOff) { this->showPreceedingPoints = onOff; }
    inline void setShowLines(bool onOff) { this->showLines = onOff; }
    inline void setShowCameras(bool onOff) { this->showCameras = onOff; }
    inline void setShowCameraLegs(bool onOff) { this->showCameraLegs = onOff; }
    inline void setShowCoordinateSystem(bool onOff) { this->showCoordinateSystem = onOff; }
    inline void setShowFloorLines(bool onOff) { this->showFloorLines = onOff; }
    inline void setShowOrtho(bool onOff) { 
        this->showOrtho = onOff;
        initializeGL();
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
    //vector<QPoint> cameras;
    vector<vector<Vector3d>> pointData;
    vector<TrackPoint::Camera*> camerasData;
    QHash <int, bool> selected;
    QPoint lastPos;
    QColor qtBlack;
    int coordinatesShown;
    int keyPressed;
    bool initialScale;
    int lastMouseX = -1;
    int lastMouseY = -1;
    double rotX;
    double rotY;
    double camDistance;
    double coneSize;
    bool adjustCamDistance = false;
    
    // Lars Aksel - Check off/on settings...
    bool showFovCone;
    bool showPreceedingPoints;
    bool showLines;
    bool showCameras;
    bool showCoordinateSystem;
    bool showFloorLines;
    bool showCameraLegs;
    bool showOrtho;

    void initializingCameras();
};

#endif // WIDGETGL_H
