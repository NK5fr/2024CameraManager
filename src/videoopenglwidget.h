
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef VIDEO_OPENGL_WIDGET_H
#define VIDEO_OPENGL_WIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtCore/qmath.h>

#include "imageopenglwidget.h"

class VideoOpenGLWidget : public ImageOpenGLWidget {
    Q_OBJECT
public:
    VideoOpenGLWidget(TrackPointProperty* trackPointProps = nullptr, QWidget* parent = nullptr);

    void updateImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);

signals:
    void forceUpdate();

public slots:
    void receiveUpdate();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event);
};

#endif // VIDEO_OPENGL_WIDGET_H
