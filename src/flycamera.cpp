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
QImage* FlyCamera::captureImage() {
    Image img;
    Error err = getCamera()->RetrieveBuffer(&img);
    if (err != PGRERROR_OK) {
        err.PrintErrorTrace();
        return nullptr;
    }
    quint64 val;
    QImage* image = new QImage(img.GetCols(), img.GetRows(), QImage::Format::Format_RGB32);
    //unsigned char* imgBuffer = image->bits();
    quint64* resBuffer = (quint64*) image->bits();
    quint64* flyCapData = (quint64*) img.GetData();
    unsigned int bufferSize = (img.GetCols() * img.GetRows()) / sizeof(quint64);
    for (int i = 0; i < bufferSize; ++i) {
        val = flyCapData[i];

        // 1 byte of 8 bytes
        resBuffer[(i * 4)] |= (val & 0xFF);
        resBuffer[(i * 4)] |= (val & 0xFF) >> 8;
        resBuffer[(i * 4)] |= (val & 0xFF) >> 16;
        resBuffer[(i * 4)] |= 0xFF00000000000000 >> 24;

        // 2 byte of 8 bytes
        resBuffer[(i * 4)] |= (val & 0x00FF) >> 32;
        resBuffer[(i * 4)] |= (val & 0x00FF) >> 40;
        resBuffer[(i * 4)] |= (val & 0x00FF) >> 48;
        resBuffer[(i * 4)] |= 0xFF00000000000000 >> 56;

        // 3 byte of 8 bytes
        resBuffer[(i * 4) + 1] |= (val & 0x0000FF);
        resBuffer[(i * 4) + 1] |= (val & 0x0000FF) >> 8;
        resBuffer[(i * 4) + 1] |= (val & 0x0000FF) >> 16;
        resBuffer[(i * 4) + 1] |= 0xFF >> 24;

        // 4 byte of 8 bytes
        resBuffer[(i * 4) + 1] |= (val & 0x000000FF) >> 32;
        resBuffer[(i * 4) + 1] |= (val & 0x000000FF) >> 40;
        resBuffer[(i * 4) + 1] |= (val & 0x000000FF) >> 48;
        resBuffer[(i * 4) + 1] |= 0xFF00000000000000 >> 56;

        // 5 byte of 8 bytes
        resBuffer[(i * 4) + 2] |= (val & 0x00000000FF);
        resBuffer[(i * 4) + 2] |= (val & 0x00000000FF) >> 8;
        resBuffer[(i * 4) + 2] |= (val & 0x00000000FF) >> 16;
        resBuffer[(i * 4) + 2] |= 0xFF00000000000000 >> 24;

        // 6 byte of 8 bytes
        resBuffer[(i * 4) + 2] |= (val & 0x0000000000FF) >> 32;
        resBuffer[(i * 4) + 2] |= (val & 0x0000000000FF) >> 40;
        resBuffer[(i * 4) + 2] |= (val & 0x0000000000FF) >> 48;
        resBuffer[(i * 4) + 2] |= 0xFF00000000000000 >> 56;

        // 7 byte of 8 bytes
        resBuffer[(i * 4) + 3] |= (val & 0x000000000000FF);
        resBuffer[(i * 4) + 3] |= (val & 0x000000000000FF) >> 8;
        resBuffer[(i * 4) + 3] |= (val & 0x000000000000FF) >> 16;
        resBuffer[(i * 4) + 3] |= 0xFF00000000000000 >> 24;

        // 8 byte of 8 bytes
        resBuffer[(i * 4) + 3] |= (val & 0x00000000000000FF) >> 32;
        resBuffer[(i * 4) + 3] |= (val & 0x00000000000000FF) >> 40;
        resBuffer[(i * 4) + 3] |= (val & 0x00000000000000FF) >> 48;
        resBuffer[(i * 4) + 3] |= 0xFF00000000000000 >> 56;

        /*
        imgVal = img.GetData()[i];
        imgBuffer[(i * 4) + 0] = imgVal;  // Red
        imgBuffer[(i * 4) + 1] = imgVal;  // Green
        imgBuffer[(i * 4) + 2] = imgVal;  // Blue
        imgBuffer[(i * 4) + 3] = 255;     // Alpha
        */
    }
    return image;
}

void FlyCamera::startAutoCapture(){
    capturing = true;
    printf("Starting autoCapture\n");
    getCamera()->StartCapture();
    while(capturing){
        QImage* image = captureImage();
        AbstractCamera::sendFrame(image);
    }
    printf("Stopped autoCapture !\n");
}

void FlyCamera::stopAutoCapture(){
    cout << "Stopping autoCapture";
    capturing = false;
    getCamera()->StopCapture();
}

QImage* FlyCamera::retrieveImage() {
    if (capturing) return nullptr;
    capturing = true;
    printf("Images begin to be retrieved\n");
    TriggerMode triggerMode;
    TriggerMode oldTrigger;

    cam.GetTriggerMode(&oldTrigger);
    cam.GetTriggerMode(&triggerMode);
    triggerMode.onOff = false;
    cam.SetTriggerMode(&triggerMode);

    printf("Retrieving images...\n");
    cam.StartCapture();


    printf("Retrieving 1...\n");
    QImage* image = captureImage();

    printf("Retrieving 2...\n");
    cam.SetTriggerMode(&oldTrigger);

    printf("Retrieving 3...\n");
    cam.StopCapture();

    printf("Images retrieved\n");
    capturing = false;
    return image;
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


