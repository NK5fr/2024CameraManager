#include "testcameramanager.h"

TestCameraManager::TestCameraManager() : AbstractCameraManager(), foundCameras() {
    for (int i = 1; i <= 10; i++) {
        foundCameras.push_back(new TestCamera(
            QString("Camera%1").arg(i)
#ifdef _MSC_VER
            .toLocal8Bit().constData()
#else
            .toStdString()
#endif
            ));
    }
        
    std::vector<CameraProperty> props = std::vector<CameraProperty>();
    props.push_back(CameraProperty(CameraManager::BRIGHTNESS, QString("Brightness"), 0, 255, 0, false));
    props.push_back(CameraProperty(CameraManager::HUE, QString("Hue"), 0, 255, 0, false));
    props.push_back(CameraProperty(CameraManager::FRAMERATE, QString("Frame Rate"), 1, 60, 0, false));
    props.push_back(CameraProperty(CameraManager::CROP, QString("Crop"), 1, 800, 0, false));
    for(int i=props.size()-1; i>=0; i--){
        props.at(i).setValue(0.0);
    }
	setProperties(props);
}

void TestCameraManager::detectNewCameras(std::vector<AbstractCamera *> *newCameras){
    *newCameras = foundCameras;
}

std::string TestCameraManager::getName() const{
    return "TestManager";
}


