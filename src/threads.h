
#ifndef THREAD_DETECT_CAMERA_H
#define THREAD_DETECT_CAMERA_H

#include <vector>
#include <qthread.h>
#include <qtreeview.h>
#include "abstractcameramanager.h"

class ThreadDetectCamera : public QThread {
    Q_OBJECT
public:
    ThreadDetectCamera(QTreeView* cameraTree, std::vector<AbstractCameraManager*>* cameraManagers) : QThread() {
        this->cameraTree = cameraTree;
        this->cameraManagers = cameraManagers;
        selectedCameraManager = 0;
        running = false;
    }

    inline void stopCameraDetection() { this->running = false; }
    inline void setSelectedCameraManager(int selectedCameraManager) { this->selectedCameraManager = selectedCameraManager; }

protected:
    void run() {
        running = true;
        while (running){
            cameraTree->setExpanded(cameraManagers->at(selectedCameraManager)->detectNewCamerasAndExpand(), true);
            //emit w->resizeColumnsCameraTree();
            QThread::msleep(200);
        }
    }
signals:
    void resizeCameraTree();

private:
    QTreeView* cameraTree;
    std::vector<AbstractCameraManager*>* cameraManagers;
    int selectedCameraManager;
    bool running;
};

#endif
