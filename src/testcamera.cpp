#include "testcamera.h"
#include <QPainter>
#include <QDebug>
#include <QThread>
#include <stdlib.h>
//#include "sleeper.h"

TestCamera::TestCamera(std::string n)
    : AbstractCamera(), n(n), capturing(false), framerate(5), brightness(128), hue(120), crop(400) {
    srand(42);
    generateBack();
}

void TestCamera::setProperty(CameraProperty* p){
    switch(p->getType()){
    case CameraManager::FRAMERATE: framerate = p->getValue();
        break;
    case CameraManager::BRIGHTNESS: brightness = p->getValue();
        generateBack();
        break;
    case CameraManager::HUE: hue = p->getValue();
        generateBack();
        break;
    case CameraManager::CROP: crop = p->getValue();
        generateBack();
        break;
    default: break;
    }
}

void TestCamera::getProperty(CameraProperty* p){
    p->setAuto(false);
    switch(p->getType()){
    case CameraManager::FRAMERATE: p->setValue(framerate);
        break;
    case CameraManager::BRIGHTNESS: p->setValue(brightness);
        break;
    case CameraManager::HUE: p->setValue(hue);
        break;
    case CameraManager::CROP: p->setValue(crop);
        break;
    default: break;
    }
}

bool TestCamera::retrieveImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {
    return generateImage();
}

void TestCamera::startAutoCapture(){
    capturing = true;
    qDebug() << "Starting autoCapture";
    while(capturing){
        QThread::msleep(1000/framerate);
        sendFrame(nullptr, 0, 0, 0);
    }
    qDebug() << "Stopped autoCapture !";
}

void TestCamera::stopAutoCapture(){
    qDebug() << "Stopping autoCapture";
    capturing = false;
}

void TestCamera::generateBack(){
    back = QImage(crop, crop, QImage::Format_RGB32);
    QColor b;
    b.setHsl(hue,255, brightness);
    back.fill(b);
    QPainter p(&back);
    p.setPen(Qt::gray);
    for(int i=0; i<crop; i+=10){
        p.drawLine(0, i, crop, i);
        p.drawLine(i, 0, i, crop);
    }
    p.end();
}

QImage* TestCamera::generateImage(){
    QImage* img = new QImage(back);
    QPainter p(img);
    p.setPen(Qt::red);
    p.drawRect(QRect( QPoint(rand()%100, rand()%100), QPoint(crop-rand()%100, crop-rand()%100) ));
    p.setPen(Qt::green);
    p.drawRect(QRect( QPoint(rand()%100, rand()%100), QPoint(crop-rand()%100, crop-rand()%100) ));
    p.setPen(Qt::blue);
    p.drawRect(QRect( QPoint(rand()%100, rand()%100), QPoint(crop-rand()%100, crop-rand()%100) ));
    p.end();
    return img;
}
