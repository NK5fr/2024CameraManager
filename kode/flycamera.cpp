#include "flycamera.h"
#include <QDebug>
#include <QPainter>
#include <QRgb>
#include <iostream>

using namespace std;

FlyCamera::FlyCamera()
    : AbstractCamera(), capturing(false)
{
    cam = new Camera();

}

Camera* FlyCamera::getCamera()
{
    return cam;
}

PGRGuid* FlyCamera::getGuid()
{
    return &guid;
}

CameraInfo* FlyCamera::getCameraInfo()
{
    return &camInfo;
}

void FlyCamera::setProperty(CameraManager::CameraProperty* p)
{
    if(p->getType() == CameraManager::AUTOTRIGGER)
    {
        TriggerMode triggerMode;

        cam->GetTriggerMode(&triggerMode);
        triggerMode.onOff = !p->getAuto();
        triggerMode.mode = 0;
        triggerMode.parameter = 0;
        triggerMode.source = 0;
        cam->SetTriggerMode(&triggerMode);

    }else
    {
        Error error;
        Property prop;
        prop.type = getPropertyType(p);
       /* cout << "setProp" << p->getName().c_str() << p->getAuto() << endl;*/
        error = cam->GetProperty(&prop);
        if (error == PGRERROR_OK)
        {
            prop.autoManualMode = p->getAuto();

            prop.absValue = p->getValue();
            prop.valueA = (int) p->getValue();
            prop.valueB = (int) p->getValue();

            cam->SetProperty(&prop);
        }
    }

}
void FlyCamera::updateProperty(CameraManager::CameraProperty* p)
{
    if(p->getType() == CameraManager::AUTOTRIGGER)
    {
        TriggerMode triggerMode;
        cam->GetTriggerMode(&triggerMode);
        p->setAuto(triggerMode.onOff ? false:true);

    }else
    {
        Error error;
        Property prop;
        prop.type = getPropertyType(p);
        error = cam->GetProperty(&prop);
        if (error == PGRERROR_OK)
        {
            p->setAuto(prop.autoManualMode);
            p->setValue(p->getDecimals() > 0 ? prop.absValue : prop.valueA);
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


QImage FlyCamera::captureImage()
{
    Image img;
    Error e=getCamera()->RetrieveBuffer(&img);
    unsigned int x = img.GetCols();
    unsigned int y = img.GetRows();
    unsigned char* picData = img.GetData();
    QImage image(x, y, QImage::Format_RGB32);
    for(unsigned int i = 0; i <y; i++){
        for(unsigned int j = 0; j <x; j++) {
            unsigned char data = picData[i*x+j];
            image.setPixel(j, i, qRgb(data, data, data));
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

QImage FlyCamera::retrieveImage()
{
    cout << "Images begin to be retrieved" << endl;
    TriggerMode triggerMode;
    TriggerMode oldTrigger;

    cam->GetTriggerMode(&oldTrigger);
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

FlyCamera::~FlyCamera()
{
    //dtor
}


