#ifndef ISCAMERA_H
#define ISCAMERA_H

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

class IsCamera : public AbstractCamera
{
public:
    IsCamera();
    virtual ~IsCamera();

    Camera* getCamera();
    PGRGuid* getGuid();
    CameraInfo* getCameraInfo();

    void setProperty(CameraManager::CameraProperty *p);
    void getProperty(CameraManager::CameraProperty *p);
    void startAutoCapture();
    void stopAutoCapture();
    void sendFrame(QImage img);
    QImage retrieveImage();
    bool equalsTo(AbstractCamera *c);
    std::string getString();

private:
    Camera* cam;
    PGRGuid guid;
    CameraInfo camInfo;
    FlyCapture2::PropertyType getPropertyType(CameraManager::CameraProperty *p);
    bool capturing;
};

#endif // FLYCAMERA_H
