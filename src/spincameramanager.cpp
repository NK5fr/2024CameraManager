#include "spincameramanager.h"
#include <iostream>

SpinCameraManager::SpinCameraManager() : AbstractCameraManager() {
#if defined(WIN32) || defined(WIN64)
    QProcess *myProcess = new QProcess(NULL);
    myProcess->start("PGRIdleStateFix.exe", QStringList("enable") );
#endif
    /*  prop.push_back(CameraProperty(CameraManager::BRIGHTNESS,  0, 255,             0, true));
    prop.push_back(CameraProperty(CameraManager::GAIN,          0, (float) 12.041,  3, true));
    prop.push_back(CameraProperty(CameraManager::EXPOSURE,      0, 62,              0, true));
    prop.push_back(CameraProperty(CameraManager::GAMMA,         0, 1,               0, false));
    prop.push_back(CameraProperty(CameraManager::SHUTTER,       0, (float)33.174,   3, true));
    prop.push_back(CameraProperty(CameraManager::PAN,           0, 112,             0, true));
    prop.push_back(CameraProperty(CameraManager::FRAMERATE,     4, 60,              1, true));
    prop.push_back(CameraProperty(CameraManager::AUTOTRIGGER,   0, 0,               0, true));*/


    // gs 2014-12-05, sl?r av auto og setter antall desimaler = 3, min og maksverdier er hardkodet- kan de leses fra kameraene
    // gs 2015-01-20, legger inn hardkodet properties value som default
    // gs 2015-01-20 noe galt -> kameraene ramler ut
    /*
    prop.push_back(CameraProperty(CameraManager::BRIGHTNESS,    0,          255,        3, true, false, 0));
    prop.push_back(CameraProperty(CameraManager::GAIN,          0,          12.041f,    3, true, false, 0));
    prop.push_back(CameraProperty(CameraManager::EXPOSURE,      -7.6f,      2.5f,       3, true, false, 1.415));
    prop.push_back(CameraProperty(CameraManager::GAMMA,         0,          1,          0, true, false, 1024));
    prop.push_back(CameraProperty(CameraManager::SHUTTER,       0,          6.6f,       3, true, false, 1.003));
    prop.push_back(CameraProperty(CameraManager::PAN,           0,          112,        0, true, false, 0));
    prop.push_back(CameraProperty(CameraManager::FRAMERATE,     4,          150,        1, true, false, 150));
    prop.push_back(CameraProperty(CameraManager::AUTOTRIGGER,   0,          0,          0, true, true, 0));
    */

    // Lars Aksel - 27.01.2015 - Loading camera properties from file
    loadPropertiesDefaultsInit();

    //gs 2015-01-20, fjerner nullstilling av value - setter den i koden over istedet
    //for(int i=prop.size()-1; i>=0; i--){
    //   prop.at(i).setValue(0.0);
    //}


    //setProperties(prop);
}

SpinCameraManager::~ SpinCameraManager() {
    //dtor
}

void  SpinCameraManager::detectNewCameras(std::vector<AbstractCamera *> *newCameras) {

}


void  SpinCameraManager::detectNewCameras() {

}

std::string SpinCameraManager::getName() const {
    return "Spinnaker Camera Manager";
}
