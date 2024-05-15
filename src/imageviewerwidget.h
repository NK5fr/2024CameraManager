
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
    void enablePointStateChanged(int);
    void customPointStringStateChanged(int);
    void singlePointClicked();
    void multiplePointClicked();
    void createPointFileClicked();

private:
    ImageOpenGLWidget imageWidget;
    QFileInfoList imageFiles;
    QString filename;
    QString path;
    QString fullPath;
    QPoint point;
    QSlider imageFileSlider;
    std::vector<std::vector<QPoint>> points;
    
    // "Sub-Region" GUI-elements
    QPushButton toggleHideSubRegionOptions;
    QCheckBox enableSubRegions;
    QSpinBox numSubRegionsX;
    QSpinBox numSubRegionsY;
    QLineEdit subRegionSizeX;
    QLineEdit subRegionSizeY;
    
    // "Create Point-Series" GUI-elements
    QCheckBox enablePointSeries;
    QCheckBox enablePointStringLabel;
    QRadioButton singlePointSeries;
    QRadioButton multiplePointSeries;
    QPushButton createPointFile;

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

class PointFileCreatorDialog : public QDialog {
    Q_OBJECT
public:
    PointFileCreatorDialog(QWidget* parent = nullptr);
    
    const PointFileInfo& getFileInfo() { return this->fileInfo; }

private slots:
    void okButtonClicked();
    void filePathUpdateClicked();
    void allPointsPerImageChecked();
    void onePointForAllImagesChecked();
    void filePathChanged(const QString&);

private:
    PointFileInfo fileInfo;
    QLineEdit filePath;
    QRadioButton allPointsPerImageFirst;
    QRadioButton onePointPerImageFirst;
    QCheckBox sortByString;
    QPushButton filePathUpdate;
    QPushButton okButton;
    QPushButton closeButton;
};

#endif // IMAGEVIEWERWIDGET_H
