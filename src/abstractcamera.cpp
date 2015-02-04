#include <iostream>
#include <QDebug>
#include "abstractcamera.h"

using namespace std;

AbstractCamera::AbstractCamera() : thread(this)
{
}

bool AbstractCamera::equalsTo(AbstractCamera* c){
    return this == c;
}
void AbstractCamera::startCapture(QVideoWidget *videoWidget){
    if(videoWidget == NULL){
        qDebug() << "[ERROR] startCapture(QVideoWidget): videoWidget is NULL";
        cout << "---------------------------------------" << endl << "Erreur" << endl;
        return ;
    }
    container = videoWidget;
    thread.start();

}

void AbstractCamera::sendFrame(QImage img){
    container->setImage(img);
}
