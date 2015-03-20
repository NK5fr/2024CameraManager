#include "widgetgl.h"

using namespace std;


// IMPORTANT: Must be compiled in Release-mode to enable rendering...
WidgetGL::WidgetGL(vector<vector<Vector3d>> *points, vector<float> minmax, SocketViewerWidget* socket, QString calibrationPath)
                   : QGLWidget(socket), pointData(*points), initialScale(false), keyPressed(0), svw(socket) {
    coordinatesShown = 0;
    qtBlack = QColor::fromCmykF(0.0, 0.0, 0.0, 0.25);

    initializingCameraCoordinates(calibrationPath);
    initializeGL();
    
    /* Initializing the selected array */
    for (int i = 0; i < pointData[0].size(); i++) {
        selected.insert(i, false);
    }
    rotX = 0;
    rotY = 0;
    camDistance = 4000;
    setMouseTracking(true);
    grabKeyboard();
    //initializingCameraCoordinates(calibrationPath);
    installEventFilter(this);
    updateGL();
}

WidgetGL::~WidgetGL() {
}

void WidgetGL::initializeGL() {
    int height = this->height();
    int width = this->width();
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat) width / (GLfloat) height;
    const double PI = 3.1415926535;
    double zNear = 0.3;
    double zFar = 8000;
    double fov = 60;
    gluPerspective(fov, (GLfloat) width / (GLfloat) height, zNear, zFar);
}

// TODO: Rewrite to support the calibration-file... (summary-version)
void WidgetGL::initializingCameraCoordinates(QString calibrationPath) {
    printf("CalibrationPath: %s\n", calibrationPath.toUtf8().constData());
    QFile myFile(calibrationPath);
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QString fullText = QString(myFile.readAll());
    myFile.close();

    // Tries to find out how many cameras are registered in the calibration file...
    QStringList lineList = fullText.split("\n");
    int numCams = 0;
    while (!lineList[numCams].isEmpty()) numCams++;

    // Generate list of cameras, and sets some values for them...
    camerasData.resize(numCams);
    for (int cam = 0; cam < numCams; cam++) {
        QStringList camList = lineList[cam].split(QRegExp("[^0-9]+")); // First index in camList is empty...
        camerasData[cam].camNo = camList[1].toInt();
        camerasData[cam].serialNo = camList[2].toInt();
    }

    int atLine = numCams;
    // Scans for lines beginning with Camno, and grabs the first coordinates in the next line... (X0, Y0, Z0)
    do {
        atLine++;
        // Find first instance of Camno, and jump to next line to grab the coordinates...
        if (!lineList[atLine].startsWith("Camno")) continue;
        int camIndex = lineList[atLine].split(QRegExp("[^0-9]+"))[1].toInt();
        if (camIndex < 0 || camIndex >= numCams) printf("Could not get \'Camno\' from this line: %s\n", lineList[atLine].constData());
        if (!camerasData[camIndex].valueSet) {
            atLine++;
            QStringList coords = lineList[atLine].split(QRegExp("\s*[XYZ][O0]:\s*"));
            camerasData[camIndex].camPos.x = (coords[1].toDouble());
            camerasData[camIndex].camPos.y = (coords[3].toDouble());
            camerasData[camIndex].camPos.z = (coords[2].toDouble());
            camerasData[camIndex].valueSet = true;
        }

    } while (atLine + 1 < lineList.size());

    // Prints out some info...
    for (int i = 0; i < numCams; i++) {
        printf("Camno: %u, SerialNo: %u\nX: %.2f\tY: %.2f\tZ: %.2f\n\n", camerasData[i].camNo, camerasData[i].serialNo, camerasData[i].camPos.x, camerasData[i].camPos.y, camerasData[i].camPos.z);
    }
}

void WidgetGL::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    gluLookAt(cos(rotY) * sin(rotX) * camDistance, sin(rotY) * camDistance, cos(rotY) * cos(rotX) * camDistance, 0, 0, 0, 0, 1, 0);
    
    /* Drawing points at the 'coordinatesShown' time */
    glPointSize(4.0);
    glColor3f(1, 0, 0);

    glColor3f(0.75f, 0.75f, 0.75f);
    for (int point = 0; point < (pointData[0]).size(); point++){
        glBegin(GL_LINE_STRIP);
        for (int time = 0; time < coordinatesShown; time++) {
            glVertex3f(GLfloat(pointData[time][point].x),
                       GLfloat(pointData[time][point].y),
                       GLfloat(pointData[time][point].z));
        }
        glEnd();
    }
    
    glColor3f(1, 0, 0);
    for (int time = 0; time < coordinatesShown; time++) {
        for (int point = 0; point < (pointData[time]).size(); point++){
            glBegin(GL_POINTS);
            glVertex3f(GLfloat(pointData[time][point].x),
                       GLfloat(pointData[time][point].y),
                       GLfloat(pointData[time][point].z));
            glEnd();
        }
    }

    
    /* Drawing the cameras point */
    glColor3f(0.9, 0, 0.9);
    glPointSize(8.0);
    for (int i = 0; i < camerasData.size(); i++){
        glBegin(GL_POINTS);
        glVertex3f(camerasData[i].camPos.x, camerasData[i].camPos.y, camerasData[i].camPos.z);
        glEnd();
    }

    const double gridSize = 2000;
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
    glVertex3f(0, 0, gridSize);
    glEnd();

    // Z Axis
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, gridSize, 0);
    glEnd();
}

void WidgetGL::resizeGL(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat) width / (GLfloat) height;
    const double PI = 3.1415926535;
    double zNear = 1;
    double zFar = 16000;
    double fov = 60;
    gluPerspective(fov, (GLfloat) width / (GLfloat) height, zNear, zFar);
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
        const double deltaSpeed = 1;
        if (adjustCamDistance) {
            camDistance = max((double) 100, min(camDistance - (eventWheel->delta() * deltaSpeed), (double) 10000));
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
        const double speed = 0.01;
        const double PI = 3.1415926535;
        double maxVerticalAngle = PI / 3; // 60 degrees in radians...
        rotX += (lastMouseX - mouseEvent->screenPos().x()) * speed;
        rotY = max(-maxVerticalAngle, min(rotY - ((lastMouseY - mouseEvent->screenPos().y()) * speed), maxVerticalAngle));
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
