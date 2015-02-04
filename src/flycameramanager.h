#ifndef FLYCAMERAMANAGER_H
#define FLYCAMERAMANAGER_H

#include "flycamera.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "abstractcamera.h"
#include "abstractcameramanager.h"

using namespace FlyCapture2;

/**
 * @brief The FlyCameraManager class deals with all the Fly Capture Cameras
 */

class FlyCameraManager : public AbstractCameraManager {
public:
    FlyCameraManager();
    ~FlyCameraManager();

    virtual void detectNewCameras(std::vector<AbstractCamera*> *newCameras);

    std::string getName() const;
private:
	unsigned int numCameras;
	BusManager busMgr;
	Error error;
};

#endif // FLYCAMERAMANAGER_H
