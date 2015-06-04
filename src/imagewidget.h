#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtWidgets/qframe.h>
#include <QtWidgets/qwidget.h>
#include <qevent.h>
#include <QtWidgets/qlabel.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qline.h>
#include <vector>

#include "datastructs.h"

class ImageWidget : public QLabel {
public:
    ImageWidget();
    ~ImageWidget();

    void setImage(QPixmap& image);
    std::vector<std::vector<TrackPoint::PointInCamera*>> getPoints() { return this->points; }

    inline void setNumImagesWidth(int numImagesWidth) { this->numImagesWidth = numImagesWidth; }
    inline void setNumImagesHeight(int numImagesHeight) { this->numImagesHeight = numImagesHeight; }
    inline int getNumImagesWidth() { return numImagesWidth; }
    inline int getNumImagesHeight() { return numImagesHeight; }
    void nextPointSeries();
    void backPointSeries();

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QPixmap image;
    QPointF mousePos;
    QSize imageWidgetSize;
    QSize subImageSize;
    int pointSeriesNr;
    std::vector<std::vector<TrackPoint::PointInCamera*>> points;
    double imagePosX;
    double imagePosY;
    double scaledSubImageSizeX;
    double scaledSubImageSizeY;
    int scaledImageWidth;
    int scaledImageHeight;
    int startX;
    int startY;
    int numImagesHeight;
    int numImagesWidth;
    bool mouseInside;
    bool showMagnified;
    bool subImagesEnabled;
};

#endif
