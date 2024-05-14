#ifndef SPINCAMERAMANAGER_H
#define SPINCAMERAMANAGER_H

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

class AbstractCameraManager;

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

/**
 * @brief The SpinCameraManager class deals with all the Spin Capture Cameras
 */

class SpinCameraManager : public AbstractCameraManager {
    Q_OBJECT
public:
    SpinCameraManager();
    ~SpinCameraManager();

    virtual void detectNewCameras();
    virtual void detectNewCameras(std::vector<AbstractCamera*> *newCameras);
    std::string getName() const;

private:
    unsigned int numCameras;
    SystemPtr sytem;
    Spinnaker::Error error;
};

#endif // SPINCAMERAMANAGER_H
