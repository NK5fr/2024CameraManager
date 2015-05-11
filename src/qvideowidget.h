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

    void setImage(QImage* image);

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
    void resizeEvent (QResizeEvent* event = NULL);

    void mouseMoveEvent (QMouseEvent* event);
    void enterEvent (QEvent *);
    void leaveEvent (QEvent *);

private:
    QImage* imgTemp;
    QImage img;
    QImage scaledImage;
    QMutex mutex;
    QSize lastSize;
    QRect scaled;
    float ratio;
    bool active;
    bool mouseIn;
    QPoint mouse;
    ImageDetect* imageDetect;
    ImageDetectThread* imgDetThread;
    TrackPointProperty* trackPointProperty;
};

#endif // QVIDEOWIDGET_H
