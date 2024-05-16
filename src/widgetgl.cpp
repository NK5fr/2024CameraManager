#include "widgetgl.h"


static const double PI = 3.1415926535;

WidgetGL::WidgetGL(SocketViewerWidget* socket, std::vector<std::vector<Vector3d*>> *points, QString calibrationPath) : QOpenGLWidget(socket), pointData(*points), initialScale(false), keyPressed(0), svw(socket) {
    coordinatesShown = 0;
    calibFile = new CalibrationFile(calibrationPath);
    initializingCameras();
    numShowedPreceedingPoints = 15;
    showFovCone = true;
    showPreceedingPoints = false;
    showLines = false;
    showCameras = false;
    showCoordinateSystem = true;
    showFloorLines = true; 
    showCameraLegs = false;
    showOrtho = false;
    showFromCamera = false; // NOT USED
    camViewIndex = 0;       // NOT USED

    rotX = 0;
    rotY = 0;
    camDistance = 4000; 
    coneSize = 500;
    setMouseTracking(true);
    installEventFilter(this);
}

WidgetGL::WidgetGL(SocketViewerWidget* socket) : QOpenGLWidget(socket), svw(socket) {
    calibFile = nullptr;
    numShowedPreceedingPoints = 30;
    coordinatesShown = 0;
    showFovCone = true;
    showPreceedingPoints = false;
    showLines = false;
    showCameras = false;
    showCoordinateSystem = true;
    showFloorLines = false;
    showCameraLegs = false;
    showOrtho = false;
    showFromCamera = false; // NOT USED
    camViewIndex = 0;       // NOT USED

    rotX = 0;
    rotY = 0;
    camDistance = 4000;
    coneSize = 500;
    setMouseTracking(true);
    installEventFilter(this);
}

WidgetGL::~WidgetGL() {
}

void WidgetGL::initializeGL() {
    int height = this->height();
    int width = this->width();
    updateProjection(width, height, 60);
}

void WidgetGL::updateProjection(int width, int height, double fov) {
    if (height == 0) height = 1;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glViewport((WidgetGL::width() - width), (WidgetGL::height() - height), (WidgetGL::width() - width) + width, (WidgetGL::height() - height) + height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (showOrtho) {
        double scale = camDistance / 1000;
        glOrtho(-width * scale, width * scale, -height * scale, height * scale, -1000000, 1000000);
    } else {
        GLdouble aspect = (GLdouble) width / (GLdouble) height;
        const double zNear = 1;
        const double zFar = 160000;
        gluPerspective(fov, aspect, zNear, zFar);
    }
}

void WidgetGL::initializingCameras() {
    bool found = false;
    for (int i = 0; i < calibFile->getNumCameras(); i++) {
        found = false;
        for (int j = 0; j < calibFile->getCameraCombinations().size(); j++) {
            if (!calibFile->getCameraCombinations()[j]->status == TrackPoint::CalibrationStatus::OK) continue;
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

void WidgetGL::setViewFromCamera(int index) {
    if (index < 0 || index >= camerasData.size()) return;

    int camScreenWidth = camerasData[index]->pixelWidth;
    int camScreenHeight = camerasData[index]->pixelHeight;
    int newScreenWidth = width();
    int newScreenHeight = height();

    if (((double) newScreenWidth / (double) newScreenHeight) > ((double) camScreenWidth / (double) camScreenHeight)) {
        newScreenWidth = (newScreenHeight * camScreenWidth) / camScreenHeight;
    } else {
        newScreenHeight = (newScreenWidth * camScreenHeight) / camScreenWidth;
    }
    updateProjection(newScreenWidth, newScreenWidth, camerasData[index]->fovWidth);
}

void WidgetGL::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0 / 255.0, 18.0 / 255.0, 53.0 / 255.0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    initializeGL();
    /*if (showFromCamera) {
        //gluLookAt(camerasData[camViewIndex]->camPos.x, camerasData[camViewIndex]->camPos.y, camerasData[camViewIndex]->camPos.z, 0, 0, 0, 0, 0, 1);
        setViewFromCamera(camViewIndex);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glPushMatrix();
        //glTranslatef(-camerasData[camViewIndex]->camPos.x, -camerasData[camViewIndex]->camPos.y, -camerasData[camViewIndex]->camPos.z);
        // Orienting camera
        double alpha = camerasData[camViewIndex]->orient.alpha;    // In radians
        double beta = camerasData[camViewIndex]->orient.beta;      // In radians
        double kappa = camerasData[camViewIndex]->orient.kappa;    // In radians
        GLdouble rotationMatrix[16] = {
            cos(alpha) * cos(kappa) + (sin(alpha) * sin(beta) * sin(kappa)), cos(beta) * sin(kappa), -sin(alpha) * cos(kappa) + (cos(alpha) * sin(beta) * sin(kappa)), camerasData[camViewIndex]->camPos.x,
            sin(alpha) * sin(beta) * cos(kappa) - cos(alpha) * sin(kappa), cos(beta) * cos(kappa), sin(alpha) * sin(kappa) + cos(alpha) * sin(beta) * cos(kappa), camerasData[camViewIndex]->camPos.y,
            sin(alpha) * cos(beta), -sin(beta), cos(alpha) * cos(beta), camerasData[camViewIndex]->camPos.z,
            0,                                                                  0,                          0,                                                                  1
        };
        GLdouble invMatrix[16];
        gluInvertMatrix(rotationMatrix, invMatrix);
        glMultMatrixd(invMatrix);
        //glmatrix
        //glRotated(180, 0, 1, 0);
        //glPopMatrix();
    } else {*/
        if (showOrtho) {
            gluLookAt(cos(rotY) * sin(rotX), cos(rotY) * cos(rotX), sin(rotY), 0, 0, 0, 0, 0, 1);
        } else {
            gluLookAt(cos(rotY) * sin(rotX) * camDistance, cos(rotY) * cos(rotX) * camDistance, sin(rotY) * camDistance, 0, 0, 0, 0, 0, 1);
        }
    //}
    
    glColor3f(1, 0, 0);
    glScaled(1, 1, 1);

    // Drawing lines connecting the preceeding point up to the selected timeframe
    if (showLines && pointData.size() > 0) {
        glColor3f(0.75f, 0.75f, 0.75f);
        for (int point = 0; point < (pointData[0]).size(); point++){
            glBegin(GL_LINE_STRIP);
            for (int time = qMax(0, coordinatesShown - numShowedPreceedingPoints); time <= coordinatesShown; time++) {
                glColor4f(1, 1, 1, 1 - (((double) coordinatesShown - time) / numShowedPreceedingPoints));
                glVertex3f(GLfloat(pointData[time][point]->x),
                           GLfloat(pointData[time][point]->z),
                           GLfloat(pointData[time][point]->y));
            }
            glEnd();
        }
    }

    glColor3f(1, 0, 0);
    glPointSize(2.5f);
    // Drawing the preceeding points to the selected timeframe
    if (showPreceedingPoints && pointData.size() > 0) {
        for (int time = qMax(0, coordinatesShown - numShowedPreceedingPoints); time < coordinatesShown; time++) {
            for (int point = 0; point < (pointData[time]).size(); point++) {
                glColor4f(1, 0, 0, 1 - (((double) coordinatesShown - time) / numShowedPreceedingPoints));
                glBegin(GL_POINTS);
                glVertex3f(GLfloat(pointData[time][point]->x),
                           GLfloat(pointData[time][point]->z),
                           GLfloat(pointData[time][point]->y));
                glEnd();
            }
        }
    }

    glColor3f(1, 0, 0);
    glPointSize(5.0);
    // Drawing the actual points in the selected timeframe
    if (pointData.size() > 0) {
        for (int point = 0; point < (pointData[coordinatesShown]).size(); point++){
            glBegin(GL_POINTS);
            glVertex3f(GLfloat(pointData[coordinatesShown][point]->x),
                       GLfloat(pointData[coordinatesShown][point]->z),
                       GLfloat(pointData[coordinatesShown][point]->y));
            glEnd();
        }
    }    

    // Drawing the cameras
    if (showCameras) {
        for (int i = 0; i < camerasData.size(); i++){       
            glPushMatrix();
            glColor3f(0.9, 0, 0.9);
            glPointSize(8.0);
            //glRotated(camerasData[i]->orient.kappa  * (180 / PI), 0, 0, 1);
            //glRotated(camerasData[i]->orient.beta   * (180 / PI), 0, 1, 0);
            //glRotated(camerasData[i]->orient.alpha  * (180 / PI), 1, 0, 0);
            glTranslatef(camerasData[i]->camPos.x, camerasData[i]->camPos.y, camerasData[i]->camPos.z);
            glBegin(GL_POINTS);
            glVertex3f(0, 0, 0);
            glEnd();
            
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
                // Orienting camera
                double alpha = camerasData[i]->orient.alpha;    // In radians
                double beta = camerasData[i]->orient.beta;      // In radians
                double kappa = camerasData[i]->orient.kappa;    // In radians
                GLdouble rotationMatrix[16] = {
                    cos(alpha) * cos(kappa) + (sin(alpha) * sin(beta) * sin(kappa)),    cos(beta) * sin(kappa),     -sin(alpha)*cos(kappa) + (cos(alpha) * sin(beta) * sin(kappa)),     0,
                    sin(alpha) * sin(beta) * cos(kappa) - cos(alpha) * sin(kappa),      cos(beta) * cos(kappa),     sin(alpha) * sin(kappa) + cos(alpha) * sin(beta) * cos(kappa),      0,
                    sin(alpha) * cos(beta),                                             -sin(beta),                 cos(alpha) * cos(beta),                                             0,
                    0,                                                                  0,                          0,                                                                  1
                };
                glMultMatrixd(rotationMatrix);

                // Converting the FOV's to degrees from radians
                double fovW = (camerasData[i]->fovWidth * (180.0 / PI));
                double fovH = (camerasData[i]->fovHeight * (180.0 / PI));
                
                // Calculating the distance from lens to the bottom of the square, and the distance from the center to the sides
                double l = cos((camerasData[i]->fovHeight / 2)) * cos((camerasData[i]->fovWidth / 2)) * coneSize;
                double w2 = cos((camerasData[i]->fovHeight / 2)) * sin((camerasData[i]->fovWidth / 2)) * coneSize;
                double h2 = sin((camerasData[i]->fovHeight / 2)) * coneSize;

                // The color of the Pyramid
                glColor3f(0, 1, 0);

                const bool showFOVWireFrame = true;
                if (showFOVWireFrame) {
                    // Drawing the bottom of the FOV-pyramid (square)
                    glPushMatrix();
                    glBegin(GL_LINE_LOOP);
                    glVertex3f(w2, h2, -l);
                    glVertex3f(-w2, h2, -l);
                    glVertex3f(-w2, -h2, -l);
                    glVertex3f(w2, -h2, -l);
                    glEnd();
                    glPopMatrix();

                    // Drawing lines from lens to the FOV-Square
                    glPushMatrix();
                    glRotated((fovW / 2), 0, 1, 0);
                    glRotated((fovH / 2), 1, 0, 0);
                    glScaled(0, 0, -coneSize);
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(0, 0, 0);
                    glVertex3f(1, 1, 1);
                    glEnd();
                    glPopMatrix();

                    glPushMatrix();
                    glRotated((fovW / 2), 0, -1, 0);
                    glRotated((fovH / 2), 1, 0, 0);
                    glScaled(0, 0, -coneSize);
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(0, 0, 0);
                    glVertex3f(1, 1, 1);
                    glEnd();
                    glPopMatrix();

                    glPushMatrix();
                    glRotated((fovW / 2), 0, 1, 0);
                    glRotated((fovH / 2), -1, 0, 0);
                    glScaled(0, 0, -coneSize);
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(0, 0, 0);
                    glVertex3f(1, 1, 1);
                    glEnd();
                    glPopMatrix();

                    glPushMatrix();
                    glRotated((fovW / 2), 0, -1, 0);
                    glRotated((fovH / 2), -1, 0, 0);
                    glScaled(0, 0, -coneSize);
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(0, 0, 0);
                    glVertex3f(1, 1, 1);
                    glEnd();
                    glPopMatrix();
                }

                const bool showPolygonBase = false;
                if (showPolygonBase) {
                    glPushMatrix();
                    //glRotated((fovW / 2), 0, -1, 0);
                    glRotated((fovH / 2), -1, 0, 0);
                    //glScaled(0, 0, -coneSize);
                    glBegin(GL_POLYGON);
                    glVertex3f(0, 0, 0);
                    glVertex3f(-(sin(fovW * (PI / 180)) * coneSize), 0, -coneSize);
                    //glRotated(fovW, 0, 1, 0);
                    glVertex3f((sin(fovW * (PI / 180)) * coneSize), 0, -coneSize);
                    glVertex3f(0, 0, 0);
                    glEnd();
                    glPopMatrix();
                }
            }
            glPopMatrix();
        }
    }

    // Drawing the coordinate axises
    glLineWidth(1);
    if (showCoordinateSystem) {
        const double gridSize = 1000;
        const double coneRadius = 50;
        const double coneHeight = 100;
        const int coneDivisions = 10;
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
    const float color = 0.2f;
    const float colorMeter = 0.4f;
    const double maxLength = 20000;
    const double lengthDivs = 100;
    glColor4f(1, 1, 1, color);
    const bool isFadingColorFloorEdge = false;
    const bool isGridByMeter = true;
    const double rotYThreshold = 1 / (180 / PI); // Threshold to determine if camera is edge on the floor, obstructing the coordinate axis
    if (showFloorLines && ((rotY > rotYThreshold || rotY < -rotYThreshold) || !showCoordinateSystem)) {
        
        if (isGridByMeter) {
            glBegin(GL_LINE_STRIP);
            glColor4f(1, 1, 1, colorMeter);
            glVertex3d(0, maxLength / 2, 0);
            glVertex3d(0, -(maxLength / 2), 0);
            glEnd();
            glBegin(GL_LINE_STRIP);
            glColor4f(1, 1, 1, colorMeter);
            glVertex3d(maxLength / 2, 0, 0);
            glVertex3d(-(maxLength / 2), 0, 0);
            glEnd();
        } else {
            glBegin(GL_LINE_STRIP);
            if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
            glVertex3d(0, maxLength / 2, 0);
            if (isFadingColorFloorEdge) glColor4f(1, 1, 1, color);
            glVertex3d(0, 0, 0);
            if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
            glVertex3d(0, -(maxLength / 2), 0);
            glEnd();
            glBegin(GL_LINE_STRIP);
            if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
            glVertex3d(maxLength / 2, 0, 0);
            if (isFadingColorFloorEdge) glColor4f(1, 1, 1, color);
            glVertex3d(0, 0, 0);
            if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
            glVertex3d(-(maxLength / 2), 0, 0);
            glEnd();
        }
        for (int i = 1; i < ((int) (maxLength / 2) / lengthDivs); i++) {
            if (!isFadingColorFloorEdge) glColor4f(1, 1, 1, color);
            if (((int) (i * lengthDivs) % 1000 != 0)) {
                glBegin(GL_LINE_STRIP);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(i * lengthDivs, maxLength / 2, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, color * (1 - (i / ((maxLength / 2) / lengthDivs))));
                glVertex3d(i * lengthDivs, 0, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(i * lengthDivs, -(maxLength / 2), 0);
                glEnd();

                glBegin(GL_LINE_STRIP);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(-i * lengthDivs, maxLength / 2, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, color * (1 - (i / ((maxLength / 2) / lengthDivs))));
                glVertex3d(-i * lengthDivs, 0, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(-i * lengthDivs, -(maxLength / 2), 0);
                glEnd();

                glBegin(GL_LINE_STRIP);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(maxLength / 2, i * lengthDivs, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, color * (1 - (i / ((maxLength / 2) / lengthDivs))));
                glVertex3d(0, i * lengthDivs, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(-(maxLength / 2), i * lengthDivs, 0);
                glEnd();

                glBegin(GL_LINE_STRIP);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(maxLength / 2, -i * lengthDivs, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, color * (1 - (i / ((maxLength / 2) / lengthDivs))));
                glVertex3d(0, -i * lengthDivs, 0);
                if (isFadingColorFloorEdge) glColor4f(1, 1, 1, 0);
                glVertex3d(-(maxLength / 2), -i * lengthDivs, 0);
                glEnd();
            }

            if (isGridByMeter && ((int) (i * lengthDivs) % 1000 == 0)) {
                glColor4f(1, 1, 1, colorMeter);
                glBegin(GL_LINE_STRIP);
                glVertex3d(i * lengthDivs, maxLength / 2, 0);
                glVertex3d(i * lengthDivs, -(maxLength / 2), 0);
                glEnd();

                glBegin(GL_LINE_STRIP);
                glVertex3d(-i * lengthDivs, maxLength / 2, 0);
                glVertex3d(-i * lengthDivs, -(maxLength / 2), 0);
                glEnd();

                glBegin(GL_LINE_STRIP);
                glVertex3d(maxLength / 2, i * lengthDivs, 0);
                glVertex3d(-(maxLength / 2), i * lengthDivs, 0);
                glEnd();

                glBegin(GL_LINE_STRIP);
                glVertex3d(maxLength / 2, -i * lengthDivs, 0);
                glVertex3d(-(maxLength / 2), -i * lengthDivs, 0);
                glEnd();
            }
        }
    }
}

void WidgetGL::resizeGL(int width, int height) {
    initializeGL();
}

void WidgetGL::showView(int viewTime) {
    coordinatesShown = viewTime;
    update();
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
        int num = (eventWheel->angleDelta().y() / 120);
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
            camDistance = qMax((double) 10, qMin(camDistance - (eventWheel->angleDelta().y() * deltaSpeed), (double) 100000));
            update();
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
    return QOpenGLWidget::eventFilter(obj, event);
}

void WidgetGL::mouseMoveEvent(QMouseEvent* mouseEvent) {
    if (mouseEvent->buttons() & Qt::LeftButton && !showFromCamera) {
        if (lastMouseX < 0 || lastMouseY < 0) {
            lastMouseX = mouseEvent->localPos().x();
            lastMouseY = mouseEvent->localPos().y();
            return;
        }
        const double speed = 0.005;
        double maxVerticalAngle = PI / 2; // 90 degrees in radians...
        rotX += (lastMouseX - mouseEvent->localPos().x()) * speed;
        rotY = qMax(-maxVerticalAngle, qMin(rotY + ((lastMouseY - mouseEvent->localPos().y()) * speed), maxVerticalAngle));
        update();
    }
    lastMouseX = mouseEvent->localPos().x();
    lastMouseY = mouseEvent->localPos().y();
}

void WidgetGL::clickOnMenu(QAction *action) {
    QString number = action->text().split(QString::fromUtf8("\u00b0")).at(1);
    int nb = number.toInt();
    initializeGL();
}

void WidgetGL::showXYPlane() {
    if (showFromCamera) return;
    rotX = PI;
    rotY = PI / 2;
    update();
}

void WidgetGL::showXZPlane() {
    if (showFromCamera) return;
    rotX = PI;
    rotY = 0;
    update();
}

void WidgetGL::showYZPlane() {
    if (showFromCamera) return;
    rotX = PI / 2;
    rotY = 0;
    update();
}
