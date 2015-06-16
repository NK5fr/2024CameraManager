
#include "calibrationfile.h"
#include "calibrationfileopenglwidget.h"

CalibrationFileOpenGLWidget::CalibrationFileOpenGLWidget(CalibrationFile* calibFile) : QOpenGLWidget() {
    //bounding.setLeft(3500);
    this->calibFile = calibFile;
    updateCameraArea();
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0 / 255.0, 18.0 / 255.0, 53.0 / 255.0, 1);
    //glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    const double gridSize = 500;
    // X Axis
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(gridSize, 0, 0);
    glEnd();

    // Y Axis
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, gridSize, 0);
    glEnd();

    // Z Axis
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, gridSize);
    glEnd();

    glColor3f(1, 0, 0);
    glPointSize(4);
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
            glPointSize(8);
            glColor3f(1, 1, 0);
            glBegin(GL_POINTS);
            for (int j = 0; j < 3; j++) {
                glVertex3d(selectedCameraCombination->cameras[j]->camPos.x, selectedCameraCombination->cameras[j]->camPos.y, selectedCameraCombination->cameras[j]->camPos.z);
            }
            glEnd();
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
        screenArea.setTop(((double) ((height * abs(calibrationCameraArea.width())) / width) / abs(calibrationCameraArea.width())) * (double) abs(calibrationCameraArea.top()));
        screenArea.setBottom(-((double) ((height * abs(calibrationCameraArea.width())) / width) / abs(calibrationCameraArea.width())) * (double) abs(calibrationCameraArea.bottom()));
    }
    int bottom = screenArea.bottom();
    int top = screenArea.top();
    int left = screenArea.left();
    int right = screenArea.right();
    glOrtho(screenArea.left(), screenArea.right(), screenArea.bottom(), screenArea.top(), -1000000, 1000000);
}

void CalibrationFileOpenGLWidget::setSelectedCameraCombination(TrackPoint::CameraCombination* selectedCamComb) {
    this->selectedCameraCombination = selectedCamComb;
}

void CalibrationFileOpenGLWidget::updateCameraArea() {
    vector<TrackPoint::CameraCombination*> camComb = calibFile->getCameraCombinations();
    for (int i = 0; i < camComb.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (camComb[i]->cameras[j] != nullptr) {
                TrackPoint::Camera* cam = camComb[i]->cameras[j];
                if (cam->camPos.x < this->calibrationCameraArea.left()) this->calibrationCameraArea.setLeft(cam->camPos.x);
                if (cam->camPos.x > this->calibrationCameraArea.right()) this->calibrationCameraArea.setRight(cam->camPos.x);

                if (cam->camPos.y < this->calibrationCameraArea.bottom()) this->calibrationCameraArea.setBottom(cam->camPos.y);
                if (cam->camPos.y > this->calibrationCameraArea.top()) this->calibrationCameraArea.setTop(cam->camPos.y);
            }
        }
    }
    const float margin = 200;

    calibrationCameraArea.setLeft(calibrationCameraArea.left() - margin);
    calibrationCameraArea.setRight(calibrationCameraArea.right() + margin);
    calibrationCameraArea.setTop(calibrationCameraArea.top() + margin);
    calibrationCameraArea.setBottom(calibrationCameraArea.bottom() - margin);
}