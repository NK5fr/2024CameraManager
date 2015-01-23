/**
 * \file testcameramanager.h
 * \author Virgile Wozny
 */

#ifndef TESTCAMERAMANAGER_H
#define TESTCAMERAMANAGER_H

#include <vector>
#include <string>
#include "abstractcamera.h"
#include "abstractcameramanager.h"

/**
 * TestCameraManager
 * \brief to test ui without real cameras.
 */
class TestCameraManager : public AbstractCameraManager {
    public:
        TestCameraManager();
        virtual void detectNewCameras(std::vector<AbstractCamera*> *newCameras);
        virtual std::string getName() const;
    private:
        std::vector<AbstractCamera*> foundCameras;
};

#endif // TESTCAMERAMANAGER_H
