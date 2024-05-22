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

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

class SpinCamera : public AbstractCamera{
public:
    SpinCamera(Spinnaker::CameraPtr pCam);
    ~SpinCamera();

    //Constant to get the trigger source
    //wrote on 12/06/2019 by French students
    static int trigger;

    //Constant to enable or disable trigger
    //20/05/2024 written  by Armand & Nathan
    static int enableTrigger;

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
    unsigned char* retrieveImage(unsigned int* bufferSize, unsigned int* imageWidth, unsigned int* imageHeight, bool colored = false);

    bool equalsTo(AbstractCamera *c);
    std::string getString();


private:
    Spinnaker::CameraPtr cam;
    Spinnaker::ImagePtr captureImage(bool colored = false);

};

#endif // SPINCAMERA_H
