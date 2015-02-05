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
#include "qvideowidget.h"
#include "imagedetect.h"
#include "trackpointproperty.h"


/**
 * AbstractCamera
 * \brief Class that need to be subclassed for each camera API. It is used to exchange CameraProperty between the application and the API, to lauch auto capture and receive frames.
 */
class AbstractCamera
{
    public:
        /**
         * @brief (Pure virtual) set the camera according to that property
         * @param p property to set
         */
        virtual void setProperty(CameraManager::CameraProperty* p) = 0;

        /**
         * @brief (Pure virtual) get the value of that property for that camera
         * @param p property to update
         */
        virtual void updateProperty(CameraManager::CameraProperty* p) = 0;

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
        virtual QImage retrieveImage() = 0;

        /**
         * @brief start liveview capture from manager
         * @param videoWidget QvideoWidget that will receive the frames
         */
        void startCapture(QVideoWidget* videoWidget);

        // Lars Aksel - 05.02.2015
        ImageDetect* getImageDetect() { return this->imageDetect; }
        TrackPointProperty* getTrackPointProperty() { return this->trackPointProperty; }
        void setDetectPoints(bool detectPoints) { this->detectPoints = detectPoints; }
        void setTrackPointProperty(TrackPointProperty* prop) { this->trackPointProperty = prop; }
        bool isDetectPoints() { return this->detectPoints; }


    protected:
        AbstractCamera();
        virtual ~AbstractCamera();

        /**
         * @brief sendFrame send a new QImage for the view
         * @param img QImage grabbed from the camera
         */
        void sendFrame(QImage img);

        // Lars Aksel - 30.01.2015
        ImageDetect* imageDetect;
        TrackPointProperty* trackPointProperty;
        bool detectPoints;

    private:
        QVideoWidget* container;
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
