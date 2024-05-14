#ifndef SPINCAMERA_H
#define SPINCAMERA_H

#include "abstractcamera.h"
#include <iostream>
#include <sstream>
#include "spincameraproperty.h"

#if defined(WIN64) || defined(WIN32)
#include <include/Spinnaker/Spinnaker.h>
#include <include/Spinnaker/SpinGenApi/SpinnakerGenApi.h>
#endif
#if defined(__unix__) || defined(__linux__)
#include <include/Spinnaker/Spinnaker.h>
#include <include/Spinnaker/SpinGenApi/SpinnakerGenApi.h>
#endif

using namespace std;
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

class SpinCamera : public AbstractCamera{
public:
    SpinCamera(Spinnaker::CameraPtr pCam);
    ~SpinCamera();

    //Constant to get the trigger mode
    //wrote on 12/06/2019 by French students
    static int trigger;

    inline Spinnaker::CameraPtr getCamera() {
        return cam;
    }
    inline QString getSerial() {
        Spinnaker::GenApi::CStringPtr ptrStringSerial = getCamera()->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
        return QString(ptrStringSerial->GetValue());
    }
    inline QString getModel() {
        Spinnaker::GenApi::CStringPtr ptrStringModel = getCamera()->GetTLDeviceNodeMap().GetNode("DeviceModelName");
        return QString(ptrStringModel->GetValue());
    }
    void setSpinProperty(CameraManagerSpin::SpinCameraProperty* p);
    void setProperty(CameraManager::CameraProperty* p);
    void startAutoCapture();
    void stopAutoCapture();
    int ConfigureTrigger(INodeMap & nodeMap);
    unsigned char* retrieveImage(unsigned int* bufferSize, unsigned int* imageWidth, unsigned int* imageHeight);

    bool equalsTo(AbstractCamera *c);
    std::string getString();


private:
    Spinnaker::CameraPtr cam;
    Spinnaker::ImagePtr captureImage();

};

#endif // SPINCAMERA_H
