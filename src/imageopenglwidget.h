

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

    void setTrackPointProperty(TrackPointProperty* trackPoint) { this->trackPointProperty = trackPoint; }
    void updateImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);
    const OpenGL::Texture* getTexture() { return &this->texture; }

protected:
    QRect scaledImageArea;

    void initializeGL();
    void paintGL();
    //void resizeGL(int w, int h);
    void mouseMoveEvent(QMouseEvent* event);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    ImageDetect* imageDetect;
    TrackPointProperty* trackPointProperty;
    OpenGL::Texture texture;
    QPoint mouse;
    unsigned char* imgBuffer;
    unsigned int bufferSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int numImageGroupsX;
    unsigned int numImageGroupsY;
    bool mouseIn;

    void updateView();
};

#endif
