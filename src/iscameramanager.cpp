#include "iscameramanager.h"
#include "iscamera.h"
#include <sstream>
#include <QImage>
#include <QProcess>
using namespace std;

IsCameraManager::IsCameraManager()
    : AbstractCameraManager()
{
#if defined(WIN32) || defined(WIN64)
    QProcess *myProcess = new QProcess(NULL);
    myProcess->start("PGRIdleStateFix.exe", QStringList("enable") );
#endif

    std::vector<CameraProperty> prop = std::vector<CameraProperty>();
    /*prop.push_back(CameraProperty(CameraManager::BRIGHTNESS, 0, 255, 0, true));
    prop.push_back(CameraProperty(CameraManager::GAIN, 0, (float) 12.041, 3, true));
    prop.push_back(CameraProperty(CameraManager::EXPOSURE, 0, 62, 0, true));
    prop.push_back(CameraProperty(CameraManager::GAMMA, 0, 1, 0, false));
    prop.push_back(CameraProperty(CameraManager::SHUTTER, 0, (float) 33.174, 3, true));*/

	// gs 2014-12-05, slår av auto og setter antall desimaler = 3, 
	prop.push_back(CameraProperty(CameraManager::BRIGHTNESS, QString("Brightness"), 0, 255.000, 3, false));   
    prop.push_back(CameraProperty(CameraManager::GAIN, QString("Gain"), 0, (float) 12.041, 3, false));
    prop.push_back(CameraProperty(CameraManager::EXPOSURE, QString("Exposure"), 0, 62.000, 3, false));
    prop.push_back(CameraProperty(CameraManager::GAMMA, QString("Gamma"), 0, 1, 3, false));
    prop.push_back(CameraProperty(CameraManager::SHUTTER, QString("Shutter"), 0, (float) 512.000, 3, false));

    for(int i=prop.size()-1; i>=0; i--){
        prop.at(i).setValue(0.0);
    }
    setProperties(prop);
}

IsCameraManager::~IsCameraManager()
{
    //dtor
}


void IsCameraManager::detectNewCameras(std::vector<AbstractCamera*> *newCameras)
{
    busMgr.GetNumOfCameras(&numCameras);
    for(unsigned int i = 0; i<numCameras; i++){
        FlyCamera* flyCam = new FlyCamera();
        busMgr.GetCameraFromIndex(i, flyCam->getGuid());
        flyCam->getCamera()->Connect(flyCam->getGuid());
        flyCam->getCamera()->GetCameraInfo(flyCam->getCameraInfo());

        newCameras->push_back(flyCam);
    }
}

void IsCameraManager::getCamerasPropertiesList() const
{
    //Nothing here
}

string IsCameraManager::getName() const
{
    return "FlyCapture Camera Manager";
}
