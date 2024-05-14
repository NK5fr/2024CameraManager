#include "systemmanager.h"

SystemManager::SystemManager(){
    system = System::GetInstance();
    list = system->GetCameras();
}

SystemManager::~SystemManager(){
    list.Clear();
    system->ReleaseInstance();
}
