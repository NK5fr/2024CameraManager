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

class FlyCamera : public AbstractCamera {
public:
    FlyCamera();
    ~FlyCamera();

    inline FlyCapture2::Camera* getCamera() { return &cam; }
    inline FlyCapture2::PGRGuid* getGuid() { return &guid; }
    inline FlyCapture2::CameraInfo* getCameraInfo() { return &camInfo; }
    inline QString getSerial() { return QString::number(camInfo.serialNumber); }
    inline QString getModel() { return QString(camInfo.modelName); }

    void setProperty(CameraManager::CameraProperty* p);
    void getProperty(CameraManager::CameraProperty* p);

	void startAutoCapture();
    void stopAutoCapture();
    QImage* retrieveImage();
        
    bool equalsTo(AbstractCamera *c);        
	  std::string getString();

private:
    FlyCapture2::Camera cam;
    FlyCapture2::PGRGuid guid;
    FlyCapture2::CameraInfo camInfo;

	FlyCapture2::PropertyType getPropertyType(CameraManager::CameraProperty *p);

    QImage* captureImage();
};

#endif // FLYCAMERA_H
