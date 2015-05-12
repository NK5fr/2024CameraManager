#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
#include <FlyCapture2.h>
#include "qvideowidget.h"
#include "mainwindow.h"

using namespace std;
using namespace FlyCapture2;

QVideoWidget::QVideoWidget(QWidget *parent) : QWidget(parent), lastSize(0, 0), active(this->windowState() & Qt::WindowActive), mouseIn(underMouse()) {
    setMouseTracking(true);
    imageDetect = nullptr;
    mouseIn = false;
    trackPointProperty = nullptr;
    connect(this, SIGNAL(forceUpdate()), this, SLOT(receiveUpdate()));
}

QVideoWidget::~QVideoWidget() {
    imageDetect->stop();
    imgDetThread->wait();
    delete imageDetect;
}

void QVideoWidget::receiveUpdate(){
    //trick to pass the update to the main thread...
    update();
}
void QVideoWidget::setImage(QImage* image){
    if (image == nullptr) return;
    if(image->isNull()){
		cout << "image er null" << endl ;  // gs lagt inn testutskrift
		return;
	}
	//cout << "image er IKKE null" << endl ;  // gs lagt inn testutskrift
    mutex.lock();
    img = image->copy();
    //image = QImage();
    mutex.unlock();
    if(lastSize != img.size()) resizeEvent();
    else scaledImage = img.scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation);
    emit forceUpdate();
}

void QVideoWidget::paintEvent(QPaintEvent *) {
    if(img.isNull()) return;
    QImage scaledImg;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    unsigned int width = img.width();
    unsigned int height = img.height();
    unsigned char imgVal = 0;

    QPen redPen(QColor(255, 0, 0), 2);
    QPen blackPen(QColor(0, 0, 0), 2);
    QPen whitePen(QColor(255, 255, 255), 2);
    painter.setPen(redPen);

    if (trackPointProperty->filteredImagePreview || trackPointProperty->trackPointPreview) {
        if (imageDetect == nullptr) {
            imageDetect = new ImageDetect(width, height, trackPointProperty->thresholdValue, trackPointProperty->subwinValue);
            imgDetThread = new ImageDetectThread(imageDetect);
            imgDetThread->start();
        }

        // If the dimensions of the image is changed during capture 
        if (imageDetect->getImageWidth() != width || imageDetect->getImageHeight() != height) {
            printf("Warning: Image-size changed during capture!\n");
            return;
            //delete imageDetect;
            //imageDetect = new ImageDetect(width, height, trackPointProperty->thresholdValue, trackPointProperty->subwinValue);
        }
        imageDetect->setThreshold(trackPointProperty->thresholdValue);
        imageDetect->setMinPix(trackPointProperty->minPointValue);
        imageDetect->setMaxPix(trackPointProperty->maxPointValue);
        imageDetect->setSubwinSize(trackPointProperty->subwinValue);
        imageDetect->setMinSep(trackPointProperty->minSepValue);
        if (!imageDetect->isBusy()) {
            unsigned char* imageBuffer = new unsigned char[width * height];
            mutex.lock();
            for (int i = 0; i < width * height; ++i) {
                imageBuffer[i] = img.bits()[i * 4];
            }
            mutex.unlock();
            imageDetect->setImage(imageBuffer); // imageDetect has ownership of imageBuffer, so it has the responsibility of deleting it.
        }
    }

    if (trackPointProperty->filteredImagePreview) {
        //imageDetect->imageRemoveBackground();
        QImage imgCopy = img.copy();
        unsigned char* newImageBuffer = imageDetect->getFilteredImage();
        mutex.lock();
        for (int i = 0; i < width * height; ++i) {
            imgVal = newImageBuffer[i];
            imgCopy.bits()[(i * 4) + 0] = imgVal;  // Red
            imgCopy.bits()[(i * 4) + 1] = imgVal;  // Green
            imgCopy.bits()[(i * 4) + 2] = imgVal;  // Blue
            imgCopy.bits()[(i * 4) + 3] = 255;     // Alpha
        }
        mutex.unlock();
        //scaledImg = imgCopy.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        //painter.drawImage(scaled.topLeft(), scaledImg);
        painter.drawImage(scaled.topLeft(), scaledImage);
    } else {
        //scaledImg = img.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        //painter.drawImage(scaled.topLeft(), scaledImg);
        painter.drawImage(scaled.topLeft(), scaledImage);
    }
    if (trackPointProperty->trackPointPreview) {
        //imageDetect->imageDetectPoints();
        ImPoint* points;
        int numPoints;
        while (imageDetect->isWritingToPoints());
        if (trackPointProperty->removeDuplicates) {
            //imageDetect->removeDuplicatePoints();
            points = imageDetect->getFinalPoints();
            numPoints = imageDetect->getFinalNumPoints();
        } else {
            points = imageDetect->getInitPoints();
            numPoints = imageDetect->getInitNumPoints();
        }
        // Drawing the points on the image...
        int crossWingSize = (int) (height / 75);
        int crossWidthSize = (int) (width / 300);
        for (int i = 0; i < numPoints; i++) {
            int xPos = ((double) points[i].x * ((double) scaled.size().width() / img.width())) + scaled.topLeft().x();
            int yPos = ((double) points[i].y * ((double) scaled.size().height() / img.height())) + scaled.topLeft().y();

            if (trackPointProperty->showCoordinates) {
                painter.fillRect(xPos, yPos, 105, 12, Qt::white);
                painter.setPen(blackPen);
                painter.drawText(xPos + 2, yPos + 10, "X: " + QString::number(points[i].x, 'f', 2) + " ,Y: " + QString::number(points[i].y, 'f', 2));
                painter.setPen(redPen);
            }
            painter.drawLine(xPos, yPos - crossWingSize, xPos, yPos + crossWingSize);
            painter.drawLine(xPos - crossWingSize, yPos, xPos + crossWingSize, yPos);
        }
        //imageDetect->setImage(nullptr);
    }

    if (Ui::crosshair && mouseIn) {
        const int magnifiedAreaSize = 25;
        const int magnifierSize = 200;

        QPoint pos = mouse - scaled.topLeft();
        int xPos = mouse.x();
        int yPos = mouse.y();
        int imageCoordX = ((float) pos.x() * ((float) img.width() / scaled.width()));
        int imageCoordY = ((float) pos.y() * ((float) img.height() / scaled.height()));

        painter.setPen(blackPen);
        painter.drawText(2, 15, "X: " + QString::number(imageCoordX, 'f', 2) + " ,Y: " + QString::number(imageCoordY, 'f', 2));
        painter.setPen(whitePen);

        if (xPos + magnifierSize > size().width()) xPos -= magnifierSize;
        if (yPos + magnifierSize > size().height()) yPos -= magnifierSize;

        QImage magnifiedImage = img.copy(imageCoordX - (magnifiedAreaSize / 2), imageCoordY - (magnifiedAreaSize / 2), magnifiedAreaSize, magnifiedAreaSize);
        magnifiedImage = magnifiedImage.scaled(QSize(magnifierSize, magnifierSize), Qt::KeepAspectRatio);
        painter.drawImage(xPos, yPos, magnifiedImage);
        painter.drawLine(QLine(xPos + (magnifierSize / 2), yPos, xPos + (magnifierSize / 2), yPos + magnifierSize));
        painter.drawLine(QLine(xPos, yPos + (magnifierSize / 2), xPos + magnifierSize, yPos + (magnifierSize / 2)));
        painter.drawRect(xPos, yPos, magnifierSize, magnifierSize);
    }

    /*
    if (mouseIn) {
        QString sx("x:%1");
        QString sy("y:%1");
        if (Ui::crosshairReal) {
            QPoint pos = (mouse-scaled.topLeft()) * ratio;
            sx = sx.arg(pos.x());
            sy = sy.arg(pos.y());

            mutex.lock();
            QImage imgCopy = img.copy();
            mutex.unlock();
            QPainter painterImg(&imgCopy);
            painterImg.drawPixmap(pos-QPoint(15, 15), QPixmap(":/icons/crosshair"));
            painterImg.drawLine(0, pos.y(), pos.x()-15, pos.y());
            painterImg.drawLine(pos.x()+16, pos.y(), imgCopy.width(), pos.y());

            painterImg.drawLine(pos.x(), 0, pos.x(), pos.y()-15);
            painterImg.drawLine(pos.x(), pos.y()+16, pos.x(), imgCopy.height());
            painterImg.end();

            scaledImg = imgCopy.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawImage(scaled.topLeft(), scaledImg);
        } else { // !Ui::crosshairReal
            QPointF pos = QPointF(mouse-scaled.topLeft()) * ratio;
            sx = sx.arg(pos.x());
            sy = sy.arg(pos.y());

            mutex.lock();
            scaledImg = img.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            mutex.unlock();
            painter.drawImage(scaled.topLeft(), scaledImg);

            painter.drawPixmap(mouse-QPoint(15, 15), QPixmap(":/icons/crosshair"));
            painter.drawLine(scaled.left(), mouse.y(), mouse.x()-15, mouse.y());
            painter.drawLine(mouse.x()+16, mouse.y(), scaled.right(), mouse.y());

            painter.drawLine(mouse.x(), scaled.top(), mouse.x(), mouse.y()-15);
            painter.drawLine(mouse.x(), mouse.y()+16, mouse.x(), scaled.bottom());
        }

        QFont font(painter.font());
        font.setPixelSize(15);
        painter.setFont(font);
        painter.fillRect( 0, 0, 70, 35, Qt::white );
        painter.drawText( 1, 16, sx );
        painter.drawText( 1, 32, sy );
    } else { // !mouseIn
        mutex.lock();
        scaledImg = img.scaled(this->size(), Qt::KeepAspectRatio, Ui::forceHighQuality ? Qt::SmoothTransformation : Qt::FastTransformation);
        mutex.unlock();
        painter.drawImage(scaled.topLeft(), scaledImg);
    }
    painter.end();
    */
}

void QVideoWidget::resizeEvent(QResizeEvent *){
    if (img.isNull()) return;
    mutex.lock();
    scaledImage = img.scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation);
    mutex.unlock();
    QPoint pos;
    if (scaledImage.height() == this->height()){
        pos = QPoint((this->width() - scaledImage.width()) / 2, 0);
        ratio = (float) img.width() / scaledImage.width();
    }else{
        pos = QPoint(0, (this->height() - scaledImage.height()) / 2);
        ratio = (float) img.height() / scaledImage.height();
    }
    //qDebug() << "ratio" << ratio;
    scaled = QRect(pos, scaledImage.size());
    lastSize = img.size();
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
    setMouseTracking(state);
}
