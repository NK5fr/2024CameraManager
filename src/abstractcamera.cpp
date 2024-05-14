#include <iostream>
#include <QDebug>
#include "abstractcamera.h"

using namespace std;

AbstractCamera::AbstractCamera() : thread(this) {
    capturing= false;
    container = nullptr;
    serial = "unknown";
    model = "unknown";
}

AbstractCamera::~AbstractCamera() {
}

bool AbstractCamera::equalsTo(AbstractCamera* c){
    return this == c;
}

void AbstractCamera::startCapture(VideoOpenGLWidget* videoWidget){
    if(videoWidget == NULL){
        //qDebug() << "[ERROR] startCapture(VideoOpenGLWidget): videoWidget is NULL";
        //cout << "---------------------------------------" << endl << "Erreur" << endl;
        return;

    }
    container = videoWidget;
    thread.start();
}

void AbstractCamera::sendFrame(void* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {
    /*
     * 13/05/2024
     * Modified by Nathan & Armand - added a verification to prevent updating a widget that isnt opened (and therefore crashed the application)
    */
    if (container->isVisible()) {
        container->updateImage((unsigned char*)imgBuffer, bufferSize, imageWidth, imageHeight);
    }
}


