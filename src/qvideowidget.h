/**
 * \file qvideowidget.h
 * \author Virgile Wozny
 */


#ifndef QVIDEOWIDGET_H
#define QVIDEOWIDGET_H

#include <QWidget>
#include <QThread>
#include <QMutex>

/**
 * QVideoWidget
 * \brief display images from cameras, handle resizing and crosshair display.
 */
class QVideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QVideoWidget(QWidget *parent = 0);
    ~QVideoWidget(){}

    void setImage (QImage image);

signals:
    void forceUpdate();
public slots:
    void changedState (Qt::WindowStates oldState, Qt::WindowStates newState);
    void activateCrosshair(bool state);
    void receiveUpdate();

protected:
    void paintEvent (QPaintEvent * event);
    void resizeEvent (QResizeEvent * event = NULL);

    void mouseMoveEvent (QMouseEvent * event);
    //void enterEvent (QEvent *);
    void leaveEvent (QEvent *);

private:
    QImage img;
    QMutex mutex;
    QSize lastSize;
    QRect scaled;
    float ratio;
    bool active;
    bool mouseIn;
    QPoint mouse;
};

#endif // QVIDEOWIDGET_H
