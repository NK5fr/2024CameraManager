#include "imageviewerwidget.h"

#include <QImage>
#include <QLabel>
#include <QToolTip>
#include <QFile>
#include <QStringList>
#include <QWheelEvent>
#include <qdebug.h>

#include <QtCore/qmath.h>

#include <iostream>

using namespace std;

ImageViewerWidget::ImageViewerWidget(QString path, QString filename) :
    path(path), filename(filename) {
    fullPath = path + "/" + filename;
    setImageFromFile(fullPath);
    labelImage.setAlignment(Qt::AlignCenter);
    setWidget(&labelImage);
    setWindowFlags(Qt::SubWindow);
    setWindowTitle(filename);
    //initializingPoints();
}

void ImageViewerWidget::setImageFromFile(const QString& filepath) {
    QFile file(filepath);
    if (!file.exists()) {
        qDebug("File does not exist: %s\n", filepath);
        return;
    }
    /*
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug("Failed to open the file : %s\n", filepath);
        return;
    } */   
    bool loaded = image.load(filepath);
    if (loaded) {
        labelImage.setPixmap(image);
        imageSize = image.size();
    } else {
        qDebug("Image could not be loaded...\n");
    }
}

void ImageViewerWidget::initializingPoints(){
    /*
    // Initializing with wrong size 
    points = vector<vector<QPoint>>(50, vector<QPoint>(0));

    // Having the number of the image (to load the right data file number) 
    QString underscoreString = name.split("_").at(1);
    QString number = underscoreString.split(".").at(0);

    // Removing the first 0 because the data file has only 4 number and not 5 as the image file 
    number=number.mid(1, number.size());
    QString relativeImagePath = relativePathToDatas + "/" + number + ".dat";
    QFile myFile(projectPath + "/" + relativeImagePath);

    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        cout << "Fail to open the file : "<< QString(projectPath + "/" + relativeImagePath).toUtf8().constData() << endl;
        correspondingData=false;
        return;
    }
    correspondingData=true;
    QString line= "";
    camerasNb=0;
    while(!myFile.atEnd()){
        while(!myFile.atEnd() && !line.contains("Camera no"))
            line=myFile.readLine();

        // Getting the line with points number 
        line=myFile.readLine();
        QStringList list = line.split(" ");
        // Removing the last char because it is the \n 
        QString tmp = list.at(list.size()-1);
        pointsNb=tmp.mid(0, tmp.size()-1).toInt();
        points[camerasNb].resize(pointsNb);
        for(int i=0;i<pointsNb;i++){
            // Spliting line with ' ' 
            line=myFile.readLine();
            list=line.split(" ");
            int j=0;
            // Removing useless cells 
            while(j<list.size()){
                QString tmp = list.at(j);
                // Removing useless spaces from cells 
                tmp.replace(QString(" "), QString(""));
                if(!tmp.toDouble() && tmp!=QString("0"))
                    list.removeAt(j);
                else
                    j++;
            }
            points[camerasNb][i]=QPoint(list.at(1).toDouble(), list.at(2).toDouble());
        }
        camerasNb++;
    }
    points.resize(camerasNb);
    */
}

void ImageViewerWidget::mousePressEvent(QMouseEvent *mouseEvent){
    QPoint relativeMousePos = mouseEvent->pos();
    QSize widgetSize = size();
    double x = 0;
    double y = 0;
    int imX = 1280;
    int imY = 1024;
    bool isMultipleImages = true;
    int scaledImageWidth = 0;
    int scaledImageHeight = 0;

    if ((widgetSize.width() / widgetSize.height()) > (imageSize.width() / imageSize.height())) { // If background is wider than image (Sides are blank)
        scaledImageWidth = (imageSize.width() * widgetSize.height()) / imageSize.height();
        scaledImageHeight = widgetSize.height();
        x = ((widgetSize.width() - scaledImageWidth) / 2);
    } else { // If background is smaller (in width) than image (top and bottom is blank)
        scaledImageWidth = widgetSize.width();
        scaledImageHeight = (widgetSize.width() * imageSize.height()) / imageSize.width();
        y = ((widgetSize.height() - scaledImageHeight) / 2);
    }

    double imageX = ((double) relativeMousePos.x() - x);
    double imageY = ((double) relativeMousePos.y() - y);

    int imageCoordX = (int) ((imageX / (double) scaledImageWidth) * imageSize.width());
    int imageCoordY = (int) ((imageY / (double) scaledImageHeight) * imageSize.height());


    int imageNr = 0;
    if (isMultipleImages) {
        imageNr = ((imageSize.width() / imX) * (imageCoordY / imY)) + (imageCoordX / imX);
        imageCoordX %= imX;
        imageCoordY %= imY;
    }

    printf("X: %u, Y: %u\n", (int) imageX, (int) imageY);
    printf("ImageX: %u, ImageY: %u\n", imageCoordX, imageCoordY);

    QString xPos = QString("X: %1").arg(imageCoordX);
    QString yPos = QString("\nY: %1").arg(imageCoordY);
    QString imgNr = QString("\nImage: %1").arg(imageNr);

    QString stringPos = xPos + yPos + imgNr;
    QToolTip::showText(cursor().pos(), stringPos, this);
}

void ImageViewerWidget::wheelEvent(QWheelEvent* event){
    /*
    QString underscoreString= name.split("_").at(1);
    QString number = underscoreString.split(".").at(0);
    int num = number.toInt()+(event->delta()/120);
    // time can't be bellow 0 
    if(num<0)
        return;
    QString name = QString("grupper_");
    QString numString = QString("%1").arg(num);
    for(int i=0;i<5-numString.length();i++)
        name = name + QString("%1").arg(0);
    name = name + QString("%1.pgm").arg(num);
    initializingImage(name);
    */
}

void ImageViewerWidget::scaleImageToWindow(const QSize& size) {
    labelImage.setPixmap(image.scaled(size, Qt::KeepAspectRatio));
}

void ImageViewerWidget::resizeEvent(QResizeEvent * resizeEvent) {
    QWidget::resizeEvent(resizeEvent);
    //size = resizeEvent->size();
    scaleImageToWindow(resizeEvent->size());
    showMaximized();
}