
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

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

#include "imageopenglwidget.h"
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
    void enableSubRegionsStateChanged(int);
    void numSubRegionsXChanged(int);
    void numSubRegionsYChanged(int);
    void zoomFactorChanged(const QString&);
    void zoomAreaSizeChanged(const QString&);

private:
    ImageOpenGLWidget imageWidget;
    QFileInfoList imageFiles;
    QString filename;
    QString path;
    QString fullPath;
    QPoint point;
    QSlider imageFileSlider;
    vector<vector<QPoint>> points;
    
    // "Sub-Region" GUI-elements
    QPushButton toggleHideSubRegionOptions;
    QCheckBox enableSubRegions;
    QSpinBox numSubRegionsX;
    QSpinBox numSubRegionsY;
    QLineEdit subRegionSizeX;
    QLineEdit subRegionSizeY;
    bool hideSubRegionOptions;
    
    // "Create Point-Series" GUI-elements
    bool hideCreatePointSeriesOptions;

    // "Create Bounding-Elements" GUI-elements
    QRadioButton createBoxes;
    QRadioButton createSpheres;


    // "Zoom Options" GUI-elements
    QComboBox zoomFactorComboBox;
    QComboBox zoomAreaSizeComboBox;

    int camerasNb;
    int pointsNb;
    int selectedImageInFolder;
    bool correspondingData;


    void setImageFromFile(const QString& filepath);
    void updateSubRegionSize();
    void initGUI();

    char* unpackPGMFile(const QString& filepath, int64_t* const bufferSize, int64_t* const imageWidth, int64_t* const imageHeight);
};

#endif // IMAGEVIEWERWIDGET_H
