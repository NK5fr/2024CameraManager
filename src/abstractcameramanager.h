/**
 * \file abstractcameramanager.h
 * \author Virgile Wozny
 */
#ifndef ABSTRACTCAMERAMANAGER_H
#define ABSTRACTCAMERAMANAGER_H

#include <string>
#include <vector>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTreeWidget>
#include <QThread>
#include <QDebug>
#include "mainwindow.h"
#include "qvideowidget.h"
#include "abstractcamera.h"
#include "cameraproperty.h"

using namespace CameraManager;

class MainWindow;

enum MyRoles {
    CameraGroupRole = Qt::UserRole + 1,
    CameraRole = Qt::UserRole + 1
};

/**
 * AbstractCameraManager
 * \brief Class that need to be subclassed for each camera API. It is used to list and display Cameras, properties and liveview.
 */
class AbstractCameraManager : public QObject
{
    Q_OBJECT
    public:
        // Pure virtual -> to implement
        /**
         * @brief (Pure virtual) detect new cameras
         * @param newCameras (modified by the function) will be filled with all the camera connected to the computer, even ones already connected
         */
        virtual void detectNewCameras(std::vector<AbstractCamera*> *newCameras) = 0;

        /**
         * @brief (Pure virtual) get the name of the Manager
         * @return String containing the name manager
         */
        virtual std::string getName() const = 0;

        //do not reimplement
        /**
         * @brief detect new cameras
         * @return the index in the model corresponding to the "Detected Cameras" group, in order to expand it in the view
         */
        QModelIndex detectNewCamerasAndExpand();

        //may be changed
        void updateImages();
        void updateProperties();

        // Lars Aksel - 27.01.2015
        void loadPropertiesDefaults();
        void loadPropertiesDefaultsInit();
        bool loadPropertiesFromFile(QString& filepath, std::vector<CameraProperty>& prop);
        bool savePropertiesToFile(QString& filepath);
        void setProperties(std::vector<CameraManager::CameraProperty> &properties);

        // Lars Aksel - 05.02.2015
        void setTrackPointProperty(TrackPointProperty* prop) { 
            for (int i = activeCameras.size() - 1; i >= 0; i--) {
                if (activeCameras.at(i).camera->getVideoContainer() == nullptr) continue;
                activeCameras.at(i).camera->getVideoContainer()->setTrackPointProperty(prop);
            }
        }
        void updateContainer() {
            for (int i = activeCameras.size() - 1; i >= 0; i--) {
                if (activeCameras.at(i).camera->getVideoContainer() == nullptr) continue;
                activeCameras.at(i).camera->getVideoContainer()->update(); 
            }
        }
        void stopCapturing() { for (int i = activeCameras.size() - 1; i >= 0; i--) activeCameras.at(i).camera->stopAutoCapture(); }
        void setUpdateProperties(bool onOff) { this->updateProps = onOff; }
        bool isCapturing() { return this->liveView; }
        bool isUpdateProperties() { return this->updateProps; }

        /**
         * @brief add a empty group of cameras in the model
         * @return the index in the model corresponding to this group, in order to select it
         */
        QModelIndex addGroup();

        /**
         * @brief remove a group from the model
         * @param index of the item to remove
         */
        void removeGroup(QModelIndex index);

        /**
         * @brief reset the name of a camera
         * @param index of the camera
         */
        void resetItem(QModelIndex index);

        /**
         * @brief check the camera in the model, add it in the activeCameras vector and open a subwindow for it
         * @param camera pointer to the camera to add
         * @param item position of the camera in the model
         * @param active true to activate, false to desactivate
         */
        void activateCamera(AbstractCamera* camera, QStandardItem* item, bool active);

        /**
         * @brief used to close all liveviews from this manager
         */
        void desactiveAllCameras();

        /**
         * @brief select a camera or a group to edit its properties
         * @param index index of the item in the model
         * @param string (modified by the function) desccritive of the item
         * @param icon (modified by the function) corresponding to the type of element selected
         * @param editable (modified by the function) true if the item is editable
         * @param deletable (modified by the function) true if the item is deletable
         */
        void cameraTree_itemClicked(const QModelIndex & index, QString &string, int &icon, bool &editable, bool &deleteable);

        void activateLiveView(bool active);

        /**
         * @brief get the model ( camera list )
         * @return pointer to the model
         */
        QStandardItemModel* getModel();

        /**
         * @brief get the model of the properties
         * @return pointer to the widget
         */
        QTreeWidget* getPropertiesWidget();

        /**
         * @brief to set the needed cross reference, called once after construction
         * @param window pointer to the main window
         */
        void setMainWindow(MainWindow* window);        

        struct activeCameraEntry {
            activeCameraEntry(AbstractCamera *c, QStandardItem* i)
                : camera(c), treeItem(i), window(new QMdiSubWindow()) {
                window->setAttribute(Qt::WA_DeleteOnClose);
                window->setWindowFlags(Qt::Tool);
                QVideoWidget* videoWidget = new QVideoWidget();
                videoWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                window->setWidget(videoWidget);
                window->resize(400, 300);
                QObject::connect(window, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)),
                                 videoWidget, SLOT(changedState(Qt::WindowStates, Qt::WindowStates)));
            }
            //~activeCameraEntry(){ delete window; }
            AbstractCamera* camera;
            QStandardItem* treeItem;
            QMdiSubWindow* window;
        };

        const std::vector<activeCameraEntry>& getActiveCameraEntries() { return this->activeCameras; }

    protected:
        /**
         * @brief constructor
         * @param empty true if the manager is an emty manager
         */
        AbstractCameraManager(bool empty=false);
        /**
         * @brief add properties that can be set and retrieved to/from  cameras for this API
         * @param properties vector of CameraProperty
         */
        void updateProperties(std::vector<CameraManager::CameraProperty> &properties);
    private slots:
        void on_CameraTree_itemChanged(QStandardItem* item);
        void on_subwindow_closing(QObject* window);
        void on_propertyCheckbox_changed(int state);
        void on_propertySlider_changed(int val);
        void on_propertyValue_changed();
    protected:

        MainWindow* mainWindow;
        QStandardItemModel cameraTree;
        QStandardItem newCameraList;
        QTreeWidget propertiesList;
        QStandardItem* selectedItem;
        AbstractCamera* selectedCamera;
        QIcon folderIcon;
        bool liveView;
        bool updateProps;
        
        std::vector<activeCameraEntry> activeCameras;
        std::vector<CameraManager::CameraProperty> cameraProperties;
        void cameraTree_recursiveCheck(QStandardItem* parent, Qt::CheckState checked);
        QStandardItem* cameraTree_recursiveFirstCamera(QStandardItem* parent);
        void cameraTree_recursiveSetProperty(QStandardItem* parent, CameraManager::CameraProperty* prop);
        void cameraTree_getCameraList(QStandardItem* parent, std::vector<QStandardItem*> *list);
        AbstractCamera* getSelectedCamera();
};

#endif // ABSTRACTCAMERAMANAGER_H
