
#include "calibrationfile.h"
#include "calibrationfileopenglwidget.h"

CalibrationFileOpenGLWidget::CalibrationFileOpenGLWidget(CalibrationFile* calibFile) : QOpenGLWidget() {
    bounding.setLeft(3500);
    this->calibFile = calibFile;
}

void CalibrationFileOpenGLWidget::initializeGL() {
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-bounding.left() * ((double) width() / height()), bounding.left() * ((double) width() / height()), -bounding.left(), bounding.left(), -1000000, 1000000);
}

void CalibrationFileOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-bounding.left() * ((double) width() / height()), bounding.left() * ((double) width() / height()), -bounding.left(), bounding.left(), -1000000, 1000000);
}

void CalibrationFileOpenGLWidget::setSelectedCameraCombination(TrackPoint::CameraCombination* selectedCamComb) {
    this->selectedCameraCombination = selectedCamComb;
}