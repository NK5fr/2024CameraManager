#include <iostream>
#include <QDebug>
#include "abstractcamera.h"

using namespace std;

AbstractCamera::AbstractCamera() : thread(this) {
    capturing = false;
    container = nullptr;
    serial = "UNKNOWN SERIAL";
    model = "UNKNOWN MODEL";
}

AbstractCamera::~AbstractCamera() {
}

bool AbstractCamera::equalsTo(AbstractCamera* c){
    return this == c;
}

void AbstractCamera::startCapture(QVideoWidget* videoWidget){
    if(videoWidget == NULL){
        //qDebug() << "[ERROR] startCapture(QVideoWidget): videoWidget is NULL";
        //cout << "---------------------------------------" << endl << "Erreur" << endl;
        return;
    }
    container = videoWidget;
    thread.start();
}

void AbstractCamera::sendFrame(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {
    container->setImage(imgBuffer, bufferSize, imageWidth, imageHeight);
}
