#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include <QMdiSubWindow>
#include <QString>
#include <QLabel>
#include <QSize>
#include <QPoint>

#include <vector>

using namespace std;

class ImageViewerWidget : public QMdiSubWindow{
public:
    ImageViewerWidget(QString path, QString n, QSize s);
protected:
    void mousePressEvent(QMouseEvent *mouseEvent);
    void wheelEvent(QWheelEvent *event);

private:
    void initializingImage(QString n);
    QString name;
    QString projectPath;
    QString relativePathToDatas;
    QString fullPath;

    bool correspondingData;

    QSize size;
    QSize imageSize;
    QPoint point;
    QLabel *labelImage;

    int camerasNb;
    int pointsNb;
    void initializingPoints();
    vector < vector < QPoint > > points;

};

#endif // IMAGEVIEWERWIDGET_H
