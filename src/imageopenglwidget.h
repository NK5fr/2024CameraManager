
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
    void setNumSubImagesX(unsigned int numSubImagesX) { this->numImageGroupsX = numSubImagesX; }
    void setNumSubImagesY(unsigned int numSubImagesY) { this->numImageGroupsY = numSubImagesY; }
    void setEnableSubImage(bool enableSubImage) { this->enableSubImages = enableSubImage; }

    const OpenGL::Texture* getTexture() { return &this->texture; }
    unsigned int getNumSubImagesX() { return this->numImageGroupsX; }
    unsigned int getNumSubImagesY() { return this->numImageGroupsY; }
    bool isEnableSubImage() { return this->enableSubImages; }

protected:
    QRect scaledImageArea;
    ImageDetect* imageDetect;
    TrackPointProperty* trackPointProperty;
    OpenGL::Texture texture;
    QPoint mousePos;
    QPoint mousePosInImage;
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
    bool enableSubImages;
    //bool showSubImageGrid;
    bool showMouseOverCoordinateLabel;
    bool showMouseCross;
    unsigned int numImageGroupsX;
    unsigned int numImageGroupsY;

    void updateView();
};

#endif
