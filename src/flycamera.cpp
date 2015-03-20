#include "flycamera.h"
#include <QDebug>
#include <QPainter>
#include <QRgb>
#include <iostream>

using namespace std;

FlyCamera::FlyCamera() : AbstractCamera() {
    cam = new Camera();
}

Camera* FlyCamera::getCamera() {
    return cam;
}

PGRGuid* FlyCamera::getGuid() {
    return &guid;
}

CameraInfo* FlyCamera::getCameraInfo() {
    return &camInfo;
}

void FlyCamera::setProperty(CameraManager::CameraProperty* p) {
    if(p->getType() == CameraManager::AUTOTRIGGER) {
        TriggerMode triggerMode;
        cam->GetTriggerMode(&triggerMode);
        triggerMode.onOff = !p->getAuto();
        triggerMode.mode = 0;
        triggerMode.parameter = 0;
        triggerMode.source = 0;
        cam->SetTriggerMode(&triggerMode);
    } else {
        Error error;
        Property prop;
        prop.type = getPropertyType(p);
        //printf("setProp - Name: %s, Value: %f, isAuto: %u\n", p->getName().c_str(), p->getValue(), p->getAuto());
        error = cam->GetProperty(&prop);
        if (error == PGRERROR_OK) {
            prop.onOff = p->getOnOff();
            prop.autoManualMode = p->getAuto();
            prop.absControl = (p->getDecimals() > 0);
            prop.absValue = p->getValue();
            prop.valueA = (int) p->getValue();
            prop.valueB = (int) p->getValue();
            cam->SetProperty(&prop);
        } else {
            printf("Error in FlyCamera.cpp - setProperty: \n");
            error.PrintErrorTrace();
        }
    }

}
void FlyCamera::getProperty(CameraManager::CameraProperty* p) {
    if(p->getType() == CameraManager::AUTOTRIGGER) {
        TriggerMode triggerMode;
        cam->GetTriggerMode(&triggerMode);
        p->setAuto(triggerMode.onOff ? false : true);
    } else {
        Error error;
        Property prop;
        prop.type = getPropertyType(p);
        error = cam->GetProperty(&prop);
        if (error == PGRERROR_OK) {
            p->setAuto(prop.autoManualMode);
            p->setValue(p->getDecimals() > 0 ? prop.absValue : prop.valueA);
        } else {
            printf("Error in FlyCamera.cpp - updateProperty: \n");
            error.PrintErrorTrace();
        }
    }
}

FlyCapture2::PropertyType FlyCamera::getPropertyType(CameraManager::CameraProperty* p)
{
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
QImage FlyCamera::captureImage() {
    Image img;
    Error err = getCamera()->RetrieveBuffer(&img);
    if (err != PGRERROR_OK) {
        err.PrintErrorTrace();
        return QImage();
    }
    unsigned int width = img.GetCols();
    unsigned int height = img.GetRows();
    unsigned char* imageBuffer = img.GetData();
    QImage image(width, height, QImage::Format_RGB32);
    unsigned char* imgData = image.bits();
    unsigned char imgVal = 0;
    if (imageDetect == nullptr) {
        imageDetect = new ImageDetect(width, height,
                                        trackPointProperty->thresholdValue,
                                        trackPointProperty->subwinValue);
    }
    imageDetect->setThreshold(trackPointProperty->thresholdValue);
    imageDetect->setMinPix(trackPointProperty->minPointValue);
    imageDetect->setMaxPix(trackPointProperty->maxPointValue);
    imageDetect->setSubwinSize(trackPointProperty->subwinValue);
    imageDetect->setMinSep(trackPointProperty->minSepValue);
    imageDetect->setImage(imageBuffer);
    if (trackPointProperty->filteredImagePreview) {
        imageDetect->imageRemoveBackground();
        imageBuffer = imageDetect->getFilteredImage();
    }
    for (int i = 0; i < img.GetDataSize(); ++i) {
        imgVal = imageBuffer[i];
        imgData[(i * 4) + 0] = imgVal;  // Red
        imgData[(i * 4) + 1] = imgVal;  // Green
        imgData[(i * 4) + 2] = imgVal;  // Blue
        imgData[(i * 4) + 3] = 255;     // Alpha
    }
    if (trackPointProperty->trackPointPreview) {
        imageDetect->imageDetectPoints();
        imageDetect->removeDuplicatePoints();
        ImPoint* points = imageDetect->getFinalPoints();
        // Drawing the points on the image...
        int crossWingSize = (int) (height / 75);
        for (int i = 0; i < imageDetect->getFinalNumPoints(); i++) {
            int xPos = points[i].x;
            int yPos = points[i].y;
            // Line along X-axis
            for (int x = xPos - crossWingSize; x <= xPos + crossWingSize; x++) {
                image.setPixel(x, yPos, qRgb(255, 0, 0));
            }
            // Line along Y-axis
            for (int y = yPos - crossWingSize; y <= yPos + crossWingSize; y++) {
                image.setPixel(xPos, y, qRgb(255, 0, 0));
            }
        }
    }
    return image;
}

void FlyCamera::startAutoCapture(){
    capturing = true;
    cout << "Starting autoCapture" << endl;
    getCamera()->StartCapture();
    while(capturing){
        QImage image = captureImage();
        AbstractCamera::sendFrame(image);
    }
    cout << "Stopped autoCapture !";
}

void FlyCamera::stopAutoCapture(){
    cout << "Stopping autoCapture";
    capturing = false;
    getCamera()->StopCapture();
}

QImage FlyCamera::retrieveImage() {
    if (capturing) return QImage();
    capturing = true;
    cout << "Images begin to be retrieved" << endl;
    TriggerMode triggerMode;
    TriggerMode oldTrigger;

    cam->GetTriggerMode(&oldTrigger);
    cam->GetTriggerMode(&triggerMode);
    triggerMode.onOff = false;
    cam->SetTriggerMode(&triggerMode);

    cout << "Retrieving images..." << endl;
    cam->StartCapture();


    cout << "Retrieving 1..." << endl;
    QImage image = captureImage();

    cout << "Retrieving 2..." << endl;
    cam->SetTriggerMode(&oldTrigger);

    cout << "Retrieving 3..." << endl;
    cam->StopCapture();

    cout << "Images retrieved" << endl;
    capturing = false;
    return image;
}

bool FlyCamera::equalsTo(AbstractCamera *c){
    return guid == *((FlyCamera *)c)->getGuid();
}

std::string FlyCamera::getString(){
    string name = FlyCamera::getCameraInfo()->modelName;
    ostringstream refTmp;
    refTmp << FlyCamera::getCameraInfo()->serialNumber;
    string ref = refTmp.str();

    return name + " - " + ref;
}

FlyCamera::~FlyCamera() {
    if (capturing) stopAutoCapture();
}


