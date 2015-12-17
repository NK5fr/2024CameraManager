#ifndef FLYCAMERAMANAGER_H
#define FLYCAMERAMANAGER_H

#include <sstream>
#include <QtCore>
#include <QImage>
#include <QProcess>
#include <qsettings.h>
#include <qdir.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "abstractcameramanager.h"
//#include "abstractcamera.h"
#include "flycamera.h"

class AbstractCameraManager;

using namespace FlyCapture2;

/**
 * @brief The FlyCameraManager class deals with all the Fly Capture Cameras
 */

class FlyCameraManager : public AbstractCameraManager {
   Q_OBJECT
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
