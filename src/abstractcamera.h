/**
 * \file abstractcamera.h
 * \author Virgile Wozny
 */

#ifndef ABSTRACTCAMERA_H
#define ABSTRACTCAMERA_H

#include <string>
#include <QImage>
#include <QThread>
#include <QDebug>
#include "cameraproperty.h"
#include "videoopenglwidget.h"

/**
 * AbstractCamera
 * \brief Class that need to be subclassed for each camera API. It is used to exchange CameraProperty between the application and the API, to lauch auto capture and receive frames.
 */
class AbstractCamera {
public:
    virtual ~AbstractCamera();
    /**
        * @brief (Pure virtual) set the camera according to that property
        * @param p property to set
        */
    virtual void setProperty(CameraManager::CameraProperty* p) = 0;

    /**
        * @brief (Pure virtual) get the value of that property for that camera
        * @param p property to update
        */
    virtual void getProperty(CameraManager::CameraProperty* p) = 0;

    /**
        * @brief compare 2 cameras
        * @param c camera to compare
        * @return true if the cameras are physically the same
        */
    virtual bool equalsTo(AbstractCamera* c);

    /**
        * @brief (Pure virtual) get the name corresponding to the camera model and id
        * @return String containing these informations
        */
    virtual std::string getString() = 0;
    virtual inline QString getSerial() { return serial; }
    virtual inline QString getModel() { return model; }
    virtual inline QString getCustomName() { return customName; }
    virtual inline void setCustomName(QString name) { this->customName = name; }

    /**
        * @brief (Pure virtual) start callback based Liveview
        */
    virtual void startAutoCapture() = 0;

    /**
        * @brief (Pure virtual) stopAutoCapture stop callback based Liveview
        */
    virtual void stopAutoCapture() = 0;

    /**
        * @brief (Pure virtual) get one image from camera
        * @return QImage image
        */
    virtual unsigned char* retrieveImage(unsigned int* bufferSize, unsigned int* imageWidth, unsigned int* imageHeight) = 0;

    /**
        * @brief start liveview capture from manager
        * @param videoWidget VideoOpenGLWidget that will receive the frames
        */
    void startCapture(VideoOpenGLWidget* videoWidget);

    // Lars Aksel - 05.02.2015
    bool isCapturing() { return this->capturing; }
    VideoOpenGLWidget* getVideoContainer() { return this->container; }
    void setVideoContainer(VideoOpenGLWidget* videoWidget) { this->container = videoWidget; }

protected:
    AbstractCamera();

    /**
        * @brief sendFrame send a new QImage for the view
        * @param img QImage grabbed from the camera
        */
    void sendFrame(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight);

    // Lars Aksel - 30.01.2015
    bool capturing;

private:
    QString customName;
    QString serial;
    QString model;
    VideoOpenGLWidget* container;
    class CaptureThread : public QThread {
        public :
            CaptureThread(AbstractCamera* cam) : QThread() { c = cam; }
            void run(){
                /*qDebug() << "Thread Started";*/
                c->startAutoCapture();
            }
        private:
            AbstractCamera* c;
    };
    CaptureThread thread;
};

#endif // ABSTRACTCAMERA_H
