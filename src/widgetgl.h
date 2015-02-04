#ifndef WIDGETGL_H
#define WIDGETGL_H

#include <QtOpenGL/QGLWidget>

#include <QThread>
#include <vector>
#include <QMouseEvent>

#include "socketviewerwidget.h"

#include <iostream>

using namespace std;

class SocketViewerWidget;

class WidgetGL : public QGLWidget{
    Q_OBJECT
public:
    WidgetGL(vector<vector<double> > *points, std::vector < float > minmax,
             SocketViewerWidget *socket, QString calibrationPath);
    ~WidgetGL();

    void showView(int viewTime);

    void start3DcoordinatesView();
    void launchView(int viewTime);
    void stopView();

    bool threadIsRunning();



public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    void clickOnMenu(QAction *);

protected:
    bool eventFilter(QObject *, QEvent *);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    /*void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);*/


private:

    void initializingCameraCoordinates(QString calibrationPath);

    vector < QPoint > cameras;

    int coordinatesShown;
    vector< vector < double > > pointsDatas;
    vector< vector < double > > camerasData;

    QHash <int, bool> selected;

    /* Rotation */
    int xRot, cumulateXRot;
    int yRot, cumulateYRot;
    int zRot, cumulateZRot;
    char rotationAxis;
    int keyPressed;

    /* Default rotation */
    void initializeDefaultRotation();
    vector < vector < int > > defaultRotation;

    /* Array of min and max value to store
     * the scale of the 3D widget whch will be displayed
     * 0, 1 and 2 for x, y and z min
     * 3, 4 and 5 for x, y and z max */
    vector < float > minMax;
    bool initialScale;

    QPoint lastPos;
    QColor qtBlack;

    SocketViewerWidget *svw;
};

#endif // WIDGETGL_H
