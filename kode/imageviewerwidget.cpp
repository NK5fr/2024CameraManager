#include "imageviewerwidget.h"

#include <QImage>
#include <QLabel>
#include <QToolTip>
#include <QFile>
#include <QStringList>
#include <QWheelEvent>

#include <QtCore/qmath.h>

#include <iostream>

using namespace std;

ImageViewerWidget::ImageViewerWidget(QString path, QString n, QSize s) :
    projectPath(path), relativePathToDatas("../../output"), size(s){

    initializingImage(n);
}


void ImageViewerWidget::initializingImage(QString n){
    name=n;
    fullPath=projectPath+"/"+name;
    QFile myFile(fullPath);

    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        cout << "Fail to open the file : "<< QString(fullPath).toUtf8().constData() << endl;
        return;
    }
    QPixmap image(fullPath);
    imageSize=image.size();
    QPixmap resized = image.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation);
    QLabel *label = new QLabel();
    label->setPixmap(resized);
    setWidget(label);
    setMaximumSize(image.size());
    setWindowFlags(Qt::SubWindow);
    setWindowTitle(name);
    initializingPoints();
}

void ImageViewerWidget::initializingPoints(){
    /* Initializing with wrong size */
    points = vector < vector < QPoint > >(50, vector < QPoint >(0));

    /* Having the number of the image (to load the right data file number) */
    QString underscoreString= name.split("_").at(1);
    QString number = underscoreString.split(".").at(0);

    /* Removing the first 0 because the data file has only 4 number and not 5 as the image file */
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

        /* Getting the line with points number */
        line=myFile.readLine();
        QStringList list = line.split(" ");
        /* Removing the last char because it is the \n */
        QString tmp = list.at(list.size()-1);
        pointsNb=tmp.mid(0, tmp.size()-1).toInt();
        points[camerasNb].resize(pointsNb);
        for(int i=0;i<pointsNb;i++){
            /* Spliting line with ' ' */
            line=myFile.readLine();
            list=line.split(" ");
            int j=0;
            /* Removing useless cells */
            while(j<list.size()){
                QString tmp = list.at(j);
                /* Removing useless spaces from cells */
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
}

void ImageViewerWidget::mousePressEvent(QMouseEvent *mouseEvent){
    if(!correspondingData)
        return;
    /* Getting the coordinates into the widget */
    QPoint realPoint = QWidget::mapFromGlobal(cursor().pos());
    /* Getting the right ratio */
    double widthRatio = imageSize.width()/size.width();
    double heightRatio = imageSize.height()/size.height();
    double ratio;
    if(widthRatio>heightRatio)
        ratio=widthRatio;
    else
        ratio=heightRatio;
    /* Setting the images coordinates */
    realPoint.setX(realPoint.x()*ratio);
    realPoint.setY(realPoint.y()*ratio);

    /* Getting the camera n°*/
    int columnNumber = realPoint.x()/(imageSize.width()/3);
    int lineNumber = realPoint.y()/(imageSize.height()/2);

    int cameraNumber = lineNumber*3+columnNumber;

    /* Setting the x and y coordinates according to the camera number */
    realPoint.setX(realPoint.x()%(imageSize.width()/3));
    realPoint.setY(realPoint.y()%(imageSize.height()/2));

    /* Getting the closest point */
    int ind=0;
    QPoint closest=points[cameraNumber][0];

    /* Calculating int distances in x and y axis */
    int xDist = qFabs(realPoint.x()-closest.x());
    int yDist = qFabs(realPoint.y()-closest.y());
    int distance=sqrt(qPow(xDist, 2) + qPow(yDist, 2));

    /* tmp var to compare */
    int tmpDistance;
    QPoint tmpPoint;

    for(int i=1;i<points[cameraNumber].size();i++){
        tmpPoint = points[cameraNumber][i];
        xDist = qFabs(realPoint.x()-tmpPoint.x());
        yDist = qFabs(realPoint.y()-tmpPoint.y());
        tmpDistance = sqrt(qPow(xDist, 2) + qPow(yDist, 2));
        if(distance>tmpDistance){
            distance=tmpDistance;
            closest=tmpPoint;
            ind=i;
        }
    }
    /* The point must be closer than 10 pixels in each axis */
    if(distance<sqrt(qPow(10, 2) + qPow(10, 2))){
        QString degree = QString::fromUtf8("\u00b0");
        QString camera = QString("Camera n"+degree+" : %1").arg(cameraNumber);
        QString pointNumber = QString("\nPoint n"+degree+" : %1").arg(ind);
        QString xPos = QString("\nx : %1").arg(closest.x());
        QString yPos = QString("\ny : %1").arg(closest.y());

        QString stringPos = camera + pointNumber + xPos + yPos;
        QToolTip::showText(cursor().pos(), stringPos, this);
    }
}

void ImageViewerWidget::wheelEvent(QWheelEvent *event){
    QString underscoreString= name.split("_").at(1);
    QString number = underscoreString.split(".").at(0);
    int num = number.toInt()+(event->delta()/120);
    /* time can't be bellow 0 */
    if(num<0)
        return;
    QString name = QString("grupper_");
    QString numString = QString("%1").arg(num);
    for(int i=0;i<5-numString.length();i++)
        name = name + QString("%1").arg(0);
    name = name + QString("%1.pgm").arg(num);
    initializingImage(name);

}
