/**
 * \file qvideowidget.h
 * \author Virgile Wozny
 */


#ifndef QVIDEOWIDGET_H
#define QVIDEOWIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtOpenGL/QGL>
#include <QtOpenGL/QtOpenGL>
#include <QtCore/qmath.h>
#include <QThread>
#include <QMutex>
#include "imagedetect.h"
#include "trackpointproperty.h"
#include "texture.h"

/**
 * QVideoWidget
 * \brief display images from cameras, handle resizing and crosshair display.
 */
class QVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit QVideoWidget(QWidget* parent = 0);
    ~QVideoWidget();

    void setImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);

    ImageDetect* getImageDetect() { return this->imageDetect; }
    TrackPointProperty* getTrackPointProperty() { return this->trackPointProperty; }
    void setTrackPointProperty(TrackPointProperty* prop) { this->trackPointProperty = prop; }

signals:
    void forceUpdate();
public slots:
    void changedState (Qt::WindowStates oldState, Qt::WindowStates newState);
    void activateCrosshair(bool state);
    void receiveUpdate();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void resizeEvent(QResizeEvent* event = nullptr);
    void showEvent(QShowEvent* event = nullptr);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    ImageDetect* imageDetect;
    TrackPointProperty* trackPointProperty;
    OpenGL::Texture texture;
    QPoint mouse;
    QSize lastSize;
    QRect scaled;
    unsigned char* imgBuffer;
    unsigned int bufferSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    float ratio;
    bool active;
    bool mouseIn;

    void updateView();
};

#endif // QVIDEOWIDGET_H
