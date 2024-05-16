#include "spincamera.h"
#include <QDebug>
#include <QPainter>
#include <QRgb>
#include <iostream>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

SpinCamera::SpinCamera(Spinnaker::CameraPtr pCam){
    cam = pCam;
    cam->Init();

}
SpinCamera::~SpinCamera() {
    if(capturing) stopAutoCapture();
}

void SpinCamera::setProperty(CameraManager::CameraProperty *p) {

}

void SpinCamera::setSpinProperty(CameraManagerSpin::SpinCameraProperty* p) {
    switch(p->getType()) { //Get the type of the property
    case CameraManagerSpin::AUTOTRIGGER :

        if(p->getAuto()){
            trigger = 0;
        }else{
            trigger = 1;
        }
        break;
    case CameraManagerSpin::BLACKLEVEL :
        try{
        if (cam->BlackLevel == NULL || cam->BlackLevel.GetAccessMode() != RW){ //Check if the property can be accessed on the camera
            std::cout << "Unable to access the black level property."<< std::endl;
            return ;
        }
        cam->BlackLevel.SetValue(p->getValue()); //Set the wanted value to the camera property
    }catch (Spinnaker::Exception &e){
            std::cout << "Error: " << e.what() << std::endl;
        }
        break;
    case CameraManagerSpin::EXPOSURETIME :
        try{
        if(cam->ExposureAuto == NULL || cam->ExposureAuto.GetAccessMode() != RW){ //Check if the auto can be disabled
            std::cout << "Unable to access the exposure auto property." << std::endl;
        }else{
            if(p->getAuto() && (cam->ExposureAuto.GetValue() == ExposureAuto_Off)){ //Check if the auto status on the camera match the auto status of the wanter property
                cam->ExposureAuto.SetValue(ExposureAuto_Continuous);
            }else if(!p->getAuto() && (cam->ExposureAuto.GetValue() != ExposureAuto_Off)){
                cam->ExposureAuto.SetValue(ExposureAuto_Off);
            }else if(!p->getAuto()){ //If auto is disabled
                if(cam->ExposureTime == NULL || cam->ExposureTime.GetAccessMode() != RW){ //And the node available
                    std::cout << "Unable to access the exposure property." << std::endl;
                }else{
                    if (p->getValue() >= cam->ExposureTime.GetMin() && p->getValue() <= cam->ExposureTime.GetMax()) {
                        cam->ExposureTime.SetValue(p->getValue()); // Set the value
                    } else {
                        std::cout << "Wanted exposure value was not within the limits, setting to minimum available value..." << std::endl;
                        cam->ExposureTime.SetValue(cam->ExposureTime.GetMin());
                    }
                }
            }
        }

    }catch (Spinnaker::Exception &e){
            std::cout << "Error : " << e.what() << std::endl;
        }
        break;
    case CameraManagerSpin::GAIN :
        try{
        if(cam->GainAuto == NULL || cam->GainAuto.GetAccessMode() != RW){
            std::cout << "Unable to access the gain auto property." << std::endl;
        }else{
            if(p->getAuto() && (cam->GainAuto.GetValue() == GainAuto_Off)){
                cam->GainAuto.SetValue(GainAuto_Continuous);
            }else if(!p->getAuto() && (cam->GainAuto.GetValue() != GainAuto_Off)){
                cam->GainAuto.SetValue(GainAuto_Off);
            }else if(!p->getAuto()){
                if(cam->Gain == NULL || cam->Gain.GetAccessMode() != RW){
                    std::cout << "Unable to access the gain property." << std::endl;
                }else{
                    cam->Gain.SetValue(p->getValue());
                }
            }
        }
    }catch (Spinnaker::Exception &e){
            std::cout << "Error: " << e.what() << std::endl;
        }
        break;

    case CameraManagerSpin::GAMMA :
        try{
        if(cam->GammaEnable == NULL || cam->GammaEnable.GetAccessMode() != RW){
            std::cout << "Unable to access the gamma auto property." << std::endl;
        }else{
            if(p->getAuto() && (cam->GammaEnable.GetValue() == 1)){
                cam->GammaEnable.SetValue(0);
            }else if(!p->getAuto() && (cam->GammaEnable.GetValue() != 0)){
                cam->GammaEnable.SetValue(1);
            }else if(!p->getAuto()){
                if(cam->Gamma == NULL || cam->Gamma.GetAccessMode() != RW){
                    std::cout << "Unable to access the gamma property." << std::endl;
                }else{
                    cam->Gamma.SetValue(p->getValue());
                }
            }
        }
    }catch (Spinnaker::Exception &e){
            std::cout << "Error: " << e.what() << std::endl;
        }
        break;

    case CameraManagerSpin::FRAMERATE :
        try{
        CBooleanPtr acquisitionFrameRateAutoNode = cam->GetNodeMap().GetNode("AcquisitionFrameRateEnable");

        if(!IsAvailable(acquisitionFrameRateAutoNode) && !IsWritable(acquisitionFrameRateAutoNode)){
            std::cout << "Unable to access the frame rate auto property." << std::endl;
        }else{
            if(p->getAuto() && (acquisitionFrameRateAutoNode->GetValue() == 1)){
                acquisitionFrameRateAutoNode->SetValue(0);
            }else if(!p->getAuto() && (acquisitionFrameRateAutoNode->GetValue() == 0)){
                acquisitionFrameRateAutoNode->SetValue(1);
            }else if(!p->getAuto()){
                if (cam->AcquisitionFrameRate == NULL || cam->AcquisitionFrameRate.GetAccessMode() != RW){
                    std::cout << "Unable to access the frame rate property." << std::endl << std::endl;
                }else{
                    cam->AcquisitionFrameRate.SetValue(p->getValue());
                }
            }
        }

    }catch (Spinnaker::Exception &e){
            std::cout << "Error: " << e.what() << std::endl;
        }
        break;
    }
}


// Lars Aksel - 30.01.2015 - Added support to ImageDetect
ImagePtr SpinCamera::captureImage() {
    if(trigger==0){

        std::cout << "Trigger Software is not available, you can't activate Auto Trigger" << std::endl; // Auto Trigger is not available, if you need it please uncomment the seven following lines

    // Armand & Nathan 10/05/2024 : Auto Trigger disable because not available with our cameras

    // Execute software trigger
    // CCommandPtr ptrSoftwareTriggerCommand = cam->GetNodeMap().GetNode("TriggerSoftware");
    // if (!IsAvailable(ptrSoftwareTriggerCommand) || !IsWritable(ptrSoftwareTriggerCommand))
    // {
    //     cout << "Unable to execute trigger. Aborting..." << endl;
    //     return -1;
    // }

    // ptrSoftwareTriggerCommand->Execute();
    }
    ImagePtr image = nullptr;
    ImagePtr convertedImage = nullptr;
    try {
        if ( cam->IsStreaming()) {
            ImageProcessor processor;
            image = cam->GetNextImage();
            convertedImage = processor.Convert(image, PixelFormat_Mono8);

        } else {
            std::cout <<" not streaming" << std::endl;
        }
        return convertedImage;

    }catch(Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }


}

// Hugo Fournier - 3.06.2019 - Trigger Configuration Spinnaker
int SpinCamera::ConfigureTrigger(INodeMap &nodeMap) {
    int result = 0;
    std::cout << std::endl << std::endl << "*** CONFIGURING TRIGGER ***" << std::endl << std::endl;
    try
    {
        // Ensure trigger mode off
        //
        // *** NOTES ***
        // The trigger must be disabled in order to configure whether the source
        // is software or hardware.
        //
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
        {
            std::cout << "Unable to disable trigger mode (node retrieval). Aborting..." << std::endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
        if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
        {
            std::cout << "Unable to disable trigger mode (enum entry retrieval). Aborting..." << std::endl;
            return -1;
        }

        ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

        std::cout << "Trigger mode disabled..." << std::endl;

        if(trigger==1){
            if (cam->TriggerSource == NULL || cam->TriggerSource.GetAccessMode() != RW){
                std::cout << "Unable to set trigger mode (node retrieval). Aborting..." << std::endl;
                return -1;
            }

            cam->TriggerSource.SetValue(TriggerSource_Line0);

            std::cout << "Trigger source set to hardware..." << std::endl;
        }else{
            if (cam->TriggerSource == NULL || cam->TriggerSource.GetAccessMode() != RW){
                std::cout << "Unable to set trigger mode (node retrieval). Aborting..." << std::endl;
                return -1;
            }

            cam->TriggerSource.SetValue(TriggerSource_Software);

            std::cout << "Trigger source set to software..." << std::endl;
        }

        // Turn trigger mode on
        //
        // *** LATER ***
        // Once the appropriate trigger source has been set, turn trigger mode
        // on in order to retrieve images using the trigger.

        // Armand & Nathan 10/05/2024 : disabled TriggerMode, because we have nothing that could enable trigger this way by default
        // if (trigger == 1) {
        //     CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
        //     if (!IsAvailable(ptrTriggerModeOn) || !IsReadable(ptrTriggerModeOn))
        //     {
        //         cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << endl;
        //         return -1;
        //     }

        //     ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());

        //     cout << "Trigger mode turned back on..." << endl << endl;
        // }

    }
    catch (Spinnaker::Exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "erreur dans trigger" << std::endl;
        result = -1;
    }
    return result;

}

int SpinCamera::trigger = 0;

//Start the AutoCapture
//wrote on 11/06/2019 by French students
void SpinCamera::startAutoCapture(){
    capturing = true;
    if (!cam->IsInitialized()) {
        cam->Init();
    }

    try{
        //cam->AcquisitionStart.Execute();
        INodeMap & nodeMap = cam->GetNodeMap();
        //set trigger to software trigger
        int result = 0;
        result = ConfigureTrigger(nodeMap);

        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
        {
            std::cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << std::endl << std::endl;
            exit(-1);
        }

        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
        {
            std::cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << std::endl << std::endl;
            exit(-1);
        }

        int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

        std::cout << "Acquisition mode set to continuous..." << std::endl;

        cam->BeginAcquisition();
    }catch (Spinnaker::Exception &ex){
        std::cout << "Error: " << ex.what() << std::endl;
    }
    std::cout << "valeur de capoturing: " << capturing << std::endl;
    while(capturing){
        ImagePtr image = captureImage();
        qInfo() << "stauca - height" << image->GetHeight();
        qInfo() << "stauca - width" << image->GetWidth();
        AbstractCamera::sendFrame(image->GetData(), image->GetBufferSize(), image->GetWidth(), image->GetHeight());

    }
    try{
        //cam->AcquisitionStop.Execute();
        cam->EndAcquisition();
    }catch (Spinnaker::Exception &ex){
        std::cout << "Error: " << ex.what() << std::endl;
    }
    printf("Stopped autoCapture!\n");


}
//Stop the auto captured
//wrote on 11/06/2019 by French students
void SpinCamera::stopAutoCapture(){
    capturing = false;

}

unsigned char* SpinCamera::retrieveImage(unsigned int* bufferSize, unsigned int* imageWidth, unsigned int* imageHeight) {
    if (capturing) return nullptr;
    capturing = true;
    //printf("Images begin to be retrieved\n");

    bool isAlreadyInit = cam->IsInitialized();
    if (!isAlreadyInit) {
        cam->Init();
    }
    Spinnaker::TriggerModeEnums oldTriggerValue =  cam->TriggerMode.GetValue();
    cam->TriggerMode.SetValue(TriggerModeEnums::TriggerMode_Off);

    //printf("Retrieving images...\n");
    //cam->AcquisitionStart.Execute(); <- Old version

    // Armand & Nathan 10/05/2024 : replaced method to start acquisition
    cam->BeginAcquisition();

    //printf("Retrieving 1...\n");
    ImagePtr image = captureImage();

    *bufferSize = image->GetBufferSize();
    *imageWidth = image->GetWidth();
    *imageHeight = image->GetHeight();
    unsigned char* imageBuffer = new unsigned char[image->GetBufferSize()];
    memcpy(imageBuffer, image->GetData(), image->GetBufferSize());


    // Armand & Nathan 10/05/2024 : deleting the image causes Camera Manager to crash
    //delete image;

    //printf("Retrieving 2...\n");

    cam->TriggerMode.SetValue(oldTriggerValue);

    //printf("Retrieving 3...\n");
    //cam->AcquisitionStop.Execute(); <- Old version

    // Armand & Nathan 10/05/2024 : replaced method to stop acquisition
    cam->EndAcquisition();

    if (!isAlreadyInit) {
        cam = nullptr;
    }
    //printf("Images retrieved\n");
    capturing = false;
    return imageBuffer;
}

bool SpinCamera::equalsTo(AbstractCamera *c){
    Spinnaker::GenApi::CStringPtr ptrStringSerial = getCamera()->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
    return ptrStringSerial->GetValue() == c->getSerial();
}

std::string SpinCamera::getString(){
    std::ostringstream ss;
    ss << cam->GetUniqueID();
    return ss.str();
}
