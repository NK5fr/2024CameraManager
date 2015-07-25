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
    Q_OBJECT
public:
    ImageViewerWidget(QString path, QString filename, TrackPointProperty* trackPoint = nullptr);
    ~ImageViewerWidget();

protected:
    //void mousePressEvent(QMouseEvent* mouseEvent);
    void wheelEvent(QWheelEvent* event);

private slots:
    void imageFileSliderChanged(int value);

private:
    QFileInfoList imageFiles;
    QString filename;
    QString path;
    QString fullPath;
    QPoint point;
    QSlider imageFileSlider;
    vector<vector<QPoint>> points;
    ImageSelectPointWidget imageWidget;
    int camerasNb;
    int pointsNb;
    int selectedImageInFolder;
    bool correspondingData;

    void setImageFromFile(const QString& filepath);
    //void initializingImage(const QString& filepath);
    //void initializingPoints();

    char* unpackPGMFile(const QString& filepath, int64_t* bufferSize, int64_t* imageWidth, int64_t* imageHeight);
};

#endif // IMAGEVIEWERWIDGET_H
