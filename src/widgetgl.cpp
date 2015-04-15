#include "widgetgl.h"

using namespace std;

static const double PI = 3.1415926535;

WidgetGL::WidgetGL(vector<vector<Vector3d>> *points, vector<float> minmax, SocketViewerWidget* socket, QString calibrationPath) : QGLWidget(socket), pointData(*points), initialScale(false), keyPressed(0), svw(socket) {
    coordinatesShown = 0;
    qtBlack = QColor::fromCmykF(0.0, 0.0, 0.0, 0.25);
    calibFile = new CalibrationFile(calibrationPath);

    initializingCameras();
    initializeGL();
    
    /* Initializing the selected array */
    if (pointData.size() > 0) {
        for (int i = 0; i < pointData[0].size(); i++) {
            selected.insert(i, false);
        }
    }

    showFovCone = true;
    showPreceedingPoints = false;
    showLines = false;
    showCameras = false;
    showCoordinateSystem = true;
    showFloorLines = true; 
    showCameraLegs = false;
    showOrtho = false;

    rotX = 0;
    rotY = 0;
    camDistance = 4000; 
    coneSize = 500;
    setMouseTracking(true);
    grabKeyboard();
    installEventFilter(this);
    updateGL();
}

WidgetGL::~WidgetGL() {
}

void WidgetGL::initializeGL() {
    int height = this->height();
    int width = this->width();
    if (height == 0) height = 1;
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho()
    
    if (showOrtho) {
        //gluOrtho2D(-width * scale, width * scale, -height * scale, height * scale);
    } else {
        GLfloat aspect = (GLfloat) width / (GLfloat) height;
        const double PI = 3.1415926535;
        double zNear = 1;
        double zFar = 160000;
        double fov = 60;
        gluPerspective(fov, (GLfloat) width / (GLfloat) height, zNear, zFar);
    }
}

void WidgetGL::initializingCameras() {
    bool found = false;
    for (int i = 0; i < calibFile->getNumCameras(); i++) {
        found = false;
        for (int j = 0; j < calibFile->getCameraCombinations().size(); j++) {
            if (!calibFile->getCameraCombinations()[j]->valid) continue;
            for (int k = 0; k < 3; k++) {
                if (calibFile->getCameraCombinations()[j]->cameras[k]->camNo == i) {
                    TrackPoint::Camera* cam = calibFile->getCameraCombinations()[j]->cameras[k];
                    camerasData.push_back(cam);
                    printf("CamNo: %u\nXrot: %.2f\tYrot: %.2f\tZrot: %.2f\n\n", cam->camNo, cam->orient.alpha * (180 / PI), cam->orient.beta * (180 / PI), cam->orient.kappa * (180 / PI));
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
    }
}

void WidgetGL::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (showOrtho) {        
        double scale = camDistance / 1000;
        glOrtho(-width() * scale, width() * scale, -height() * scale, height() * scale, -1000000, 1000000);
        gluLookAt(cos(rotY) * sin(rotX), cos(rotY) * cos(rotX), sin(rotY), 0, 0, 0, 0, 0, 1);
    } else {
        gluLookAt(cos(rotY) * sin(rotX) * camDistance, cos(rotY) * cos(rotX) * camDistance, sin(rotY) * camDistance, 0, 0, 0, 0, 0, 1);
    }
    
    glPointSize(4.0);
    glColor3f(1, 0, 0);
    glScaled(1, 1, 1);

    // Drawing lines connecting the preceeding point up to the selected timeframe
    if (showLines && pointData.size() > 0) {
        glColor3f(0.75f, 0.75f, 0.75f);
        for (int point = 0; point < (pointData[0]).size(); point++){
            glBegin(GL_LINE_STRIP);
            for (int time = 0; time <= coordinatesShown; time++) {
                glVertex3f(GLfloat(pointData[time][point].x),
                           GLfloat(pointData[time][point].z),
                           GLfloat(pointData[time][point].y));
            }
            glEnd();
        }
    }

    glColor3f(1, 0, 0);
    // Drawing the preceeding points to the selected timeframe
    if (showPreceedingPoints && pointData.size() > 0) {
        for (int time = 0; time < coordinatesShown; time++) {
            for (int point = 0; point < (pointData[time]).size(); point++){
                glBegin(GL_POINTS);
                glVertex3f(GLfloat(pointData[time][point].x),
                           GLfloat(pointData[time][point].z),
                           GLfloat(pointData[time][point].y));
                glEnd();
            }
        }
    }

    // Drawing the actual points in the selected timeframe
    if (pointData.size() > 0) {
        for (int point = 0; point < (pointData[coordinatesShown]).size(); point++){
            glBegin(GL_POINTS);
            glVertex3f(GLfloat(pointData[coordinatesShown][point].x),
                        GLfloat(pointData[coordinatesShown][point].z),
                        GLfloat(pointData[coordinatesShown][point].y));
            glEnd();
        }
    }    

    // Drawing the cameras
    if (showCameras) {
        for (int i = 0; i < camerasData.size(); i++){       
            glPushMatrix();
            glColor3f(0.9, 0, 0.9);
            glPointSize(8.0);
            glTranslatef(camerasData[i]->camPos.x, camerasData[i]->camPos.y, camerasData[i]->camPos.z);
            glBegin(GL_POINTS);
            glVertex3f(0, 0, 0);
            glEnd();
            glRotated(camerasData[i]->orient.kappa  * (180 / PI), 0, 0, 1);
            //glRotated(90, 1, 0, 0);
            //glRotated(camerasData[i]->orient.alpha  * (180 / PI), 1, 0, 0);
            //glRotated(camerasData[i]->orient.beta   * (180 / PI), 0, 1, 0);
            //glRotated(camerasData[i]->orient.kappa  * (180 / PI), 0, 0, 1);

            // Render tripod
            if (showCameraLegs) {
                const double tripodAngle = 20; // in degrees
                const int numLegs = 3;
                glColor3f(0, 0.75, 0);
                for (int leg = 0; leg < numLegs; leg++) {
                    glBegin(GL_LINES);
                    glVertex3d(0, 0, 0);
                    glVertex3d(0, (tan(tripodAngle / (180 / PI)) * camerasData[i]->camPos.z), -camerasData[i]->camPos.z);
                    glEnd();
                    glRotated(360 / numLegs, 0, 0, 1);
                }
            }

            // Render FOV-pyramid
            if (showFovCone) {
                // Converting the FOV's to degrees from radians
                double fovW = (camerasData[i]->fovWidth * (180.0 / PI));
                double fovH = (camerasData[i]->fovHeight * (180.0 / PI));
                
                // Calculating the distance from lens to the bottom of the square, and the distance from the center to the sides
                double l = cos((camerasData[i]->fovHeight / 2)) * cos((camerasData[i]->fovWidth / 2)) * coneSize;
                double w2 = cos((camerasData[i]->fovHeight / 2)) * sin((camerasData[i]->fovWidth / 2)) * coneSize;
                double h2 = sin((camerasData[i]->fovHeight / 2)) * coneSize;

                // The color of the Pyramid
                glColor3f(0, 1, 0);

                // Drawing the bottom of the FOV-pyramid (square)
                glPushMatrix();
                glBegin(GL_LINE_LOOP);
                glVertex3f(w2, l, h2);
                glVertex3f(-w2, l, h2);
                glVertex3f(-w2, l, -h2);
                glVertex3f(w2, l, -h2);
                glEnd();
                glPopMatrix();
                
                // Drawing lines from lens to the FOV-Square
                glPushMatrix();
                glRotated((fovW / 2), 0, 0, 1);
                glRotated((fovH / 2), 1, 0, 0);
                glScaled(0, coneSize, 0);
                glBegin(GL_LINE_STRIP);
                glVertex3f(0, 0, 0);
                glVertex3f(1, 1, 1);
                glEnd();
                glPopMatrix();

                glPushMatrix();
                glRotated((fovW / 2), 0, 0, -1);
                glRotated((fovH / 2), 1, 0, 0);
                glScaled(0, coneSize, 0);
                glBegin(GL_LINE_STRIP);
                glVertex3f(0, 0, 0);
                glVertex3f(1, 1, 1);
                glEnd();
                glPopMatrix();
                
                glPushMatrix();
                glRotated((fovW / 2), 0, 0, 1);
                glRotated((fovH / 2), -1, 0, 0);
                glScaled(0, coneSize, 0);
                glBegin(GL_LINE_STRIP);
                glVertex3f(0, 0, 0);
                glVertex3f(1, 1, 1);
                glEnd();
                glPopMatrix();

                glPushMatrix();
                glRotated((fovW / 2), 0, 0, -1);
                glRotated((fovH / 2), -1, 0, 0);
                glScaled(0, coneSize, 0);
                glBegin(GL_LINE_STRIP);
                glVertex3f(0, 0, 0);
                glVertex3f(1, 1, 1);
                glEnd();
                glPopMatrix();
            }
            glPopMatrix();
        }
    }

    // Drawing the coordinate axises
    glLineWidth(1);
    if (showCoordinateSystem) {
        const double gridSize = 1000;
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
    }

    // Drawing the floor with a grid in grey
    glLineWidth(1);
    const double rotYThreshold = 1 / (180 / PI); // Threshold to determine if camera is edge on the floor, obstructing the coordinate axis
    if (showFloorLines && ((rotY > rotYThreshold || rotY < -rotYThreshold) || !showCoordinateSystem)) {
        const float color = 0.50f;
        const double maxLength = 10000;
        const double lengthDivs = 100;
        glColor3f(color, color, color);
        for (double x = -(maxLength / 2); x < (maxLength / 2); x += lengthDivs) {
            glBegin(GL_LINES);
            glVertex3d(x, -(maxLength / 2), 0);
            glVertex3d(x, maxLength / 2,    0);
            glEnd();
        }
        for (double y = -(maxLength / 2); y < (maxLength / 2); y += lengthDivs) {
            glBegin(GL_LINES);
            glVertex3d(-(maxLength / 2), y, 0);
            glVertex3d(maxLength / 2, y,    0);
            glEnd();
        }
    }
}

void WidgetGL::resizeGL(int width, int height) {
    initializeGL();
    /*
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat) width / (GLfloat) height;
    const double PI = 3.1415926535;
    double zNear = 1;
    double zFar = 160000;
    double fov = 60;
    gluPerspective(fov, (GLfloat) width / (GLfloat) height, zNear, zFar);
    */
}

void WidgetGL::showView(int viewTime) {
    coordinatesShown = viewTime;
    updateGL();
}

bool WidgetGL::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress){
        keyPressed = ((QKeyEvent *) event)->key();
        /* If Left Arrow and into range */
        if (keyPressed == Qt::Key_Left && coordinatesShown - 1 >= 0) {
            int newValue = coordinatesShown - 1;
            showView(newValue);
            svw->getTimeSlider()->setValue(newValue);
            return true;
        }
        /* If Right Arrow and into range */
        if (keyPressed == Qt::Key_Right && coordinatesShown + 1 < pointData.size()){
            int newValue = coordinatesShown + 1;
            showView(newValue);
            svw->getTimeSlider()->setValue(newValue);
            return true;
        }
        if (keyPressed == Qt::Key_Shift) {
            adjustCamDistance = true;
        }
    }
    if (event->type() == QEvent::KeyRelease){
        keyPressed = 0;
        if (((QKeyEvent *) event)->key() == Qt::Key_Shift) {
            adjustCamDistance = false;
        }
    }
    if (event->type() == QEvent::Wheel){
        QWheelEvent *eventWheel = (QWheelEvent *) event;
        int num = (eventWheel->delta() / 120);
        /* No key Pressed, then change view */
        if (keyPressed == 0) {
            if (coordinatesShown + num >= 0 && coordinatesShown + num < pointData.size()) {
                int newValue = coordinatesShown + num;
                showView(newValue);
                svw->getTimeSlider()->setValue(newValue);
            }
            return true;
        }
        const double deltaSpeed = 2;
        if (adjustCamDistance) {
            camDistance = max((double) 10, min(camDistance - (eventWheel->delta() * deltaSpeed), (double) 100000));
            updateGL();
            return true;
        }
    }
    if (event->type() == QEvent::MouseMove){
        QMouseEvent *mouseEvent = (QMouseEvent *) event;
        if (mouseEvent->buttons() & Qt::RightButton) {

        }
        if (mouseEvent->button() == Qt::LeftButton) {

        }
    }
    return false;
}

void WidgetGL::mouseMoveEvent(QMouseEvent* mouseEvent) {
    if (mouseEvent->buttons() & Qt::LeftButton) {
        if (lastMouseX < 0 || lastMouseY < 0) {
            lastMouseX = mouseEvent->screenPos().x();
            lastMouseY = mouseEvent->screenPos().y();
            return;
        }
        const double speed = 0.005;
        const double PI = 3.1415926535;
        double maxVerticalAngle = PI / 2; // 90 degrees in radians...
        rotX += (lastMouseX - mouseEvent->screenPos().x()) * speed;
        rotY = max(-maxVerticalAngle, min(rotY + ((lastMouseY - mouseEvent->screenPos().y()) * speed), maxVerticalAngle));
        updateGL();
    }
    lastMouseX = mouseEvent->screenPos().x();
    lastMouseY = mouseEvent->screenPos().y();
}

void WidgetGL::clickOnMenu(QAction *action) {
    QString number = action->text().split(QString::fromUtf8("\u00b0")).at(1);
    int nb = number.toInt();
    initializeGL();
}

void WidgetGL::showXYPlane() {
    rotX = PI;
    rotY = PI / 2;
    updateGL();
}

void WidgetGL::showXZPlane() {
    rotX = PI;
    rotY = 0;
    updateGL();
}

void WidgetGL::showYZPlane() {
    rotX = PI / 2;
    rotY = 0;
    updateGL();
}
