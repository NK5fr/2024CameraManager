#ifndef ISCAMERAMANAGER_H
#define ISAMERAMANAGER_H

#include "flycamera.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "abstractcamera.h"
#include "abstractcameramanager.h"

using namespace FlyCapture2;

class IsCameraManager
        : public AbstractCameraManager
{
public:
    IsCameraManager();
    virtual ~IsCameraManager();
    virtual void detectNewCameras(std::vector<AbstractCamera*> *newCameras);
    std::string getName() const;
    void getCamerasPropertiesList() const;

    unsigned int numCameras;

private:
    BusManager busMgr;
    Error error;
};

#endif // ISCAMERAMANAGER_H
