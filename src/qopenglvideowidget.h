
// Written by: Lars Aksel Tveråmo

#ifndef QOPENGLVIDEOWIDGET_H
#define QOPENGLVIDEOWIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/qmath.h>
#include <QThread>
#include <QMutex>
#include "imagedetect.h"
#include "trackpointproperty.h"
#include "texture.h"

class QOpenglVideoWidget : public QGLWidget {
    Q_OBJECT
public:
    QOpenglVideoWidget();
    ~QOpenglVideoWidget();

    void setImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);

    ImageDetect* getImageDetect() { return this->imageDetect; }
    TrackPointProperty* getTrackPointProperty() { return this->trackPointProperty; }
    void setTrackPointProperty(TrackPointProperty* prop) { this->trackPointProperty = prop; }

signals:
    void forceUpdate();
public slots:
    void changedState(Qt::WindowStates oldState, Qt::WindowStates newState);
    void activateCrosshair(bool state);
    void receiveUpdate();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void resizeEvent(QResizeEvent* event = nullptr);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    ImageDetect* imageDetect;
    ImageDetectThread* imgDetThread;
    TrackPointProperty* trackPointProperty;
    OpenGL::Texture texture;
    unsigned char* imgBuffer;
    unsigned int bufferSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    bool mouseIn;
};


#endif
