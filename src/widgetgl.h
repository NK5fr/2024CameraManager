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

#include "sleeper.h"
#include "socketviewerwidget.h"

using namespace std;

class SocketViewerWidget;

struct Vector3d {
    double x;
    double y;
    double z;
};

struct CalibrationCameraInfo {
    int camNo;
    int serialNo;
    Vector3d camPos;
    bool valueSet = false;
};

class WidgetGL : public QGLWidget {
    Q_OBJECT
public:
    WidgetGL(vector<vector<Vector3d>>* points, vector<float> minmax, SocketViewerWidget* socket, QString calibrationPath);
    ~WidgetGL();

    void showView(int viewTime);
    void start3DcoordinatesView();
    void launchView(int viewTime);
    void stopView();
    bool threadIsRunning();

public slots:
    void clickOnMenu(QAction *);

protected:
    bool eventFilter(QObject *, QEvent *);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    //void wheelEvent(QWheelEvent *event);
    //void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
private:
    SocketViewerWidget* svw;
    vector<QPoint> cameras;
    vector<vector<Vector3d>> pointData;
    vector<CalibrationCameraInfo> camerasData;
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
    bool adjustCamDistance = false;

    void initializingCameraCoordinates(QString calibrationPath);
};

#endif // WIDGETGL_H
