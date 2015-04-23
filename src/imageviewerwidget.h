#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include <QMdiSubWindow>
#include <QString>
#include <QLabel>
#include <QSize>
#include <QPoint>
#include <qfile.h>

#include <vector>

using namespace std;

class ImageViewerWidget : public QMdiSubWindow {
public:
    ImageViewerWidget(QString path, QString filename);

    void scaleImageToWindow(const QSize& size);

protected:
    void mousePressEvent(QMouseEvent *mouseEvent);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent * resizeEvent);

private:
    QString filename;
    QString path;
    QString fullPath;
    QSize imageSize;
    QPixmap image;
    QPoint point;
    QLabel labelImage;
    vector<vector<QPoint>> points;
    int camerasNb;
    int pointsNb;
    bool correspondingData;

    void setImageFromFile(const QString& filepath);
    //void initializingImage(const QString& filepath);
    void initializingPoints();
};

#endif // IMAGEVIEWERWIDGET_H
