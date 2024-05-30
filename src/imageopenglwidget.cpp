
#include "imageopenglwidget.h"

ImageOpenGLWidget::ImageOpenGLWidget(bool colored, TrackPointProperty* trackPointProps, QWidget* parent) : QOpenGLWidget(parent) {
    this->colored = colored;
    trackPointProperty = trackPointProps;
    imageDetect = nullptr;
    mouseIn = false;
    imgBuffer = nullptr;
    bufferSize = 0;
    imageWidth = 0;
    imageHeight = 0;
    enableSubImages = true;
    showMouseOverCoordinateLabel = true;
    showMouseCross = true;
    showBoundingAreas = false;
    numImageGroupsX = 3;
    numImageGroupsY = 3;
    setMouseTracking(true);

    // Test-code for boxes and circles, used for drawing masking-areas etc.
    // Maybe about 70-80 % finished bounding-area management code.
    // This bounding-area imformation can be sent to TrackPoint-software for removing unwanted points.
    // 16. aug. 2015
    /*
    QRectF* box = new QRectF(QPointF(200, 700), QSizeF(300, 100));
    boundingBoxes.push_back(box);

    CircleF* s = new CircleF();
    s->pos.x = 640;
    s->pos.y = 600;
    s->radius = 200;
    boundingCircles.push_back(s);
    showBoundingAreas = true;
    */
}

ImageOpenGLWidget::~ImageOpenGLWidget() {
    delete imageDetect;
}

void ImageOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    // used to change the OpenGL background
    //glClearColor(1.0, 1.0, 1.0, 1.0);
    updateView();
}

void ImageOpenGLWidget::updateImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {
    //qInfo() << "ioglw" << this->size();
    //qInfo() << "ioglw" << this->size();

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
    newImageReady = true;
}

void ImageOpenGLWidget::updateView() {
    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glViewport(0, 0, this->width(), this->height());
    // qInfo() << "image w/h \t\t" << imageWidth << "/" << imageHeight;
    qInfo() << "subwin w/h \t\t" << this->width() << "/" << this->height();
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    glMatrixMode(GL_MODELVIEW);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int width = this->width();
    int height = this->height();
    //QSize s = size();
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

    // qInfo() << scaledImageArea.width() << "/" << scaledImageArea.height();
    // qInfo() << scaledImageArea.top() << "/" << scaledImageArea.left();


    // QPixmap scaled = QPixmap(imageWidth, imageHeight).scaled(this->width(), this->height(), Qt::KeepAspectRatio);
    // scaledImageArea.setTopLeft(scaled.rect().topLeft());
    // scaledImageArea.setBottomRight(scaled.rect().bottomRight());
    //qInfo() << "scaled w/h \t\t" << scaledImageArea.width() << "/" << scaledImageArea.height();

}

void ImageOpenGLWidget::paintGL() {
    updateView();
    int subImageWidth = imageWidth / numImageGroupsX;
    int subImageHeight = imageHeight / numImageGroupsY;

    imageToScreenCoordX = ((double) scaledImageArea.width() / imageWidth);
    imageToScreenCoordY = ((double) scaledImageArea.height() / imageHeight);
    screenToImageCoordX = ((double) imageWidth / scaledImageArea.width());
    screenToImageCoordY = ((double) imageHeight / scaledImageArea.height());

    if (texture.getTextureWidth() != imageWidth || texture.getTextureHeight() != imageHeight) {
        if(colored){
            texture.createEmptyTexture(imageWidth, imageHeight, OpenGL::PixelFormat::RGB);
        }else{
            texture.createEmptyTexture(imageWidth, imageHeight);
        }
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
    } else {
        texture.updateTexture(imgBuffer, bufferSize);
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
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

    if (showBoundingAreas) {
        for (int i = 0; i < boundingCircles.size(); i++) {
            CircleF* mySphere = boundingCircles[i];
            float circleSize = mySphere->radius * imageToScreenCoordX;
            const int num_segments = 30;

            double xPos = (mySphere->pos.x * imageToScreenCoordX);
            double yPos = (mySphere->pos.y * imageToScreenCoordY);

            glPushMatrix();
            glTranslated(xPos, yPos, 0);
            /*if (selectedBoundingCircle == i && !selectedBoundingCircleEdge) {
                glColor4f(0, 1, 0, 0.7f);
            } else */glColor4f(0, 1, 0, 0.3f);
            glBegin(GL_POLYGON);
            for (double i = 0; i < 2 * 3.1415926f; i += 3.1415926f / num_segments)
                glVertex3f(cos(i) * circleSize, sin(i) * circleSize, 0.0);
            glEnd();
            glPopMatrix();

            glColor4f(0, 1, 0, 1);
            if (selectedBoundingCircle == i && selectedBoundingCircleEdge) {
                glLineWidth(1);
                glBegin(GL_LINE_LOOP);
                for (int ii = 0; ii < num_segments; ii++) {
                    float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
                    float x = (circleSize - boundingCircleEdgeThreshold) * cosf(theta);
                    float y = (circleSize - boundingCircleEdgeThreshold) * sinf(theta);
                    glVertex2f(x + xPos, y + yPos);

                }
                glEnd();

                glBegin(GL_LINE_LOOP);
                for (int ii = 0; ii < num_segments; ii++) {
                    float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
                    float x = (circleSize + boundingCircleEdgeThreshold) * cosf(theta);
                    float y = (circleSize + boundingCircleEdgeThreshold) * sinf(theta);
                    glVertex2f(x + xPos, y + yPos);

                }
                glEnd();
                glLineWidth(1);
            } else {
                glBegin(GL_LINE_LOOP);
                for (int ii = 0; ii < num_segments; ii++) {
                    float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
                    float x = circleSize * cosf(theta);
                    float y = circleSize * sinf(theta);
                    glVertex2f(x + xPos, y + yPos);

                }
                glEnd();
            }

            glColor4f(0, 1, 0, 0.5f);
            double crossSize = circleSize;
            glBegin(GL_LINES);
            glVertex2f(xPos - crossSize, yPos);
            glVertex2f(xPos + crossSize, yPos);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(xPos, yPos - crossSize);
            glVertex2f(xPos, yPos + crossSize);
            glEnd();
        }
        
        for (int i = 0; i < boundingBoxes.size(); i++) {
            QRectF* myBox = boundingBoxes[i];

            glColor4f(0, 1, 0, 0.3f);
            glBegin(GL_POLYGON);
            glVertex3d(myBox->topLeft().x() * imageToScreenCoordX,
                       myBox->topLeft().y() * imageToScreenCoordY,
                       0);
            glVertex3d(myBox->topRight().x() * imageToScreenCoordX,
                       myBox->topRight().y() * imageToScreenCoordY,
                       0);
            glVertex3d(myBox->bottomRight().x() * imageToScreenCoordX,
                       myBox->bottomRight().y() * imageToScreenCoordY,
                       0);
            glVertex3d(myBox->bottomLeft().x() * imageToScreenCoordX,
                       myBox->bottomLeft().y() * imageToScreenCoordY,
                       0);
            glEnd();


            glColor4f(0, 1, 0, 1);
            glBegin(GL_LINE_LOOP);
            glVertex3d(myBox->topLeft().x() * imageToScreenCoordX,
                       myBox->topLeft().y() * imageToScreenCoordY,
                       0);
            glVertex3d(myBox->topRight().x() * imageToScreenCoordX,
                       myBox->topRight().y() * imageToScreenCoordY,
                       0);
            glVertex3d(myBox->bottomRight().x() * imageToScreenCoordX,
                       myBox->bottomRight().y() * imageToScreenCoordY,
                       0);
            glVertex3d(myBox->bottomLeft().x() * imageToScreenCoordX,
                       myBox->bottomLeft().y() * imageToScreenCoordY,
                       0);
            glEnd();

            glLineWidth(1);
            int cornerBoxSize = 3;
            if (selectedBoundingBoxCorner == 0) cornerBoxSize *= 2;
            QPointF corner = myBox->topLeft();
            glBegin(GL_LINE_LOOP);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glEnd();
            if (selectedBoundingBoxCorner == 0) cornerBoxSize /= 2;
            if (selectedBoundingBoxCorner == 1) cornerBoxSize *= 2;
            corner = myBox->topRight();
            glBegin(GL_LINE_LOOP);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glEnd();
            if (selectedBoundingBoxCorner == 1) cornerBoxSize /= 2;
            if (selectedBoundingBoxCorner == 2) cornerBoxSize *= 2;
            corner = myBox->bottomLeft();
            glBegin(GL_LINE_LOOP);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glEnd();
            if (selectedBoundingBoxCorner == 2) cornerBoxSize /= 2;
            if (selectedBoundingBoxCorner == 3) cornerBoxSize *= 2;
            corner = myBox->bottomRight();
            glBegin(GL_LINE_LOOP);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX + cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY - cornerBoxSize,
                       0);
            glVertex3d(corner.x() * imageToScreenCoordX - cornerBoxSize,
                       corner.y() * imageToScreenCoordY + cornerBoxSize,
                       0);
            glEnd();
            if (selectedBoundingBoxCorner == 3) cornerBoxSize /= 2;

            glLineWidth(1);
            glColor4f(0, 1, 0, 0.5f);
            glBegin(GL_LINES);
            glVertex2f(myBox->topLeft().x() * imageToScreenCoordX + (myBox->width() * imageToScreenCoordX / 2),
                       myBox->topLeft().y() * imageToScreenCoordY);
            glVertex2f(myBox->topLeft().x() * imageToScreenCoordX + (myBox->width() * imageToScreenCoordX / 2),
                       myBox->bottomLeft().y() * imageToScreenCoordY);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(myBox->topLeft().x() * imageToScreenCoordX,
                       myBox->topLeft().y() * imageToScreenCoordY + (myBox->height() * imageToScreenCoordY / 2));
            glVertex2f(myBox->topRight().x() * imageToScreenCoordX,
                       myBox->topLeft().y() * imageToScreenCoordY + (myBox->height() * imageToScreenCoordY / 2));
            glEnd();
        }
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
            if (trackPointProperty->showMinSepCircle) crossWingSize = imageToScreenCoordX * trackPointProperty->minSepValue;
            glLineWidth(1);
            for (int i = 0; i < numPoints; i++) {
                double xPos = ((double) points[i].x * imageToScreenCoordX);
                double yPos = ((double) points[i].y * imageToScreenCoordY);

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

                if (trackPointProperty->showCoordinates && !showZoomArea) {
                    QPainter painter(this);
                    QPoint pos = scaledImageArea.topLeft() + QPoint(xPos, yPos);
                    painter.fillRect(pos.x(), pos.y(), 110, 12, Qt::gray);
                    if (enableSubImages) {
                        painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(fmod(points[i].x, subImageWidth), 'f', 2) + " ,Y: " + QString::number(fmod(points[i].y, subImageHeight), 'f', 2));
                    } else {
                        painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(points[i].x, 'f', 2) + " ,Y: " + QString::number(points[i].y, 'f', 2));
                    }
                }
            }
            glLineWidth(1);
        }
    }

    if (showPointSeries) {
        double crossWingSize = ((double) height() / 30);
        double diffFromCross = selectedPointThreshold;
        pointCrossWingSize = crossWingSize;
        glColor3f(0, 1, 0);
        if (selectedPoint >= 0) {
            double xPos = pointSeries[selectedPoint]->pointX * imageToScreenCoordY; // Screen-coordinates
            double yPos = pointSeries[selectedPoint]->pointY * imageToScreenCoordY; // Screen-coordinates

            glBegin(GL_LINE_LOOP);
            glVertex2d(xPos - crossWingSize - diffFromCross, yPos - diffFromCross);
            glVertex2d(xPos - crossWingSize - diffFromCross, yPos + diffFromCross);
            glVertex2d(xPos - diffFromCross, yPos + diffFromCross);
            glVertex2d(xPos - diffFromCross, yPos + crossWingSize + diffFromCross);
            glVertex2d(xPos + diffFromCross, yPos + crossWingSize + diffFromCross);
            glVertex2d(xPos + diffFromCross, yPos + diffFromCross);
            glVertex2d(xPos + crossWingSize + diffFromCross, yPos + diffFromCross);
            glVertex2d(xPos + crossWingSize + diffFromCross, yPos - diffFromCross);
            glVertex2d(xPos + diffFromCross, yPos - diffFromCross);
            glVertex2d(xPos + diffFromCross, yPos - crossWingSize - diffFromCross);
            glVertex2d(xPos - diffFromCross, yPos - crossWingSize - diffFromCross);
            glVertex2d(xPos - diffFromCross, yPos - diffFromCross);
            glEnd();
        }
        //glLineWidth(2);
        for (int i = 0; i < pointSeries.size(); i++) {
            glColor3f(0, 1, 0);
            double xPos = pointSeries[i]->pointX * imageToScreenCoordY; // Screen-coordinates
            double yPos = pointSeries[i]->pointY * imageToScreenCoordY; // Screen-coordinates

            glBegin(GL_LINES);
            glVertex2d(xPos - crossWingSize, yPos);
            glVertex2d(xPos + crossWingSize, yPos);
            glEnd();

            glBegin(GL_LINES);
            glVertex2d(xPos, yPos - crossWingSize);
            glVertex2d(xPos, yPos + crossWingSize);
            glEnd();
            if (showPointSeriesString) {
                QString s = pointSeries[i]->string;
                QPainter painter(this);
                painter.setFont(QFont("Courier"));
                QPoint pos = scaledImageArea.topLeft() + QPoint(xPos, yPos);
                painter.fillRect(pos.x(), pos.y(), 10 * s.size(), 12, Qt::white);
                painter.drawText(pos + QPoint(2, 10), s);
            }
        }
        glColor3f(1, 1, 1);
        //glLineWidth(1);
    }

    if (showMouseCross && mouseIn) {
        float crossWingSize = (width() / scaledImageArea.width()) * 50;
        QPointF mPos = mousePos - scaledImageArea.topLeft();
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

        if (showZoomArea) {
            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            texture.bind();
            if ((mPos.x() + scaledImageArea.left()) + zoomSize > width()) mPos.setX(mPos.x() - zoomSize);
            if ((mPos.y() + scaledImageArea.top()) + zoomSize > height()) mPos.setY(mPos.y() - zoomSize);
            glTranslated(mPos.x(), mPos.y(), 0);

            double texCoordX = (double) mousePosInImage.x() / imageWidth;
            double texCoordY = (double) mousePosInImage.y() / imageHeight;
            double texSizeX = (zoomSize * screenToImageCoordX) / (imageWidth * zoomFactor * 2);
            double texSizeY = (zoomSize * screenToImageCoordY) / (imageHeight * zoomFactor * 2);
            glBegin(GL_QUADS);
            glTexCoord2f(texCoordX - texSizeX, texCoordY - texSizeY);
            glVertex2d(0, 0);
            glTexCoord2f(texCoordX + texSizeX, texCoordY - texSizeY);
            glVertex2d(zoomSize, 0);
            glTexCoord2f(texCoordX + texSizeX, texCoordY + texSizeY);
            glVertex2d(zoomSize, zoomSize);
            glTexCoord2f(texCoordX - texSizeX, texCoordY + texSizeY);
            glVertex2d(0, zoomSize);
            glEnd();
            texture.unbind();
            glDisable(GL_TEXTURE_2D);
            
            glColor4f(1, 0, 0, 1);
            glBegin(GL_LINE_LOOP);
            glVertex2d(0, 0);
            glVertex2d(0, zoomSize);
            glVertex2d(zoomSize, zoomSize);
            glVertex2d(zoomSize, 0);
            glEnd();

            glBegin(GL_LINES);
            glVertex2d(zoomSize / 2, 0);
            glVertex2d(zoomSize / 2, zoomSize);
            glEnd();
            glBegin(GL_LINES);
            glVertex2d(0, zoomSize / 2);
            glVertex2d(zoomSize, zoomSize / 2);
            glEnd();
            glColor4f(1, 1, 1, 1);

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
                    int crossWingSize = 20;
                    if (trackPointProperty->showMinSepCircle) crossWingSize = imageToScreenCoordX * trackPointProperty->minSepValue;
                    double scaleWidth = imageToScreenCoordX;
                    double scaleHeight = imageToScreenCoordY;
                    double zoomHalfSizeInImageX = (zoomSize * screenToImageCoordX) / (2 * zoomFactor);
                    double zoomHalfSizeInImageY = (zoomSize * screenToImageCoordY) / (2 * zoomFactor);
                    double adjustX = ((mPos.x() + scaledImageArea.left()) + zoomSize > width()) ? -zoomSize : zoomSize;
                    double adjustY = ((mPos.y() + scaledImageArea.top()) + zoomSize > height()) ? -zoomSize : zoomSize;
                    for (int i = 0; i < numPoints; i++) {
                        if (points[i].x > mousePosInImage.x() + zoomHalfSizeInImageX || points[i].x < mousePosInImage.x() - zoomHalfSizeInImageX) continue;
                        if (points[i].y > mousePosInImage.y() + zoomHalfSizeInImageY || points[i].y < mousePosInImage.y() - zoomHalfSizeInImageY) continue;

                        double diffX = points[i].x - mousePosInImage.x(); // Image-coordinates
                        double diffY = points[i].y - mousePosInImage.y(); // Image-coordinates
                        double diffXZoomArea = (diffX * imageToScreenCoordX * zoomFactor); // Screen-coordinates
                        double diffYZoomArea = (diffY * imageToScreenCoordY * zoomFactor); // Screen-coordinates
                        double xPos = (adjustX / 2) + diffXZoomArea; // Screen-coordinates
                        double yPos = (adjustY / 2) + diffYZoomArea; // Screen-coordinates

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

                        if (trackPointProperty->showCoordinates) {
                            QPainter painter(this);
                            QPoint pos = scaledImageArea.topLeft() + QPoint(xPos + mPos.x(), yPos + mPos.y());
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
            glPopMatrix();
        }
    }

    if (showMouseOverCoordinateLabel) {
        QPainter painter(this);
        QPoint pos = scaledImageArea.topLeft();
        painter.fillRect(pos.x(), pos.y(), 110, 12, Qt::gray);
        if (enableSubImages && subImageWidth > 0 && subImageHeight > 0) {
            painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(std::fmod(mousePosInImage.x(), subImageWidth), 'f', 2) + " ,Y: " + QString::number(std::fmod(mousePosInImage.y(), subImageHeight), 'f', 2));
        } else {
            painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(mousePosInImage.x(), 'f', 2) + " ,Y: " + QString::number(mousePosInImage.y(), 'f', 2));
        }
    }
}

void ImageOpenGLWidget::enterEvent(QEvent *) {
    mouseIn = true;
    setCursor(QCursor(Qt::BlankCursor));
}

void ImageOpenGLWidget::leaveEvent(QEvent *) {
    mouseIn = false;
    leftMouseButtonDown = false;
    selectedBoundingCircle = -1;
    selectedBoundingCircleEdge = false;
    selectedBoundingBox = -1;
    selectedBoundingBoxCorner = -1;
    unsetCursor();
    update();
}

void ImageOpenGLWidget::mousePressEvent(QMouseEvent* event) {
    QOpenGLWidget::mousePressEvent(event);
    if (event->button() == Qt::MouseButton::LeftButton) {
        leftMouseButtonDown = true;
        showZoomArea = true;
        mouseDragStart = event->localPos() - scaledImageArea.topLeft();
        mouseDragStart.setX(mouseDragStart.x() * ((double) imageWidth / scaledImageArea.width()));
        mouseDragStart.setY(mouseDragStart.y() * ((double) imageHeight / scaledImageArea.height()));
        update();
    }
    if (event->button() == Qt::MouseButton::RightButton) {
        if (selectedPoint < 0) showZoomArea = true;
        update();
    }
}

void ImageOpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
    QOpenGLWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::MouseButton::LeftButton) {
        if (showPointSeries) {
            if (!singlePointsOnly) {
                TrackPoint::PointInCamera* newPoint = new TrackPoint::PointInCamera(mousePosInImage.x(), mousePosInImage.y());
                if (showPointSeriesString) {
                    newPoint->string = createPointLabelDialog();
                }
                pointSeries.push_back(newPoint);
                checkPointSeries();
                if (selectedPoint >= 0) selectedPoint = getClosestPoint(mousePosInImage);
            } else {
                int mouseSubRegionX = (int) mousePosInImage.x() / (imageWidth / ((enableSubImages) ? numImageGroupsX : 1));
                int mouseSubRegionY = (int) mousePosInImage.y() / (imageHeight / ((enableSubImages) ? numImageGroupsY : 1));
                bool found = false;
                for (int i = 0; i < pointSeries.size(); i++) {
                    int pointSubRegionX = (int) pointSeries[i]->pointX / (imageWidth / ((enableSubImages) ? numImageGroupsX : 1));
                    int pointSubRegionY = (int) pointSeries[i]->pointY / (imageHeight / ((enableSubImages) ? numImageGroupsY : 1));
                    if (mouseSubRegionX == pointSubRegionX && mouseSubRegionY == pointSubRegionY) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    TrackPoint::PointInCamera* newPoint = new TrackPoint::PointInCamera(mousePosInImage.x(), mousePosInImage.y());
                    if (showPointSeriesString) {
                        newPoint->string = createPointLabelDialog();
                    }
                    pointSeries.push_back(newPoint);
                    checkPointSeries();
                    if (selectedPoint >= 0) selectedPoint = getClosestPoint(mousePosInImage);
                }
            }
        }

        leftMouseButtonDown = false;
        showZoomArea = false;
        QPoint globalPos = mapToGlobal(QPoint(mousePos.x(), mousePos.y()));
        QCursor::setPos(globalPos.x(), globalPos.y());
        update();
        return;
    }
    if (event->button() == Qt::MouseButton::RightButton) {
        if (selectedPoint >= 0 && !showZoomArea) {
            pointSeries.erase(pointSeries.begin() + selectedPoint);
            checkPointSeries();
            if (selectedPoint >= 0) selectedPoint = getClosestPoint(mousePosInImage);
        }
        if (showZoomArea) {
            showZoomArea = false;
            QPoint globalPos = mapToGlobal(QPoint(mousePos.x(), mousePos.y()));
            QCursor::setPos(globalPos.x(), globalPos.y());
        }
        //removeBoundingArea(QPointF(mousePosInImage.x(), mousePosInImage.y())); //tomas 10-12-2015
        update();
    }
}

void ImageOpenGLWidget::mouseMoveEvent(QMouseEvent * event) {
    int dx = event->x() - actualMousePos.x();
    int dy = event->y() - actualMousePos.y();
    actualMousePos = event->pos();
    if (showZoomArea) {
        double sensitivity = zoomFactor * 2;
        mousePos.setX(mousePos.x() + ((double) dx) / sensitivity);
        mousePos.setY(mousePos.y() + ((double) dy) / sensitivity);
    } else {
        mousePos = actualMousePos;
    }
    mousePosInImage = mousePos - scaledImageArea.topLeft();
    mousePosInImage.setX(mousePosInImage.x() * ((double) imageWidth / scaledImageArea.width()));
    mousePosInImage.setY(mousePosInImage.y() * ((double) imageHeight / scaledImageArea.height()));

    if (leftMouseButtonDown) {
        dragBoundingArea();
    }
    checkPointSeries();
    if (selectedPoint >= 0) selectedPoint = getClosestPoint(mousePosInImage);
    checkBoundingAreas();
    QOpenGLWidget::mouseMoveEvent(event);
    update();
}

void ImageOpenGLWidget::dragBoundingArea() {
    if (selectedBoundingCircle >= 0) {
        QPointF circlePos = QPointF(boundingCircles[selectedBoundingCircle]->pos.x, boundingCircles[selectedBoundingCircle]->pos.y);
        if (!selectedBoundingCircleEdge) {
            QPointF cPos = QPointF(mousePosInImage.x(), mousePosInImage.y()) + selectedBoundingAreaMouseRelative;
            boundingCircles[selectedBoundingCircle]->pos.x = cPos.x();
            boundingCircles[selectedBoundingCircle]->pos.y = cPos.y();
        } else {
            QPointF lenVec = circlePos - QPointF(mousePosInImage.x(), mousePosInImage.y());
            double length = std::sqrt(std::pow(lenVec.x(), 2) + std::pow(lenVec.y(), 2));
            boundingCircles[selectedBoundingCircle]->radius = length;
        }
    }

    if (selectedBoundingBox >= 0) {
        if (selectedBoundingBoxCorner == 0) {
            boundingBoxes[selectedBoundingBox]->setTopLeft(QPointF(mousePosInImage.x(), mousePosInImage.y()));
        }
        if (selectedBoundingBoxCorner == 1) {
            boundingBoxes[selectedBoundingBox]->setTopRight(QPointF(mousePosInImage.x(), mousePosInImage.y()));
        }
        if (selectedBoundingBoxCorner == 2) {
            boundingBoxes[selectedBoundingBox]->setBottomLeft(QPointF(mousePosInImage.x(), mousePosInImage.y()));
        }
        if (selectedBoundingBoxCorner == 3) {
            boundingBoxes[selectedBoundingBox]->setBottomRight(QPointF(mousePosInImage.x(), mousePosInImage.y()));
        }
        if (selectedBoundingBoxCorner < 0) {
            boundingBoxes[selectedBoundingBox]->moveCenter(QPointF(mousePosInImage.x(), mousePosInImage.y()) + selectedBoundingAreaMouseRelative);
        }
    }
}

void ImageOpenGLWidget::removeBoundingArea(QPointF& mousePos) {
    for (int i = 0; i < boundingCircles.size(); i++) {
        QPointF relPos = QPointF(boundingCircles[i]->pos.x, boundingCircles[i]->pos.y) - mousePos;
        double distance = std::sqrt(std::pow(relPos.x(), 2) + std::pow(relPos.y(), 2));
        if (distance < boundingCircles[i]->radius) {
            boundingCircles.erase(boundingCircles.begin() + i);
        }
    }

    for (int i = 0; i < boundingBoxes.size(); i++) {
        if (boundingBoxes[i]->contains(mousePos)) {
            boundingBoxes.erase(boundingBoxes.begin() + i);
        }
    }
}

void ImageOpenGLWidget::checkBoundingAreas() {
    bool found = false;
    double boundingCircleThreshCorr = (boundingCircleEdgeThreshold * ((double) imageWidth / scaledImageArea.width()));
    for (int i = 0; i < boundingCircles.size(); i++) {
        QPointF relPos = QPointF(boundingCircles[i]->pos.x, boundingCircles[i]->pos.y) - mousePosInImage;
        double distance = std::sqrt(std::pow(relPos.x(), 2) + std::pow(relPos.y(), 2));
        if (distance < boundingCircles[i]->radius + boundingCircleThreshCorr) {
            selectedBoundingCircle = i;
            selectedBoundingAreaMouseRelative = QPointF(boundingCircles[i]->pos.x, boundingCircles[i]->pos.y) - QPointF(mousePosInImage.x(), mousePosInImage.y());
            found = true;
        }
        if (found && distance > boundingCircles[i]->radius - boundingCircleThreshCorr) {
            selectedBoundingCircleEdge = true;
            break;
        } else if (!leftMouseButtonDown) selectedBoundingCircleEdge = false;
    }
    if (!found) selectedBoundingCircle = -1;

    QRectF checkBox(0, 0, boundingBoxCornerThreshold * ((double) imageWidth / scaledImageArea.width()), boundingBoxCornerThreshold * ((double) imageWidth / scaledImageArea.width()));
    for (int i = 0; i < boundingBoxes.size(); i++) {
        if (!leftMouseButtonDown) {
            checkBox.moveCenter(boundingBoxes[i]->topLeft());
            if (checkBox.contains(QPointF(mousePosInImage.x(), mousePosInImage.y()))) {
                selectedBoundingBox = i;
                selectedBoundingBoxCorner = 0;
                break;
            }
            checkBox.moveCenter(boundingBoxes[i]->topRight());
            if (checkBox.contains(QPointF(mousePosInImage.x(), mousePosInImage.y()))) {
                selectedBoundingBox = i;
                selectedBoundingBoxCorner = 1;
                break;
            }
            checkBox.moveCenter(boundingBoxes[i]->bottomLeft());
            if (checkBox.contains(QPointF(mousePosInImage.x(), mousePosInImage.y()))) {
                selectedBoundingBox = i;
                selectedBoundingBoxCorner = 2;
                break;
            }
            checkBox.moveCenter(boundingBoxes[i]->bottomRight());
            if (checkBox.contains(QPointF(mousePosInImage.x(), mousePosInImage.y()))) {
                selectedBoundingBox = i;
                selectedBoundingBoxCorner = 3;
                break;
            }
            selectedBoundingBoxCorner = -1;
        }
        if (boundingBoxes[i]->contains(QPointF(mousePosInImage.x(), mousePosInImage.y()))) {
            selectedBoundingAreaMouseRelative = boundingBoxes[i]->center() - QPointF(mousePosInImage.x(), mousePosInImage.y());
            selectedBoundingBox = i;
        } else selectedBoundingBox = -1;
    }
}

void ImageOpenGLWidget::checkPointSeries() {
    QPointF mPos = mousePos - scaledImageArea.topLeft();
    for (int i = 0; i < pointSeries.size(); i++) {
        TrackPoint::PointInCamera* point = pointSeries[i];
        double xPos = point->pointX * imageToScreenCoordX;
        double yPos = point->pointY * imageToScreenCoordY;
        bool insideXLine = false;
       
        if (mPos.x() < xPos + selectedPointThreshold && mPos.x() > xPos - selectedPointThreshold) {
            if (mPos.y() < yPos + pointCrossWingSize + selectedPointThreshold && mPos.y() > yPos - pointCrossWingSize - selectedPointThreshold) {
                selectedPoint = i;
                return;
            }
        }

        if (mPos.y() < yPos + selectedPointThreshold && mPos.y() > yPos - selectedPointThreshold) {
            if (mPos.x() < xPos + pointCrossWingSize + selectedPointThreshold && mPos.x() > xPos - pointCrossWingSize - selectedPointThreshold) {
                selectedPoint = i;
                return;
            }
        }
    }
    selectedPoint = -1;
}

int ImageOpenGLWidget::getClosestPoint(const QPointF& pos) {
    double dist = 1e10;
    int index;
    for (int i = 0; i < pointSeries.size(); i++) {
        QPointF pointPos(pointSeries[i]->pointX, pointSeries[i]->pointY);
        pointPos -= pos;
        double newDist = std::sqrt(std::pow(pointPos.x(), 2) + std::pow(pointPos.y(), 2));
        if (newDist < dist) {
            dist = newDist;
            index = i;
        }
    }
    return index;
}

QString ImageOpenGLWidget::createPointLabelDialog() {
    QString res = QInputDialog::getText(this, "Create Point Label", "Write label for this point:");
    lastPointString = res;
    return res;
}

