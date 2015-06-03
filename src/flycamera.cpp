#include "flycamera.h"
#include <QDebug>
#include <QPainter>
#include <QRgb>
#include <iostream>

using namespace std;
using namespace FlyCapture2;

FlyCamera::FlyCamera() : AbstractCamera() {
}

FlyCamera::~FlyCamera() {
    if (capturing) stopAutoCapture();
}

void FlyCamera::setProperty(CameraManager::CameraProperty* p) {
    if(p->getType() == CameraManager::AUTOTRIGGER) {
        TriggerMode triggerMode;
        cam.GetTriggerMode(&triggerMode);
        triggerMode.onOff = !p->getAuto();
        triggerMode.mode = 0;
        triggerMode.parameter = 0;
        triggerMode.source = 0;
        cam.SetTriggerMode(&triggerMode);
    } else {
        Error error;
        Property prop;
        prop.type = getPropertyType(p);
        //printf("setProp - Name: %s, Value: %f, isAuto: %u\n", p->getName().c_str(), p->getValue(), p->getAuto());
        error = cam.GetProperty(&prop);
        if (error == PGRERROR_OK) {
            prop.onOff = p->getOnOff();
            prop.autoManualMode = p->getAuto();
            prop.absControl = (p->getDecimals() > 0);
            prop.absValue = p->getValue();
            prop.valueA = (int) p->getValue();
            prop.valueB = (int) p->getValue();
            cam.SetProperty(&prop);
        } else {
            printf("Error in FlyCamera.cpp - setProperty: \n");
            error.PrintErrorTrace();
        }
    }
}

void FlyCamera::getProperty(CameraManager::CameraProperty* p) {
    if(p->getType() == CameraManager::AUTOTRIGGER) {
        TriggerMode triggerMode;
        cam.GetTriggerMode(&triggerMode);
        p->setAuto(triggerMode.onOff ? false : true);
    } else {
        Error error;
        Property prop;
        prop.type = getPropertyType(p);
        error = cam.GetProperty(&prop);
        if (error == PGRERROR_OK) {
            p->setAuto(prop.autoManualMode);
            p->setValue(p->getDecimals() > 0 ? prop.absValue : prop.valueA);
        } else {
            printf("Error in FlyCamera.cpp - updateProperty: \n");
            error.PrintErrorTrace();
        }
    }
}

FlyCapture2::PropertyType FlyCamera::getPropertyType(CameraManager::CameraProperty* p) {
    switch(p->getType()){
    case CameraManager::BRIGHTNESS:
        return BRIGHTNESS;
        break;
    case CameraManager::EXPOSURE:
        return AUTO_EXPOSURE;
        break;
    case CameraManager::GAIN:
        return GAIN;
        break;
    case CameraManager::GAMMA:
        return GAMMA;
        break;
    case CameraManager::SHUTTER:
        return SHUTTER;
        break;
    case CameraManager::PAN:
        return PAN;
        break;
    case CameraManager::FRAMERATE:
        return FRAME_RATE;
        break;
    case CameraManager::AUTOTRIGGER:
        return TRIGGER_MODE;
        break;
    }
    return BRIGHTNESS;
}

// Lars Aksel - 30.01.2015 - Added support to ImageDetect
Image* FlyCamera::captureImage() {
    Image* image = new Image();
    Error err = getCamera()->RetrieveBuffer(image);
    if (err != PGRERROR_OK) {
        err.PrintErrorTrace();
        delete image;
        return nullptr;
    }
    return image;
}

void FlyCamera::startAutoCapture(){
    capturing = true;
    printf("Starting autoCapture\n");
    getCamera()->StartCapture();
    while(capturing){
        Image* image = captureImage();
        if (image != nullptr) {
            AbstractCamera::sendFrame(image->GetData(), image->GetDataSize(), image->GetCols(), image->GetRows());
            delete image;
        }
    }
    printf("Stopped autoCapture!\n");
}

void FlyCamera::stopAutoCapture(){
    printf("Stopping autoCapture\n");
    capturing = false;
    getCamera()->StopCapture();
}

unsigned char* FlyCamera::retrieveImage(unsigned int* bufferSize, unsigned int* imageWidth, unsigned int* imageHeight) {
    if (capturing) return nullptr;
    capturing = true;
    //printf("Images begin to be retrieved\n");
    TriggerMode triggerMode;
    TriggerMode oldTrigger;

    cam.GetTriggerMode(&oldTrigger);
    cam.GetTriggerMode(&triggerMode);
    triggerMode.onOff = false;
    cam.SetTriggerMode(&triggerMode);

    //printf("Retrieving images...\n");
    cam.StartCapture();

    //printf("Retrieving 1...\n");
    Image* image = captureImage();

    *bufferSize = image->GetDataSize();
    *imageWidth = image->GetCols();
    *imageHeight = image->GetRows();
    unsigned char* imageBuffer = new unsigned char[image->GetDataSize()];
    memcpy(imageBuffer, image->GetData(), image->GetDataSize());
    delete image;
    //printf("Retrieving 2...\n");
    cam.SetTriggerMode(&oldTrigger);

    //printf("Retrieving 3...\n");
    cam.StopCapture();

    //printf("Images retrieved\n");
    capturing = false;
    return imageBuffer;
}

bool FlyCamera::equalsTo(AbstractCamera *c){
    return guid == *((FlyCamera *)c)->getGuid();
}

std::string FlyCamera::getString(){
    ostringstream ss;
    ss << FlyCamera::getCameraInfo()->modelName;
    ss << " - ";
    ss << FlyCamera::getCameraInfo()->serialNumber;
    return ss.str();
}


