#include "emptycameramanager.h"

EmptyCameraManager::EmptyCameraManager()
    : AbstractCameraManager(true) {
}
void EmptyCameraManager::detectNewCameras(std::vector<AbstractCamera *> *){
    //Nothing here
}
void EmptyCameraManager::getCamerasPropertiesList() const{
    //Nothing
}

std::string EmptyCameraManager::getName() const{
    return "Select";
}
