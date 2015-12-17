
#include "calibrationfile.h"
#include "calibrationfileopenglwidget.h"

CalibrationFileOpenGLWidget::CalibrationFileOpenGLWidget(CalibrationFile* calibFile) : QOpenGLWidget() {
    setMouseTracking(true);
    this->calibFile = calibFile;
    initCameraArea();
    adjustCameraAreaForCoordinateOrigin();
    addCameraAreaMargin(200);
    installEventFilter(this);
}

void CalibrationFileOpenGLWidget::initializeGL() {
    resizeGL(width(), height());
}

void CalibrationFileOpenGLWidget::paintGL() {
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0 / 255.0, 18.0 / 255.0, 53.0 / 255.0, 1);
    //glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const bool showGridLines = true;
    if (showGridLines) {
        const QColor meterGridColor(255, 255, 255, 75);
        const QColor decimeterGridColor(255, 255, 255, 50);
        const QColor centimeterGridColor(255, 255, 255, 40);
        const QColor millimeterGridColor(255, 255, 255, 30);
        createCoordinateGrid(screenArea, 1000, meterGridColor);
        QString scaleString = "meter";
        const int probedPixelWidth = 100; // Check if different scales are visible within this pixelWidth
        if (abs((screenArea.width() / width()) * probedPixelWidth) < 1000) { // if pixelwidth corresponds to a width of less than 1000 mm
            createCoordinateGrid(screenArea, 100, decimeterGridColor);
            scaleString = "decimeter";
        }
        if (abs((screenArea.width() / width()) * probedPixelWidth) < 100) { // if pixelwidth corresponds to a width of less than 100 mm
            createCoordinateGrid(screenArea, 10, centimeterGridColor);
            scaleString = "centimeter";
        }
        if (abs((screenArea.width() / width()) * probedPixelWidth) < 10) { // if pixelwidth corresponds to a width of less than 10 mm
            createCoordinateGrid(screenArea, 1, millimeterGridColor);
            scaleString = "millimeter";
        }
        QPainter painter(this);
        QPoint pos(4, 4);
        painter.fillRect(pos.x(), pos.y(), 60 + (scaleString.size() * 5), 13, Qt::white);
        painter.drawText(pos + QPoint(3, 10), "Gridscale: " + scaleString);
        if (enteredCameraWindow) {
            pos.setY(pos.y() + 14);
            painter.fillRect(pos.x(), pos.y(), 110, 13, Qt::white);
            painter.drawText(pos + QPoint(3, 10), "Press 'ESC' to escape");
        }
    }

    updateMouseOverCameraArea();
    if (mouseOverCameraArea >= 0 && !enteredCameraWindow && mouseInside) {
        glColor4f(1, 1, 0, 1);
        glBegin(GL_POLYGON);
        glVertex3d(cameraArea[mouseOverCameraArea].left() - cameraAreaMargin, cameraArea[mouseOverCameraArea].bottom() - cameraAreaMargin, 1);
        glVertex3d(cameraArea[mouseOverCameraArea].right() + cameraAreaMargin, cameraArea[mouseOverCameraArea].bottom() - cameraAreaMargin, 1);
        glVertex3d(cameraArea[mouseOverCameraArea].right() + cameraAreaMargin, cameraArea[mouseOverCameraArea].top() + cameraAreaMargin, 1);
        glVertex3d(cameraArea[mouseOverCameraArea].left() - cameraAreaMargin, cameraArea[mouseOverCameraArea].top() + cameraAreaMargin, 1);
        glEnd();
    }


    glLineWidth(2);
    // X Axis
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 1);
    glVertex3f(gridSize, 0, 1);
    glEnd();

    // Y Axis
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 1);
    glVertex3f(0, gridSize, 1);
    glEnd();
    glLineWidth(1);

    glColor3f(1, 0, 0);
    double pointSize = 5;
    glPointSize(max(1.0, pointSize));
    vector<TrackPoint::CameraCombination*> camComb = calibFile->getCameraCombinations();
    for (int i = 0; i < camComb.size(); i++) {
        if (camComb[i]->cameras[0] != nullptr && camComb[i] != selectedCameraCombination) {
            glBegin(GL_POINTS);
            for (int j = 0; j < 3; j++) glVertex3d(camComb[i]->cameras[j]->camPos.x, camComb[i]->cameras[j]->camPos.y, camComb[i]->cameras[j]->camPos.z);
            glEnd();
        }
    }
    if (selectedCameraCombination != nullptr) {
        if (selectedCameraCombination->cameras[0] != nullptr) {
            glPointSize(max(1.0, pointSize * 2));
            glColor3f(1, 1, 0);
            glBegin(GL_POINTS);
            for (int j = 0; j < 3; j++) {
                glVertex3d(selectedCameraCombination->cameras[j]->camPos.x, selectedCameraCombination->cameras[j]->camPos.y, selectedCameraCombination->cameras[j]->camPos.z);
            }
            glEnd();
        }
    }
    if (selectedCamera >= 0) {
        glPointSize(max(1.0, pointSize * 2));
        glColor3f(1, 1, 0);
        glBegin(GL_POINTS);
        for (int i = 0; i < camComb.size(); i++) {
            for (int j = 0; j < 3; j++) {
                if (camComb[i]->cameras[j] == nullptr) continue;
                if (selectedCamera == camComb[i]->cameras[j]->camNo) {
                    TrackPoint::Camera* cam = camComb[i]->cameras[j];
                    glVertex3d(cam->camPos.x, cam->camPos.y, cam->camPos.z);
                }
            }
        }
        glEnd();
    }

    vector<TrackPoint::Camera*> cams = calibFile->getCameras();
    float screenFactorX = screenFactorX = (float) width() / screenArea.width();
    float screenFactorY = screenFactorY = (float) height() / screenArea.height();
    int leftBottomOffsetX = screenArea.left() * screenFactorX;
    int leftBottomOffsetY = screenArea.top() * screenFactorY;
    for (int i = 0; i < cams.size(); i++) {
        glColor3f(0, 1, 0);
        glBegin(GL_LINE_LOOP);
        glVertex2d(cameraArea[i].left() - cameraAreaMargin, cameraArea[i].bottom() - cameraAreaMargin);
        glVertex2d(cameraArea[i].right() + cameraAreaMargin, cameraArea[i].bottom() - cameraAreaMargin);
        glVertex2d(cameraArea[i].right() + cameraAreaMargin, cameraArea[i].top() + cameraAreaMargin);
        glVertex2d(cameraArea[i].left() - cameraAreaMargin, cameraArea[i].top() + cameraAreaMargin);
        glEnd();

        QPainter painter(this);
        float camX = (cameraArea[i].right() + cameraAreaMargin) * screenFactorX;
        float camY = (cameraArea[i].top() + cameraAreaMargin) * screenFactorY;
        QPointF pos = QPointF(camX + abs(leftBottomOffsetX), camY + abs(leftBottomOffsetY));
        painter.fillRect(pos.x(), pos.y(), (i > 9) ? 18 : 12, 13, Qt::white);
        painter.drawText(pos + QPoint(3, 10), QString::number(i));
    }
    if (enteredCameraWindow) {
        QPainter painter(this);
        QPointF pos = QPointF(((cameraArea[enteredCameraArea].left() - cameraAreaMargin) * screenFactorX) - leftBottomOffsetX, ((cameraArea[enteredCameraArea].top() + cameraAreaMargin) * screenFactorY) - leftBottomOffsetY);
        painter.fillRect(pos.x(), pos.y(), 12, 13, Qt::white);
        painter.drawText(pos + QPoint(3, 10), QString::number(enteredCameraArea));
        for (int i = 0; i < camComb.size(); i++) {
            for (int j = 0; j < 3; j++) {
                if (camComb[i]->cameras[j] == nullptr) continue;
                //if (j != mouseOverCameraArea) continue;
                float camX = camComb[i]->cameras[j]->camPos.x * screenFactorX;
                float camY = camComb[i]->cameras[j]->camPos.y * screenFactorY;
                //QPointF pos = QPointF(camX, camY);
                pos = QPointF(camX - leftBottomOffsetX, camY - leftBottomOffsetY);
                painter.fillRect(pos.x(), pos.y(), 36, 13, Qt::white);
                painter.drawText(pos + QPoint(3, 10), QString::number(camComb[i]->camNumbers[0]) + "_" + QString::number(camComb[i]->camNumbers[1]) + "_" + QString::number(camComb[i]->camNumbers[2]));
            }
        }
    }
}

void CalibrationFileOpenGLWidget::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
    int width = w;
    int height = h;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    if (enteredCameraArea >= 0) {
        keepAspectRatio(screenArea, cameraArea[enteredCameraArea], w, h, cameraAreaMargin);
        glOrtho(screenArea.left(), screenArea.right(), screenArea.bottom(), screenArea.top(), -1000000, 1000000);
    } else {
        keepAspectRatio(screenArea, calibrationCameraArea, w, h);
        float scaleSize = 0;
        glOrtho(screenArea.left() + abs(screenArea.width() * scaleSize), screenArea.right() + abs(screenArea.width() * scaleSize), screenArea.bottom() + abs(screenArea.height() * scaleSize), screenArea.top() + abs(screenArea.height() * scaleSize), -1000000, 1000000);
    }
}

void CalibrationFileOpenGLWidget::setSelectedCameraCombination(TrackPoint::CameraCombination* selectedCamComb) {
    this->selectedCameraCombination = selectedCamComb;
}

void CalibrationFileOpenGLWidget::setSelectedCamera(int selectedCam) {
    this->selectedCamera = selectedCam;
}

void CalibrationFileOpenGLWidget::initCameraArea() {
    vector<TrackPoint::CameraCombination*> camComb = calibFile->getCameraCombinations();
    vector<TrackPoint::Camera*> cams = calibFile->getCameras();
    cameraArea.resize(cams.size());
    const float max = 10000000; // 10 km
    for (int i = 0; i < cams.size(); i++) {
        cameraArea[i].setLeft(max);
        cameraArea[i].setRight(-max);
        cameraArea[i].setBottom(max);
        cameraArea[i].setTop(-max);
    }

    for (int i = 0; i < camComb.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (camComb[i]->cameras[j] != nullptr) {
                TrackPoint::Camera* cam = camComb[i]->cameras[j];
                if (cam->camPos.x < this->calibrationCameraArea.left()) this->calibrationCameraArea.setLeft(cam->camPos.x);
                if (cam->camPos.x > this->calibrationCameraArea.right()) this->calibrationCameraArea.setRight(cam->camPos.x);

                if (cam->camPos.y < this->calibrationCameraArea.bottom()) this->calibrationCameraArea.setBottom(cam->camPos.y);
                if (cam->camPos.y > this->calibrationCameraArea.top()) this->calibrationCameraArea.setTop(cam->camPos.y);

                if (cameraArea[cam->camNo].left() > cam->camPos.x) cameraArea[cam->camNo].setLeft(cam->camPos.x);
                if (cameraArea[cam->camNo].right() < cam->camPos.x) cameraArea[cam->camNo].setRight(cam->camPos.x);
                if (cameraArea[cam->camNo].top() < cam->camPos.y) cameraArea[cam->camNo].setTop(cam->camPos.y);
                if (cameraArea[cam->camNo].bottom() > cam->camPos.y) cameraArea[cam->camNo].setBottom(cam->camPos.y);
            }
        }
    }
}

void CalibrationFileOpenGLWidget::addCameraAreaMargin(double margin) {
    calibrationCameraArea.setLeft(calibrationCameraArea.left() - margin);
    calibrationCameraArea.setRight(calibrationCameraArea.right() + margin);
    calibrationCameraArea.setTop(calibrationCameraArea.top() + margin);
    calibrationCameraArea.setBottom(calibrationCameraArea.bottom() - margin);
}

void CalibrationFileOpenGLWidget::adjustCameraAreaForCoordinateOrigin() {
    calibrationCameraArea.setLeft(min(calibrationCameraArea.left(), 0.0));
    calibrationCameraArea.setRight(max(calibrationCameraArea.right(), 0.0));
    calibrationCameraArea.setTop(max(calibrationCameraArea.top(), 0.0));
    calibrationCameraArea.setBottom(min(calibrationCameraArea.bottom(), 0.0));
}

void CalibrationFileOpenGLWidget::createCoordinateGrid(QRectF area, double gridSize, const QColor& color) {
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    for (double i = 0; i < abs(area.left()); i += gridSize) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(((area.left() > 0) ? 1 : -1) * i, area.top());
        glVertex2d(((area.left() > 0) ? 1 : -1) * i, area.bottom());
        glEnd();
    }

    for (double i = gridSize; i < abs(area.right()); i += gridSize) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(((area.right() > 0) ? 1 : -1) * i, area.top());
        glVertex2d(((area.right() > 0) ? 1 : -1) * i, area.bottom());
        glEnd();
    }

    for (double i = 0; i < abs(area.top()); i += gridSize) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(area.left(), ((area.top() > 0) ? 1 : -1) * i);
        glVertex2d(area.right(), ((area.top() > 0) ? 1 : -1) * i);
        glEnd();
    }

    for (double i = gridSize; i < abs(area.bottom()); i += gridSize) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(area.left(), ((area.bottom() > 0) ? 1 : -1) * i);
        glVertex2d(area.right(), ((area.bottom() > 0) ? 1 : -1) * i);
        glEnd();
    }
    glColor4f(1, 1, 1, 1);
}

void CalibrationFileOpenGLWidget::updateMouseOverCameraArea() {
    if (!mouseInside) {
        mouseOverCameraArea = -1;
        return;
    }
    for (int i = 0; i < cameraArea.size(); i++) {
        QRectF rect = cameraArea[i];
        if (rect.left() - cameraAreaMargin < mouseOpenGLPos.x() && rect.right() + cameraAreaMargin > mouseOpenGLPos.x() && rect.top() + cameraAreaMargin > mouseOpenGLPos.y() && rect.bottom() - cameraAreaMargin < mouseOpenGLPos.y()) {
            mouseOverCameraArea = i;
            return;
        }
    }
    mouseOverCameraArea = -1;
}

void CalibrationFileOpenGLWidget::keepAspectRatio(QRectF& topWindowRect, QRectF windowRect, int width, int height, double margin) {
    double screenRatio = ((double) width / height);
    double areaRatio = ((double) (abs(windowRect.width()) + (margin * 2.0)) / (abs(windowRect.height()) + (margin * 2.0)));
    if (screenRatio > areaRatio) {
        topWindowRect.setLeft(-((double) ((width * abs(windowRect.height())) / height) / abs(windowRect.width())) * (double) abs(windowRect.width()) / 2 - (margin * screenRatio));
        topWindowRect.setRight(((double) ((width * abs(windowRect.height())) / height) / abs(windowRect.width())) * (double) abs(windowRect.width()) / 2 + (margin * screenRatio));
        topWindowRect.setTop(windowRect.top() + margin);
        topWindowRect.setBottom(windowRect.bottom() - margin);
    } else {
        topWindowRect.setLeft(windowRect.left() - margin);
        topWindowRect.setRight(windowRect.right() + margin);
        topWindowRect.setTop(((double) ((height * abs(windowRect.width())) / width) / abs(windowRect.height())) * (double) abs(windowRect.height()) / 2 + (margin * ((double) height / width)));
        topWindowRect.setBottom(-((double) ((height * abs(windowRect.width())) / width) / abs(windowRect.height())) * (double) abs(windowRect.height()) / 2 - (margin * ((double) height / width)));
    }
    QPointF centerCameraArea = windowRect.center();
    QPointF centerScreenArea = topWindowRect.center();
    topWindowRect.translate((centerCameraArea - centerScreenArea));
}

void CalibrationFileOpenGLWidget::enterEvent(QEvent* event) {
    mouseInside = true;
}

void CalibrationFileOpenGLWidget::leaveEvent(QEvent* event) {
    mouseInside = false;
}

void CalibrationFileOpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
    mousePos = event->localPos();
    mouseOpenGLPos.setX(mousePos.x() * (screenArea.width() / width()) + screenArea.left());
    mouseOpenGLPos.setY(mousePos.y() * (screenArea.height() / height()) + screenArea.top());
    update();
}

void CalibrationFileOpenGLWidget::mousePressEvent(QMouseEvent* event) {
    setFocus();
}

void CalibrationFileOpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (mouseOverCameraArea >= 0) {
        enteredCameraArea = mouseOverCameraArea;
        makeCurrent();
        resizeGL(width(), height());
        enteredCameraWindow = true;
        update();
    } else {
        enteredCameraArea = -1;
        makeCurrent();
        resizeGL(width(), height());
        enteredCameraWindow = false;
        update();
    }
}

bool CalibrationFileOpenGLWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            enteredCameraArea = -1;
            mouseOverCameraArea = -1;
            makeCurrent();
            resizeGL(width(), height());
            enteredCameraWindow = false;
            update();
        }
    }
    return QObject::eventFilter(obj, event);
}