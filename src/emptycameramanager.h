/**
 * \file emptycameramanager.h
 * \author Virgile Wozny
 */


#ifndef EMPTYCAMERAMANAGER_H
#define EMPTYCAMERAMANAGER_H

#include "abstractcamera.h"
#include "abstractcameramanager.h"

/**
 * EmptyCameraManager
 * \brief empty camera manager named "Select".
 */
class EmptyCameraManager : public AbstractCameraManager {
    public:
        EmptyCameraManager();
        virtual void detectNewCameras(std::vector<AbstractCamera*> *);
        virtual void getCamerasPropertiesList() const;
        virtual std::string getName() const;
};

#endif // EMPTYCAMERAMANAGER_H
