#include <iostream>
#include <QDebug>
#include "abstractcamera.h"

AbstractCamera::AbstractCamera() : thread(this) {
    capturing= false;
    container = nullptr;
    coloredContainer = nullptr;
    serial = "unknown";
    model = "unknown";
}

AbstractCamera::~AbstractCamera() {
}

bool AbstractCamera::equalsTo(AbstractCamera* c){
    return this == c;
}

void AbstractCamera::startCapture(VideoOpenGLWidget* videoWidget, VideoOpenGLWidget* coloredVideoWidget){
    if(videoWidget == NULL || coloredVideoWidget == NULL){
        //qDebug() << "[ERROR] startCapture(VideoOpenGLWidget): videoWidget is NULL";
        //cout << "---------------------------------------" << endl << "Erreur" << endl;
        return;

    }
    container = videoWidget;
    coloredContainer = coloredVideoWidget;
    thread.start();
}

void AbstractCamera::sendFrame(void* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight, bool colored) {
    /*
     * 13/05/2024
     * Modified by Nathan & Armand - added a verification to prevent updating a widget that isnt opened (and therefore crashed the application)
    */
    if (!colored && container->isVisible()) {
        container->updateImage((unsigned char*)imgBuffer, bufferSize, imageWidth, imageHeight);
    }else if(colored && coloredContainer->isVisible()){
        coloredContainer->updateImage((unsigned char*)imgBuffer, bufferSize, imageWidth, imageHeight);
    }
}


