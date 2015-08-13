
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
#include "datastructs.h"

class ImageOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {

public:
    ImageOpenGLWidget(TrackPointProperty* trackPointProps = nullptr, QWidget* parent = nullptr);
    ~ImageOpenGLWidget();

    void setTrackPointProperty(TrackPointProperty* trackPoint) { this->trackPointProperty = trackPoint; }
    virtual void updateImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);
    void setNumSubImagesX(unsigned int numSubImagesX) { this->numImageGroupsX = numSubImagesX; }
    void setNumSubImagesY(unsigned int numSubImagesY) { this->numImageGroupsY = numSubImagesY; }
    void setEnableSubImage(bool enableSubImage) { this->enableSubImages = enableSubImage; }
    void setZoomAreaSize(double zoomSize) { this->zoomSize = zoomSize; }
    void setZoomFactor(double zoomFactor) { this->zoomFactor = zoomFactor; }

    const OpenGL::Texture* getTexture() { return &this->texture; }
    unsigned int getNumSubImagesX() { return this->numImageGroupsX; }
    unsigned int getNumSubImagesY() { return this->numImageGroupsY; }
    unsigned int getImageWidth() { return this->imageWidth; }
    unsigned int getImageHeight() { return this->imageHeight; }
    bool isEnableSubImage() { return this->enableSubImages; }

protected:
    QRect scaledImageArea;
    ImageDetect* imageDetect;
    TrackPointProperty* trackPointProperty;
    OpenGL::Texture texture;
    QPointF actualMousePos;
    QPointF mousePos;
    QPointF mousePosInImage;
    unsigned char* imgBuffer;
    unsigned int bufferSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int numImageGroupsX;
    unsigned int numImageGroupsY;
    bool mouseIn;
    bool enableSubImages;
    bool showMouseOverCoordinateLabel;
    bool showMouseCross;
    bool showBoundingAreas;

    void initializeGL();
    void paintGL();
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    vector<CircleF*> boundingCircles;
    vector<QRectF*> boundingBoxes;
    bool newImageReady;

    QPointF mouseDragStart;
    bool leftMouseButtonDown = false;
    //bool rightMouseButtonDown = false;

    int selectedBoundingCircle = -1;
    bool selectedBoundingCircleEdge = false; // If edge is selected
    int selectedBoundingBox = -1;
    int selectedBoundingBoxCorner = -1; // 0-topLeft, 1-topRight, 2-bottomLeft, 3-bottomRight
    QPointF selectedBoundingAreaMouseRelative;

    double boundingCircleEdgeThreshold = 2; // In pixels
    double boundingBoxCornerThreshold = 10; // In pixels
    double mouseDragThreshold = 5; // Not used
  
    bool showZoomArea = false; // Drawing of zoom-area at cursor, controlled by holding left mouse-button
    double zoomFactor = 4.0; // Zooming-factor, 
    double zoomSize = 200.0; // Size of zooming-area

    void dragBoundingArea();
    void removeBoundingArea(QPointF& mousePos);
    void checkBoundingAreas();
    void updateView();
};

#endif
