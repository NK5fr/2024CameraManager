#ifndef FLYCAMERA_H
#define FLYCAMERA_H

#include "abstractcamera.h"
#include <iostream>
#include <sstream>

#if defined(WIN64) || defined(WIN32)
	#include "FlyCapture2.h"
#elif defined(__unix__)
	#include "flycapture/FlyCapture2.h"
#endif

using namespace FlyCapture2;
using namespace std;

/**
 * @brief The FlyCamera class, represent a FlyCapture Camera with all its settings
 */

class FlyCamera : public AbstractCamera {
public:
    FlyCamera();
    ~FlyCamera();

    Camera* getCamera();        
    PGRGuid* getGuid();        
    CameraInfo* getCameraInfo();

    void setProperty(CameraManager::CameraProperty* p);
    void getProperty(CameraManager::CameraProperty* p);

	void startAutoCapture();
    void stopAutoCapture();

    inline QString getSerial() { return QString::number(camInfo.serialNumber); }
    inline QString getModel() { return QString(camInfo.modelName); }

    QImage retrieveImage();
        
    bool equalsTo(AbstractCamera *c);        
	std::string getString();

private:
    /**
        * @brief cam object that represent the camera
        */
    Camera* cam;
    /**
        * @brief guid : unique ID for each camera
        */
    PGRGuid guid;
    /**
        * @brief camInfo : object that gather all camera informations (model name, serial number, ...)
        */
    CameraInfo camInfo;
    /**
        * @brief getPropertyType
        * @param p
        * @return
        */
	FlyCapture2::PropertyType getPropertyType(CameraManager::CameraProperty *p);

    /**
        * @brief captureImage : get an image from the camera and convert it in QImage type
        * @return a QImage object
        */
    QImage captureImage();
};

#endif // FLYCAMERA_H
