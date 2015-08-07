
#include "videoopenglwidget.h"

VideoOpenGLWidget::VideoOpenGLWidget(TrackPointProperty* trackPointProps, QWidget* parent) : ImageOpenGLWidget(trackPointProperty, parent) {
    connect(this, SIGNAL(forceUpdate()), this, SLOT(receiveUpdate()));
}

void VideoOpenGLWidget::receiveUpdate() {
    update();
}

void VideoOpenGLWidget::updateImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {
    ImageOpenGLWidget::updateImage(imgBuffer, bufferSize, imageWidth, imageHeight);
    emit forceUpdate();
}

void VideoOpenGLWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (isMaximized()) showNormal();
    else showMaximized();
}
