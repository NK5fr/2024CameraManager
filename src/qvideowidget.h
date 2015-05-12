/**
 * \file qvideowidget.h
 * \author Virgile Wozny
 */


#ifndef QVIDEOWIDGET_H
#define QVIDEOWIDGET_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include "imagedetect.h"
#include "trackpointproperty.h"

/**
 * QVideoWidget
 * \brief display images from cameras, handle resizing and crosshair display.
 */
class QVideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit QVideoWidget(QWidget *parent = 0);
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
    void paintEvent (QPaintEvent* event);
    void resizeEvent (QResizeEvent* event = nullptr);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent (QMouseEvent* event);
    void enterEvent (QEvent*);
    void leaveEvent (QEvent*);

private:
    QImage img;
    QImage scaledImage;
    QMutex mutex;
    QSize lastSize;
    QRect scaled;
    unsigned char* imgBuffer;
    unsigned int bufferSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    float ratio;
    bool active;
    bool mouseIn;
    QPoint mouse;
    ImageDetect* imageDetect;
    ImageDetectThread* imgDetThread;
    TrackPointProperty* trackPointProperty;

    void setQImage(QImage& img, unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);
};

#endif // QVIDEOWIDGET_H
