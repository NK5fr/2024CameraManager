
#include "imageopenglwidget.h"

ImageOpenGLWidget::ImageOpenGLWidget(TrackPointProperty* trackPointProps, QWidget* parent) : QOpenGLWidget(parent) {
    trackPointProperty = trackPointProps;
    imageDetect = nullptr;
    mouseIn = false;
    imgBuffer = nullptr;
    bufferSize = 0;
    imageWidth = 0;
    imageHeight = 0;
    enableSubImages = false;
    showMouseOverCoordinateLabel = true;
    showMouseCross = false;
    numImageGroupsX = 3;
    numImageGroupsY = 2;
    setMouseTracking(true);
}

ImageOpenGLWidget::~ImageOpenGLWidget() {
    delete imageDetect;
}

void ImageOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    updateView();
}

void ImageOpenGLWidget::updateImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {
    if (imgBuffer == nullptr) return;
    if (this->imageWidth != imageWidth || this->imageHeight != imageHeight) {
        if (this->imgBuffer != nullptr) delete[] this->imgBuffer;
        this->imgBuffer = new unsigned char[bufferSize];
        this->imageWidth = imageWidth;
        this->imageHeight = imageHeight;
    }
    memcpy(this->imgBuffer, imgBuffer, bufferSize);
    this->bufferSize = bufferSize;
    if (imageDetect == nullptr) {
        imageDetect = new ImageDetect(imageWidth, imageHeight);
    }
}

void ImageOpenGLWidget::updateView() {
    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, this->width(), this->height());
    glMatrixMode(GL_MODELVIEW);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int width = this->width();
    int height = this->height();
    QSize s = size();
    glOrtho(0, this->width(), this->height(), 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);

    if (((float) width / height) > ((float) imageWidth / imageHeight)) {
        scaledImageArea.setHeight(height);
        scaledImageArea.setWidth(((float) imageWidth / imageHeight) * height);
        scaledImageArea.setTop(0);
        scaledImageArea.setLeft((width - (((float) imageWidth / imageHeight) * height)) / 2);
    } else {
        scaledImageArea.setHeight(((float) imageHeight / imageWidth) * width);
        scaledImageArea.setWidth(width);
        scaledImageArea.setTop((height - (((float) imageHeight / imageWidth) * width)) / 2);
        scaledImageArea.setLeft(0);
    }
}

void ImageOpenGLWidget::paintGL() {
    updateView();
    int subImageWidth = imageWidth / numImageGroupsX;
    int subImageHeight = imageHeight / numImageGroupsY;
    if (texture.getTextureWidth() != imageWidth || texture.getTextureHeight() != imageHeight) {
        texture.createEmptyTexture(imageWidth, imageHeight);
    }
    if (imageDetect != nullptr && trackPointProperty != nullptr) {
        imageDetect->setThreshold(trackPointProperty->thresholdValue);
        imageDetect->setMinPix(trackPointProperty->minPointValue);
        imageDetect->setMaxPix(trackPointProperty->maxPointValue);
        imageDetect->setSubwinSize(trackPointProperty->subwinValue);
        imageDetect->setMinSep(trackPointProperty->minSepValue);

        if (trackPointProperty->filteredImagePreview || trackPointProperty->trackPointPreview) {
            unsigned char* copyBuffer = new unsigned char[bufferSize];
            memcpy(copyBuffer, imgBuffer, bufferSize);
            imageDetect->setImage(copyBuffer);
        }

        if (trackPointProperty->filteredImagePreview) {
            imageDetect->imageRemoveBackground();
            texture.updateTexture(imageDetect->getFilteredImage(), bufferSize);
        }
        if (trackPointProperty->trackPointPreview) {
            imageDetect->imageDetectPoints();
            if (!trackPointProperty->filteredImagePreview) texture.updateTexture(imgBuffer, bufferSize);
        }
        if (!trackPointProperty->trackPointPreview && !trackPointProperty->filteredImagePreview) {
            texture.updateTexture(imgBuffer, bufferSize);
        }
        delete[] imageDetect->getImage();
        imageDetect->setImage(nullptr);
    } else texture.updateTexture(imgBuffer, bufferSize);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLineWidth(1);

    glEnable(GL_TEXTURE_2D);
    texture.bind();
    glTranslated(scaledImageArea.left(), scaledImageArea.top(), 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2d(0, 0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2d(scaledImageArea.width(), 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2d(scaledImageArea.width(), scaledImageArea.height());
    glTexCoord2f(0.0f, 1.0f);
    glVertex2d(0, scaledImageArea.height());
    glEnd();
    texture.unbind();
    glDisable(GL_TEXTURE_2D);

    if (showMouseCross && mouseIn) {
        float crossWingSize = (width() / scaledImageArea.width()) * 100;
        QPoint mPos = mousePos - scaledImageArea.topLeft();
        glColor4f(1, 0, 0, 1);
        glBegin(GL_LINES);
        glVertex3d(mPos.x() + (crossWingSize / 2), mPos.y(), 0);
        glVertex3d(mPos.x() - (crossWingSize / 2), mPos.y(), 0);
        glEnd();

        glBegin(GL_LINES);
        glVertex3d(mPos.x(), mPos.y() + (crossWingSize / 2), 0);
        glVertex3d(mPos.x(), mPos.y() - (crossWingSize / 2), 0);
        glEnd();
        glColor4f(1, 1, 1, 1);
    }

    if (enableSubImages) {
        glColor4f(1, 1, 0, 1);
        for (int xSubImgLine = 1; xSubImgLine < numImageGroupsX; xSubImgLine++) {
            glBegin(GL_LINES);
            glVertex3d(xSubImgLine * (scaledImageArea.width() / numImageGroupsX), 0, 0);
            glVertex3d(xSubImgLine * (scaledImageArea.width() / numImageGroupsX), scaledImageArea.height(), 0);
            glEnd();
        }
        for (int ySubImgLine = 1; ySubImgLine < numImageGroupsY; ySubImgLine++) {
            glBegin(GL_LINES);
            glVertex3d(0, ySubImgLine * (scaledImageArea.height() / numImageGroupsY), 0);
            glVertex3d(scaledImageArea.width(), ySubImgLine * (scaledImageArea.height() / numImageGroupsY), 0);
            glEnd();
        }
        glColor4f(1, 1, 1, 1);
    }

    if (imageDetect != nullptr && trackPointProperty != nullptr) {
        if (trackPointProperty->trackPointPreview) {
            ImPoint* points;
            int numPoints;
            if (trackPointProperty->removeDuplicates) {
                imageDetect->removeDuplicatePoints();
                points = imageDetect->getFinalPoints();
                numPoints = imageDetect->getFinalNumPoints();
            } else {
                points = imageDetect->getInitPoints();
                numPoints = imageDetect->getInitNumPoints();
            }
            int crossWingSize = (int) (height() / 75);
            if (trackPointProperty->showMinSepCircle) crossWingSize = ((double) scaledImageArea.width() / imageWidth) * trackPointProperty->minSepValue;
            glLineWidth(1.5);
            for (int i = 0; i < numPoints; i++) {
                int w = scaledImageArea.width();
                int h = scaledImageArea.height();
                double xPos = ((double) points[i].x * ((double) scaledImageArea.width() / imageWidth));
                double yPos = ((double) points[i].y * ((double) scaledImageArea.height() / imageHeight));

                glColor3f(1, 0, 0);
                glBegin(GL_LINES);
                glVertex2d(xPos - crossWingSize, yPos);
                glVertex2d(xPos + crossWingSize, yPos);
                glEnd();

                glBegin(GL_LINES);
                glVertex2d(xPos, yPos - crossWingSize);
                glVertex2d(xPos, yPos + crossWingSize);
                glEnd();
                glColor3f(1, 1, 1);

                if (trackPointProperty->showMinSepCircle) {
                    glColor3f(1, 1, 0);
                    float circleSize = crossWingSize;
                    const int num_segments = 30;
                    glBegin(GL_LINE_LOOP);
                    for (int ii = 0; ii < num_segments; ii++) {
                        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
                        float x = circleSize * cosf(theta);
                        float y = circleSize * sinf(theta);
                        glVertex2f(x + xPos, y + yPos);

                    }
                    glEnd();
                }
                glColor3f(1, 1, 1);

                if (trackPointProperty->showCoordinates) {
                    QPainter painter(this);
                    QPoint pos = scaledImageArea.topLeft() + QPoint(xPos, yPos);
                    painter.fillRect(pos.x(), pos.y(), 110, 12, Qt::white);
                    if (enableSubImages) {
                        painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(fmod(points[i].x, subImageWidth), 'f', 2) + " ,Y: " + QString::number(fmod(points[i].y, subImageHeight), 'f', 2));
                    } else {
                        painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(points[i].x, 'f', 2) + " ,Y: " + QString::number(points[i].y, 'f', 2));
                    }
                }
            }
        }
    }

    if (showMouseOverCoordinateLabel) {
        QPainter painter(this);
        QPoint pos = scaledImageArea.topLeft();
        painter.fillRect(pos.x(), pos.y(), 110, 12, Qt::white);
        if (enableSubImages && subImageWidth > 0 && subImageHeight > 0) {
            painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(mousePosInImage.x() % subImageWidth) + " ,Y: " + QString::number(mousePosInImage.y() % subImageHeight));
        } else {
            painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(mousePosInImage.x()) + " ,Y: " + QString::number(mousePosInImage.y()));
        }
    }
}

void ImageOpenGLWidget::enterEvent(QEvent *) {
    mouseIn = true;
}

void ImageOpenGLWidget::leaveEvent(QEvent *) {
    mouseIn = false;
    update();
}

void ImageOpenGLWidget::mouseMoveEvent(QMouseEvent * event) {
    mousePos = event->pos();
    
    mousePosInImage = mousePos - scaledImageArea.topLeft();
    mousePosInImage.setX(mousePosInImage.x() * ((double) imageWidth / scaledImageArea.width()));
    mousePosInImage.setY(mousePosInImage.y() * ((double) imageHeight / scaledImageArea.height()));
    
    update();
}
