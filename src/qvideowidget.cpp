#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
#include <include/FlyCapture2.h>
#include "qvideowidget.h"
#include "mainwindow.h"

using namespace std;
using namespace FlyCapture2;

QVideoWidget::QVideoWidget(QWidget *parent) : QOpenGLWidget(parent), lastSize(0, 0), active(this->windowState() & Qt::WindowActive), mouseIn(underMouse()) {
    setMouseTracking(true);
    imageDetect = nullptr;
    imgDetThread = nullptr;
    mouseIn = false;
    trackPointProperty = nullptr;
    imgBuffer = nullptr;
    bufferSize = 0;
    imageWidth = 0;
    imageHeight = 0;
    connect(this, SIGNAL(forceUpdate()), this, SLOT(receiveUpdate()));
}

QVideoWidget::~QVideoWidget() {
    if (imageDetect != nullptr) {
        imageDetect->stop();
        imgDetThread->wait();
        delete imageDetect;
    }
}

void QVideoWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    updateView();
}

void QVideoWidget::receiveUpdate(){
    //trick to pass the update to the main thread...
    update();
}

void QVideoWidget::setImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {
    if (imgBuffer == nullptr) return;
    if (this->imageWidth != imageWidth || this->imageHeight != imageHeight) {
        if (this->imgBuffer != nullptr) delete[] this->imgBuffer;
        this->imgBuffer = new unsigned char[bufferSize];
        this->imageWidth = imageWidth;
        this->imageHeight = imageHeight;
    }
    memcpy(this->imgBuffer, imgBuffer, bufferSize);
    this->bufferSize = bufferSize;
    if (imageDetect == nullptr) {
        imageDetect = new ImageDetect(imageWidth, imageHeight);
        imgDetThread = new ImageDetectThread(imageDetect);
        imgDetThread->start();
    }
    if (trackPointProperty != nullptr) {
        if (trackPointProperty->filteredImagePreview || trackPointProperty->trackPointPreview) {
            
            imageDetect->setThreshold(trackPointProperty->thresholdValue);
            imageDetect->setMinPix(trackPointProperty->minPointValue);
            imageDetect->setMaxPix(trackPointProperty->maxPointValue);
            imageDetect->setSubwinSize(trackPointProperty->subwinValue);
            imageDetect->setMinSep(trackPointProperty->minSepValue);
            if (!imageDetect->isBusy()) {
                unsigned char* buffer = new unsigned char[bufferSize];
                memcpy(buffer, imgBuffer, bufferSize);
                imageDetect->setImage(buffer); // ImageDetect gets ownership of the buffer
            }
        }
        imageDetect->setRemoveBackround(trackPointProperty->filteredImagePreview);
    }

    emit forceUpdate();
}

void QVideoWidget::updateView() {
    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, this->width(), this->height());
    glMatrixMode(GL_MODELVIEW);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int width = this->width();
    int height = this->height();
    QSize s = size();
    glOrtho(0, this->width(), this->height(), 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);

    if (((float) width / height) > ((float) imageWidth / imageHeight)) {
        scaled.setHeight(height);
        scaled.setWidth(((float) imageWidth / imageHeight) * height);
        scaled.setTop(0);
        scaled.setLeft((width - (((float) imageWidth / imageHeight) * height)) / 2);
    } else {
        scaled.setHeight(((float) imageHeight / imageWidth) * width);
        scaled.setWidth(width);
        scaled.setTop((height - (((float) imageHeight / imageWidth) * width)) / 2);
        scaled.setLeft(0);
    }
}

void QVideoWidget::paintGL() {
    updateView();
    if (texture.getTextureWidth() != imageWidth || texture.getTextureHeight() != imageHeight) {
        texture.createEmptyTexture(imageWidth, imageHeight);
    }
    if (imageDetect != nullptr && trackPointProperty != nullptr) {
        if (trackPointProperty->filteredImagePreview) {
            texture.updateTexture(imageDetect->getFilteredImage(), bufferSize);
        } else texture.updateTexture(imgBuffer, bufferSize);
    } else {
        texture.updateTexture(imgBuffer, bufferSize);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    texture.bind();
    glTranslated(scaled.left(), scaled.top(), 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2d(0, 0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2d(scaled.width(), 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2d(scaled.width(), scaled.height());
    glTexCoord2f(0.0f, 1.0f);
    glVertex2d(0, scaled.height());
    glEnd();
    texture.unbind();
    glDisable(GL_TEXTURE_2D);


    if (imageDetect != nullptr && trackPointProperty != nullptr) {
        if (trackPointProperty->trackPointPreview) {
            ImPoint* points;
            int numPoints;
            if (trackPointProperty->removeDuplicates) {
                points = imageDetect->getFinalPoints();
                numPoints = imageDetect->getFinalNumPoints();
            } else {
                points = imageDetect->getInitPoints();
                numPoints = imageDetect->getInitNumPoints();
            }
            int crossWingSize = (int) (height() / 75);
            glLineWidth(2);
            for (int i = 0; i < numPoints; i++) {
                int w = scaled.width();
                int h = scaled.height();
                double xPos = ((double) points[i].x * ((double) scaled.width() / imageWidth));
                double yPos = ((double) points[i].y * ((double) scaled.height() / imageHeight));

                glColor3f(1, 0, 0);
                glBegin(GL_LINES);
                glVertex2d(xPos - crossWingSize, yPos);
                glVertex2d(xPos + crossWingSize, yPos);
                glEnd();

                glBegin(GL_LINES);
                glVertex2d(xPos, yPos - crossWingSize);
                glVertex2d(xPos, yPos + crossWingSize);
                glEnd();
                glColor3f(1, 1, 1);

                if (trackPointProperty->showCoordinates) {
                    QPainter painter(this);
                    QPoint pos = scaled.topLeft() + QPoint(xPos, yPos);
                    painter.fillRect(pos.x(), pos.y(), 105, 12, Qt::white);
                    painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(points[i].x, 'f', 2) + " ,Y: " + QString::number(points[i].y, 'f', 2));
                }
            }
        }
    }
}

void QVideoWidget::resizeGL(int width, int height) {
}

void QVideoWidget::resizeEvent(QResizeEvent* event) {
    QOpenGLWidget::resizeEvent(event);
    updateView();
}

void QVideoWidget::showEvent(QShowEvent* event) {
    QOpenGLWidget::showEvent(event);
    updateView();
}

void QVideoWidget::enterEvent(QEvent *) {
    mouseIn = true;
}

void QVideoWidget::leaveEvent(QEvent *) {
    if(!Ui::crosshair) return;
    mouseIn = false;
    update();
}

void QVideoWidget::mouseMoveEvent ( QMouseEvent * event ){
    if(!Ui::crosshair) return;
    mouse = event->pos();
    update();
}

void QVideoWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (isMaximized()) showNormal();
    else showMaximized();
}

void QVideoWidget::changedState(Qt::WindowStates, Qt::WindowStates newState){
    active = newState & Qt::WindowActive;
}

void QVideoWidget::activateCrosshair(bool state){
    //setMouseTracking(state);
}
