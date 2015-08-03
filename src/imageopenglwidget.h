
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef IMAGE_OPENGL_WIDGET_H
#define IMAGE_OPENGL_WIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtOpenGL/QGL>
#include <QtOpenGL/QtOpenGL>
#include <qopenglwidget.h>
#include <qopengl.h>

#include "imagedetect.h"
#include "trackpointproperty.h"
#include "texture.h"

class ImageOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {

public:
    ImageOpenGLWidget(TrackPointProperty* trackPointProps = nullptr, QWidget* parent = nullptr);
    ~ImageOpenGLWidget();

    void setTrackPointProperty(TrackPointProperty* trackPoint) { this->trackPointProperty = trackPoint; }
    virtual void updateImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);
    const OpenGL::Texture* getTexture() { return &this->texture; }

protected:
    QRect scaledImageArea;
    ImageDetect* imageDetect;
    TrackPointProperty* trackPointProperty;
    OpenGL::Texture texture;
    QPoint mouse;
    unsigned char* imgBuffer;
    unsigned int bufferSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    bool mouseIn;

    void initializeGL();
    void paintGL();
    void mouseMoveEvent(QMouseEvent* event);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    unsigned int numImageGroupsX;
    unsigned int numImageGroupsY;

    void updateView();
};

#endif
