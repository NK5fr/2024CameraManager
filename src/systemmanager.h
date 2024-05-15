#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <QObject>
#include <include/Spinnaker/Spinnaker.h>
#include <include/Spinnaker/SpinGenApi/SpinnakerGenApi.h>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

class SystemManager
{
public:
    SystemManager();
    ~SystemManager();

    SystemPtr getSystem(){return system;}
    CameraList getCamList(){return system->GetCameras();}

private:
    SystemPtr system;
    CameraList list;
};

#endif // SYSTEMMANAGER_H
