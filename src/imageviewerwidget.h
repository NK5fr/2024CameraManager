#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include <QtWidgets/QMdiSubWindow>
#include <QString>
#include <QtWidgets/QLabel>
#include <QSize>
#include <QPoint>
#include <QtWidgets/qgridlayout.h>
#include <qfile.h>
#include <QImage>
#include <QtWidgets/QToolTip>
#include <QFile>
#include <QStringList>
#include <QWheelEvent>
#include <qdebug.h>
#include <QtCore/qmath.h>
#include <QtWidgets/qpushbutton.h>

#include "imagewidget.h"
#include "imageselectpointwidget.h"
#include "constants.h"

#include <vector>
#include <iostream>
#include <qdir.h>
#include <qfileinfo.h>


using namespace std;

class ImageViewerWidget : public QMdiSubWindow {
public:
    ImageViewerWidget(QString path, QString filename, TrackPointProperty* trackPoint = nullptr);
    ~ImageViewerWidget();

protected:
    //void mousePressEvent(QMouseEvent* mouseEvent);
    void wheelEvent(QWheelEvent* event);

private:
    QString filename;
    QString path;
    QString fullPath;
    QPoint point;
    vector<vector<QPoint>> points;
    ImageSelectPointWidget imageWidget;
    int camerasNb;
    int pointsNb;
    bool correspondingData;
    int selectedImageInFolder;
    QFileInfoList imageFiles;
    vector<QPixmap*> images;

    void setImageFromFile(const QString& filepath);
    //void initializingImage(const QString& filepath);
    //void initializingPoints();
};

#endif // IMAGEVIEWERWIDGET_H
