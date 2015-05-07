
#include "imagewidget.h"

ImageWidget::ImageWidget() {
    setMouseTracking(true);
    mouseInside = false;
    showMagnified = false;
    numImagesWidth = 3;
    numImagesHeight = 2;
    subImagesEnabled = true;
    pointSeriesNr = 0;
    startX = 0;
    startY = 0;
}

ImageWidget::~ImageWidget() {

}

void ImageWidget::resizeEvent(QResizeEvent* resizeEvent) {
    //QLabel::resizeEvent(resizeEvent);
    imageWidgetSize = resizeEvent->size();
}

void ImageWidget::paintEvent(QPaintEvent* event) {
    //QLabel::paintEvent(event);
    QPainter painter(this);
    QFont font(painter.font());
    font.setPixelSize(15);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);
    QPixmap scaledImage = image.scaled(imageWidgetSize, Qt::KeepAspectRatio);
    
    startX = (size().width() - scaledImage.width()) / 2;
    startY = (size().height() - scaledImage.height()) / 2;

    if (size() != scaledImage.size()) {
        //resize(scaledImage.size());
        //update();
        //updateGeometry();
    }
    
    QPen whitePen(QColor(255, 255, 255), 1);
    QPen blackPen(QColor(0, 0, 0), 1);
    QPen redPen(QColor(255, 0, 0), 2);
    QPen greenPen(QColor(0, 255, 0), 2);
    painter.setPen(whitePen);
    painter.drawPixmap(startX, startY, scaledImage);

    QSize widgetSize = scaledImage.size();
    scaledImageWidth = widgetSize.width();
    scaledImageHeight = widgetSize.height();

    if ((widgetSize.width() / widgetSize.height()) > (image.width() / image.height())) { // If background is wider than image (Sides are blank)
        scaledImageWidth = (image.width() * widgetSize.height()) / image.height();
    } else { // If background is smaller (in width) than image (top and bottom is blank)
        scaledImageHeight = (widgetSize.width() * image.height()) / image.width();
    }
    scaledSubImageSizeX = subImageSize.width() * ((double) scaledImageWidth / image.width());
    scaledSubImageSizeY = subImageSize.height() * ((double) scaledImageHeight / image.height());

    if (subImagesEnabled) {
        for (double x = 0; x < scaledImageWidth; x += scaledSubImageSizeX) {
            painter.drawLine(x + startX, startY, x + startX, scaledImageHeight + startY);
            for (double y = 0; y < scaledImageHeight; y += scaledSubImageSizeY) {
                painter.drawLine(startX, y + startY, scaledImageWidth + startX, y + startY);
            }
        }
    }

    painter.setPen(redPen);
    //for (int y = 0; y < (image.height() / subImageSize.height()); y++) {
    //    for (int x = 0; x < (image.width() / subImageSize.width()); x++) {
    for (int pointSeries = 0; pointSeries < points.size(); pointSeries++) {
        for (int cam = 0; cam < points[pointSeries].size(); cam++) {
            TrackPoint::PointInCamera* point = points[pointSeries][cam];
            if (point == nullptr) continue;
            if (point->isInitialized) {
                int x = point->camNo % (image.width() / subImageSize.width());
                int y = point->camNo / (image.width() / subImageSize.width());
                if (!point->isUsed) {
                    //painter.drawLine(x * scaledSubImageSizeX + startX, y * scaledSubImageSizeY + startY, (x + 1) * scaledSubImageSizeX + startX, (y + 1) * scaledSubImageSizeY + startY);
                    //painter.drawLine(x * scaledSubImageSizeX + startX, (y + 1) * scaledSubImageSizeY + startY, (x + 1) * scaledSubImageSizeX + startX, y * scaledSubImageSizeY + startY);
                } else {
                    const int crossSizePrime = 20;
                    const int crossSizeSecondary = 5;
                    int crossSize = crossSizeSecondary;
                    if (pointSeries == pointSeriesNr) {
                        crossSize = crossSizePrime;
                        painter.setPen(greenPen);
                    }
                    if (point->pointX > 0 && point->pointY > 0) {
                        int xPos = (((double) point->pointX / subImageSize.width()) * scaledSubImageSizeX) + (scaledSubImageSizeX * x) + startX;
                        int yPos = (((double) point->pointY / subImageSize.height()) * scaledSubImageSizeY) + (scaledSubImageSizeY * y) + startY;
                        painter.drawLine(xPos, yPos - crossSize, xPos, yPos + crossSize);
                        painter.drawLine(xPos - crossSize, yPos, xPos + crossSize, yPos);
                        painter.setPen(blackPen);
                        painter.fillRect(xPos, yPos, ((pointSeries < 9) ? 11 : 20), 15, Qt::white);
                        painter.drawText(xPos + 1, yPos + 13, QString::number(pointSeries + 1));
                        painter.setPen(redPen);
                    }
                    if (pointSeries == pointSeriesNr) {
                        painter.setPen(redPen);
                    }
                }
            }
        }
    }
    painter.setPen(whitePen);

    if (mouseInside) {
        //if (mouseInside && imagePosX < scaledImageWidth && imagePosY < scaledImageHeight) {
        int xPos = imagePosX;
        int yPos = imagePosY;
        //painter.
        painter.drawLine(QLine(xPos, yPos + 20, xPos, yPos - 20));
        painter.drawLine(QLine(xPos + 20, yPos, xPos - 20, yPos));

        int imageCoordX = (int) (((imagePosX - startX) / (double) scaledImageWidth) * image.width());
        int imageCoordY = (int) (((imagePosY - startY) / (double) scaledImageHeight) * image.height());

        const bool showCoordinates = true;
        if (showCoordinates) {
            painter.setPen(blackPen);
            painter.fillRect(0 + startX, 0 + startY, 110, 16, Qt::white);
            painter.drawText(4 + startX, 12 + startY, "X: " + QString::number(imageCoordX % subImageSize.width()) + " Y: " + QString::number(imageCoordY % subImageSize.height()));
            painter.setPen(whitePen);
        }
        if (showMagnified) {
            const int magnifiedAreaSize = 25;
            const int magnifierSize = 200;

            if (xPos + magnifierSize > widgetSize.width()) xPos -= magnifierSize;
            if (yPos + magnifierSize > widgetSize.height()) yPos -= magnifierSize;

            QPixmap magnifiedImage = image.copy(imageCoordX - (magnifiedAreaSize / 2), imageCoordY - (magnifiedAreaSize / 2), magnifiedAreaSize, magnifiedAreaSize);
            magnifiedImage = magnifiedImage.scaled(QSize(magnifierSize, magnifierSize), Qt::KeepAspectRatio);
            painter.drawPixmap(xPos, yPos, magnifiedImage);
            painter.drawLine(QLine(xPos + (magnifierSize / 2), yPos, xPos + (magnifierSize / 2), yPos + magnifierSize));
            painter.drawLine(QLine(xPos, yPos + (magnifierSize / 2), xPos + magnifierSize, yPos + (magnifierSize / 2)));
            painter.drawRect(xPos, yPos, magnifierSize, magnifierSize);
        }
    }
    painter.end();
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event) {
    int dx = event->x() - mousePos.x();
    int dy = event->y() - mousePos.y();
    mousePos = event->pos();
    if (showMagnified) {
        const double sensitivity = 40;
        imagePosX += ((double) dx) / sensitivity;
        imagePosY += ((double) dy) / sensitivity;
    } else {
        imagePosX = mousePos.x();
        imagePosY = mousePos.y();
    }
    update();
}

void ImageWidget::enterEvent(QEvent* event) {
    mouseInside = true;
}

void ImageWidget::leaveEvent(QEvent* event) {
    mouseInside = false;
    update();
}

void ImageWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        showMagnified = true;
        setCursor(Qt::BlankCursor);
        update();
    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event) {
    QPoint pos = mapToGlobal(QPoint(imagePosX, imagePosY));
    QCursor::setPos(pos);
    imagePosX -= startX;
    imagePosY -= startY;
    if (imagePosX >= 0 && imagePosX < scaledImageWidth && imagePosY >= 0 && imagePosY < scaledImageHeight) {
        if (pointSeriesNr >= points.size()) points.resize(pointSeriesNr + 1);
        int indexX = (int) (imagePosX / scaledSubImageSizeX);
        int indexY = (int) (imagePosY / scaledSubImageSizeY);
        if (points[pointSeriesNr].size() < ((image.width() / subImageSize.width()) * (image.height() / subImageSize.height()))) {
            points[pointSeriesNr].resize((image.width() / subImageSize.width()) * (image.height() / subImageSize.height()));
        }
        TrackPoint::PointInCamera* point = points[pointSeriesNr][(indexY * (image.width() / subImageSize.width())) + indexX];
        if (event->button() == Qt::MouseButton::LeftButton) {
            int xPos = (((double) imagePosX) / scaledSubImageSizeX) * subImageSize.width();
            int yPos = (((double) imagePosY) / scaledSubImageSizeY) * subImageSize.height();
            xPos %= subImageSize.width();
            yPos %= subImageSize.height();
            if (point == nullptr) {
                point = new TrackPoint::PointInCamera(xPos, yPos, (indexY * (image.width() / subImageSize.width())) + indexX);
                points[pointSeriesNr][(indexY * (image.width() / subImageSize.width())) + indexX] = point;
            } else {
                point->camNo = (indexY * (image.width() / subImageSize.width())) + indexX;
                point->pointX = xPos;
                point->pointY = yPos;
                //point->isUsed = true;
                //point->isInitialized = true;
            }
        } else if (event->button() == Qt::MouseButton::RightButton && point != nullptr) {
            point->isUsed = !point->isUsed;
            point->isInitialized = true;
        }
        //points[(indexY * (image.width() / subImageSize.width())) + indexX] = point;
    }
    showMagnified = false;
    unsetCursor();
    imagePosX += startX;
    imagePosY += startY;
    update();
}

void ImageWidget::setImage(QPixmap& image) {
    if (image.size().width() % numImagesWidth == 0 || image.size().height() % numImagesHeight == 0) {
        subImageSize = QSize(image.size().width() / numImagesWidth, image.size().height() / numImagesHeight);
        this->image = image;
    }
}

void ImageWidget::nextPointSeries() {
    pointSeriesNr++;
    update();
}

void ImageWidget::backPointSeries() {
    pointSeriesNr = std::max(0, pointSeriesNr - 1);
    update();
}