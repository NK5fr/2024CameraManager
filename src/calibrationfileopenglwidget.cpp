
#include "calibrationfile.h"
#include "calibrationfileopenglwidget.h"

CalibrationFileOpenGLWidget::CalibrationFileOpenGLWidget(CalibrationFile* calibFile) : QOpenGLWidget() {
    //bounding.setLeft(3500);
    this->calibFile = calibFile;
    initCameraArea();
    adjustCameraAreaForCoordinateOrigin();
    addCameraAreaMargin(200);
}

void CalibrationFileOpenGLWidget::initializeGL() {
    resizeGL(width(), height());
    /*glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-bounding.left() * ((double) width() / height()), bounding.left() * ((double) width() / height()), -bounding.left(), bounding.left(), -1000000, 1000000);
    */
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
    
    const QColor meterGridColor(255, 255, 255, 75);
    const QColor decimeterGridColor(255, 255, 255, 50);
    createCoordinateGrid(screenArea, 1000, meterGridColor);
    /*if (screenArea.width() / width() < 10)*/ createCoordinateGrid(screenArea, 100, decimeterGridColor);

    const double gridSize = 500;
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
    const double factor = 400;
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
            QPainter painter(this);
            //QPoint pos(2, yPos);
            //painter.fillRect(pos.x(), pos.y(), 105, 12, Qt::white);
            //painter.drawText(pos + QPoint(2, 10), "X: " + QString::number(points[i].x, 'f', 2) + " ,Y: " + QString::number(points[i].y, 'f', 2));
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
    const float cameraAreaMargin = 100; // 10 cm
    float screenFactorAbs = abs((float) width() / screenArea.width());
    float screenFactorX = (float) width() / screenArea.width();
    float screenFactorY = (float) height() / screenArea.height();
    int centerOffsetX = (screenArea.center().x() * screenFactorX);
    int centerOffsetY = (screenArea.center().y() * screenFactorY);
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
}

void CalibrationFileOpenGLWidget::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
    int width = w;
    int height = h;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    QPointF centerCameraArea = calibrationCameraArea.center();

    double screenRatio = ((double) width / height);
    double areaRatio = ((double) abs(calibrationCameraArea.width()) / abs(calibrationCameraArea.height()));
    if (screenRatio > areaRatio) {
        screenArea.setRight(((double) ((width * abs(calibrationCameraArea.height())) / height) / abs(calibrationCameraArea.width())) * (double) abs(calibrationCameraArea.width()) / 2);
        double t = ((double) ((width * abs(calibrationCameraArea.height())) / height) / abs(calibrationCameraArea.width())) * (double) abs(calibrationCameraArea.width()) / 2;
        screenArea.setLeft(-((double) ((width * abs(calibrationCameraArea.height())) / height) / abs(calibrationCameraArea.width())) * (double) abs(calibrationCameraArea.width()) / 2);
        screenArea.setTop(calibrationCameraArea.top());
        screenArea.setBottom(calibrationCameraArea.bottom());
    } else {
        screenArea.setRight(calibrationCameraArea.right());
        screenArea.setLeft(calibrationCameraArea.left());
        double t = ((double) ((height * abs(calibrationCameraArea.width())) / width) / abs(calibrationCameraArea.width())) * (double) abs(calibrationCameraArea.top());
        screenArea.setTop(((double) ((height * abs(calibrationCameraArea.width())) / width) / abs(calibrationCameraArea.height())) * (double) abs(calibrationCameraArea.height()) / 2);
        screenArea.setBottom(-((double) ((height * abs(calibrationCameraArea.width())) / width) / abs(calibrationCameraArea.height())) * (double) abs(calibrationCameraArea.height()) / 2);
    }

    QPointF centerScreenArea = screenArea.center();
    screenArea.translate((centerCameraArea - centerScreenArea));
    int bottom = screenArea.bottom();
    int top = screenArea.top();
    int left = screenArea.left();
    int right = screenArea.right();
    glOrtho(screenArea.left(), screenArea.right(), screenArea.bottom(), screenArea.top(), -1000000, 1000000);
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