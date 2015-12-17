/**
 * \file testcameramanager.h
 * \author Virgile Wozny
 */

#ifndef TESTCAMERAMANAGER_H
#define TESTCAMERAMANAGER_H

#include <vector>
#include <string>
#include <QtCore>
#include "abstractcameramanager.h"
//#include "abstractcamera.h"
#include "testcamera.h"

/**
 * TestCameraManager
 * \brief to test ui without real cameras.
 */

class AbstractCameraManager;

class TestCameraManager : public AbstractCameraManager {
    //Q_OBJECT
    public:
        TestCameraManager();
        virtual void detectNewCameras(std::vector<AbstractCamera*> *newCameras);
        virtual std::string getName() const;
    private:
        std::vector<AbstractCamera*> foundCameras;
};

#endif // TESTCAMERAMANAGER_H
