#include "displaywindow.h"

#include <QApplication>



static const double PI = 3.1415926535;

DisplayWindow::DisplayWindow(QWidget *parent) : QOpenGLWidget(parent) {
    data = NULL;
    currentStep = 0;
    linkMarkerMode = false;
    eraseOneLinkMode = false;
    displayLinks = true;
    selectMarkerMode = false;
    swapMode = false;
    displayFormerSteps = false;
    displayFurtherSteps = false;
    formerStepsPoints = false;
    formerStepsSelectedMarkers = false;
    lineBeingDrawn = false;
    linkedMarkersIndexes.append(std::array<int, 2>({-1, -1}));
    for(auto& element : markersToBeSwapedIndexes) {
        element = -1;
    }
    for(int i = 7 ; i < 19 ; i++) {
        colorsAvailable.append(i);
    }
    installEventFilter(this);
    this->setObjectName("DisplayWindow");
    this->grabKeyboard();
}

void DisplayWindow::setViewPort() {
    glViewport(0, 0, width() * this->devicePixelRatio(), height() * this->devicePixelRatio());
}

void DisplayWindow::setProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    QMatrix4x4 projection;
    projection.perspective(70.0, (double)width() / (double)height(), 1.0, 20.0);
    float * tab = projection.data();
    glMultMatrixf(tab);
}

void DisplayWindow::setModelView() {
    cameraPos = QVector3D(2, -2, 0.5);
    zAngle= 0;
    cameraCenter = QVector3D(0,0,0);
    updateViewFromCameraPos();
}

void DisplayWindow::setData(const Data * pointerToData) {
    data = pointerToData;
}

void DisplayWindow::setCurrentStep(int index) {
    currentStep = index;
}

const QVector<int>& DisplayWindow::getSelectedMarkerIndexes() const {
    return selectedMarkerIndexes;
}

const std::array<int, 2>& DisplayWindow::getMarkersToBeSwaped() const {
    return markersToBeSwapedIndexes;
}

const QVector<std::array<int, 2>>& DisplayWindow::getLinkedMarkersIndexes() const {
    return linkedMarkersIndexes;
}

void DisplayWindow::removePickedIndex(int index) {
    for(int i=0;i<2;i++){
        if(selectedMarkerIndexes.at(index)==markersToBeSwapedIndexes[i]){
            emit changeColorMarkerToBeSwapped(i, Qt::white);
        }
    }
    selectedMarkerIndexes.remove(index);
    colorsAvailable.append(colorsAvailable.at(index));
    colorsAvailable.remove(index);
    update();
}

void DisplayWindow::initializeGL()
{
    initializeOpenGLFunctions();//THIS MUST BE CALLED TO INIT OPENGL

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    qDebug() << "initializeGL()";
}

void DisplayWindow::paintGL()
{
    //qDebug() << "paintGL";
    glPointSize(3.0);
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundColor.alphaF());

    glClear(GL_COLOR_BUFFER_BIT);



    if(data != NULL) {
        paintFloor();
        paintMarkers();
        paintMarkersWithRedCross();
        if (dragSelectActive) {
            paintDragZone();
        }
        if(lineBeingDrawn) {
            paintMarkerWithCross();
        }
        if(displayLinks) {
            paintLinkedMarkers();
        }
        paintSelectedMarkers();
        if(displayFormerSteps) {
            paintFormerSteps();
        }
        if(displayFurtherSteps) {
            paintFurtherSteps();
        }
        paintAxes();
        if (points.size() > 0) {
            paintPoints();
        }
        glFlush();
    }
}

void DisplayWindow::drawLine(QVector3D begin, QVector3D end) {
    glVertex3f(begin.x(), begin.y(), begin.z());
    glVertex3f(end.x(), end.y(), end.z());
}

void DisplayWindow::drawSquare(QVector3D begin, QVector3D end) {
    glColor4f(dragColor.redF(), dragColor.greenF(), dragColor.blueF(), 0.5);
    QVector3D corner1 = begin;
    QVector3D corner2 = QVector3D(end.x(), begin.y(), begin.z());
    QVector3D corner3 = end;
    QVector3D corner4 = QVector3D(begin.x(), end.y(), end.z());
    while (!points.empty()) {
        points.removeLast();
    }
    points.append(corner1);
    points.append(corner2);
    points.append(corner3);
    points.append(corner4);
}

void DisplayWindow::paintPoints()
{
    makeCurrent();
    glColor4f(dragColor.redF(), dragColor.greenF(), dragColor.blueF(), 0.5);
    glPointSize(5.0);
    glLineWidth(5.0);
    glBegin(GL_POLYGON);
    for (int i=0; i < points.size() ; i++) {
        glVertex3f(points.at(i).x(), points.at(i).y(), points.at(i).z());
    }
    glEnd();
    glPointSize(1.0);
    glLineWidth(1.0);
}

void DisplayWindow::drawFloorLine(float x, float y) {
    if (x <= -floorLength) {
        return;
    } else {
        QVector3D beginHorizontal = QVector3D(floorLength, y, 0);
        QVector3D endHorizontal = QVector3D(-floorLength, y, 0);
        QVector3D beginVertical = QVector3D(x, floorLength, 0);
        QVector3D endVertical = QVector3D(x, -floorLength, 0);
        if (x == (int)x) {
            glColor4f(gridColor.redF(), gridColor.greenF(), gridColor.blueF(), 0.5);
        } else {
            glColor4f(gridColor.redF(), gridColor.greenF(), gridColor.blueF(), 0.2);
        }
        drawLine(beginHorizontal, endHorizontal);
        drawLine(beginVertical, endVertical);
        drawFloorLine(x-0.5, y-0.5);
    }
}

void DisplayWindow::paintDragZone()
{
    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

    QMatrix4x4 modelviewMatrix;
    for (int i = 0; i < 16; i++) {
        modelviewMatrix.data()[i] = static_cast<float>(modelview[i]);
    }

    QMatrix4x4 inverseModelviewMatrix = modelviewMatrix.inverted();

    // Near plane 3D coordinates
    QVector3D start = get3DPos(mouseClickX * this->devicePixelRatio(), mouseClickY * this->devicePixelRatio());
    QVector3D current = get3DPos(mouseDragPosX * this->devicePixelRatio(), mouseDragPosY * this->devicePixelRatio());

    // View space coordinates
    QVector3D startViewSpace = transformToViewSpace(start, modelviewMatrix);
    QVector3D currentViewSpace = transformToViewSpace(current, modelviewMatrix);

    // Test that both have the same z coordinate
    float nearPlaneZ = startViewSpace.z();
    startViewSpace.setZ(nearPlaneZ);
    currentViewSpace.setZ(nearPlaneZ);

    // Transform to world space
    QVector3D startAligned = transformFromViewSpace(startViewSpace, inverseModelviewMatrix);
    QVector3D currentAligned = transformFromViewSpace(currentViewSpace, inverseModelviewMatrix);

    makeCurrent();
    drawSquare(startAligned, currentAligned);
}

QVector3D DisplayWindow::get3DPos(int x, int y) {
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLfloat winX = (GLfloat)x;
    GLfloat winZ = (GLfloat) 0.0f;
    GLfloat winY = (GLfloat)(viewport[3] - y);

    // obtain global coordinates
    double posX, posY, posZ;
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
    return QVector3D(static_cast<float>(posX), static_cast<float>(posY), static_cast<float>(posZ));
}

QVector3D DisplayWindow::transformToViewSpace(const QVector3D& point, const QMatrix4x4& modelview) {
    QVector4D homogenousPoint = QVector4D(point, 1.0f);
    QVector4D viewSpacePoint = modelview * homogenousPoint;
    return viewSpacePoint.toVector3D();

}

QVector3D DisplayWindow::transformFromViewSpace(const QVector3D& point, const QMatrix4x4& inverseModelview) {
    QVector4D homogenousPoint = QVector4D(point, 1.0f);
    QVector4D viewSpacePoint = inverseModelview * homogenousPoint;
    return viewSpacePoint.toVector3D();
}

void DisplayWindow::mousePressEvent(QMouseEvent *event) {
    mouseXStartPosition = event->position().x() * devicePixelRatio();
    mouseYStartPosition = event->position().y() * devicePixelRatio();
    if(selectMarkerMode) {
        selectMarker();
    }
    else if(linkMarkerMode) {
        linkMarkerLine();
    }
    else if(swapMode) {
        swapMarkers();
    }
    else if(eraseOneLinkMode && displayLinks) {
        removePickedLink();
    }
}

bool DisplayWindow::eventFilter(QObject *watched, QEvent *event)
{

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        mouseClickX = mouseEvent->pos().x() * this->devicePixelRatio();
        mouseClickY = mouseEvent->pos().y() * this->devicePixelRatio();
        if ((selectMarkerMode || linkMarkerMode || swapMode || (eraseOneLinkMode && displayLinks))) {
        } else {
            dragSelectActive = (mouseEvent->button() == Qt::RightButton);
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        mouseClickX = 0;
        mouseClickY = 0;
        mouseDragPosX = 0;
        mouseDragPosY = 0;
        if (dragSelectActive) { dragSelectActive = false;}

    }
    else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        mouseDragPosX = mouseEvent->pos().x();
        mouseDragPosY = mouseEvent->pos().y();
        if (mouseEvent->buttons() == Qt::LeftButton) {
            moveCamera(mouseEvent);
        }
    }
    else if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->angleDelta().y() > 0) {
            changeCameraPosition(0,0,1);
        } else {
            changeCameraPosition(0,0,-1);
        }
    }
    else if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch(keyEvent->key()) {
        case(Qt::Key_Up):
            // changeCameraPosition(0,1,0);
            changeCameraPosition(0,0,1);
            break;
        case(Qt::Key_Down):
            //changeCameraPosition(0,-1,0);
            changeCameraPosition(0,0,-1);
            break;
        case(Qt::Key_Left):
            //changeCameraPosition(-1,0,0);
            zAngle += 15;
            zAngle = zAngle%360;
            updateViewFromCameraPos();
            break;
        case(Qt::Key_Right):
            //changeCameraPosition(1,0,0);
            zAngle -= 15;
            zAngle = zAngle%360;
            updateViewFromCameraPos();
            break;
        }
    }


    if (dragSelectActive) {
        backgroundColor.setBlue(0);
    } else {
        backgroundColor.setBlue(58);
    }
    update();
    return QOpenGLWidget::eventFilter(watched, event);
}

void DisplayWindow::paintFloor()
{
    makeCurrent();
    glBegin(GL_LINES);
    drawFloorLine(floorLength, floorLength);
    glEnd();
}

void DisplayWindow::paintMarkers() {
    makeCurrent();
    glBegin(GL_POINTS);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    QVector<Marker> vector = data->get1Vector(currentStep);
    QList<Marker> list = QList<Marker>(vector.begin(), vector.end());
    for(auto marker : list) {
        glVertex3f(marker.getX() / 1500, marker.getY() / 1500, marker.getZ() / 1500);
    }
    glEnd();
}

void DisplayWindow::paintSelectedMarkers() {
    makeCurrent();
    int colorIndex = 0;
    QColor *color;
    glBegin(GL_POINTS);
    for(auto index : selectedMarkerIndexes) {
        color = new QColor(Qt::GlobalColor(colorsAvailable.at(colorIndex)));
        glColor4f(color->red() / 255.0, color->green() / 255.0, color->blue() / 255.0, 1.0);
        glVertex3f(data->get1Marker(currentStep, index).getX() / 1500, data->get1Marker(currentStep, index).getY() / 1500, data->get1Marker(currentStep, index).getZ() / 1500);
        delete color;
        colorIndex++;
    }
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnd();
}

void DisplayWindow::paintAxes() {
    makeCurrent();
    glBegin(GL_LINES);
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0.0, 1.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0.0, 0.0, 1.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();
}

void DisplayWindow::paintMarkerWithCross() {
    makeCurrent();
    glBegin(GL_LINES);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glVertex3f((getMarkerWithCross().getX() + 50) / 1500, (getMarkerWithCross().getY()) / 1500, (getMarkerWithCross().getZ()) / 1500);
    glVertex3f((getMarkerWithCross().getX() - 50) / 1500, (getMarkerWithCross().getY()) / 1500, (getMarkerWithCross().getZ()) / 1500);
    glVertex3f((getMarkerWithCross().getX()) / 1500, (getMarkerWithCross().getY() + 50) / 1500, (getMarkerWithCross().getZ()) / 1500);
    glVertex3f((getMarkerWithCross().getX()) / 1500, (getMarkerWithCross().getY() - 50) / 1500, (getMarkerWithCross().getZ()) / 1500);
    glVertex3f((getMarkerWithCross().getX()) / 1500, (getMarkerWithCross().getY()) / 1500, (getMarkerWithCross().getZ() + 50) / 1500);
    glVertex3f((getMarkerWithCross().getX()) / 1500, (getMarkerWithCross().getY()) / 1500, (getMarkerWithCross().getZ() - 50) / 1500);
    glEnd();
}

void DisplayWindow::paintMarkersWithRedCross() {
    makeCurrent();
    glBegin(GL_LINES);
    glColor4f(1.0, 0.0, 0.0, 1.0);
    for(auto index : markersToBeSwapedIndexes) {
        if(index != -1) {
            glVertex3f((data->get1Marker(currentStep, index).getX() + 50) /1500, data->get1Marker(currentStep, index).getY() / 1500, data->get1Marker(currentStep, index).getZ()/ 1500);
            glVertex3f((data->get1Marker(currentStep, index).getX() - 50) /1500, data->get1Marker(currentStep, index).getY() / 1500, data->get1Marker(currentStep, index).getZ()/ 1500);
            glVertex3f(data->get1Marker(currentStep, index).getX() /1500, (data->get1Marker(currentStep, index).getY() + 50) / 1500, data->get1Marker(currentStep, index).getZ()/ 1500);
            glVertex3f(data->get1Marker(currentStep, index).getX() /1500, (data->get1Marker(currentStep, index).getY() - 50) / 1500, data->get1Marker(currentStep, index).getZ()/ 1500);
            glVertex3f(data->get1Marker(currentStep, index).getX() / 1500, data->get1Marker(currentStep, index).getY() / 1500, (data->get1Marker(currentStep, index).getZ() + 50) / 1500);
            glVertex3f(data->get1Marker(currentStep, index).getX() / 1500, data->get1Marker(currentStep, index).getY() / 1500, (data->get1Marker(currentStep, index).getZ() - 50) / 1500);
        }
    }
    glEnd();
}

void DisplayWindow::paintFormerSteps() {
    makeCurrent();
    glPointSize(1.5);
    int j = 0;
    int colorIndex = 0;
    QColor *color;
    for(int i = 0 ; i < data->get1Vector(0).size() ; i++) {
        if(currentStep >= numberOfFormerStepsDisplayed) {
            j = currentStep - numberOfFormerStepsDisplayed;
        }
        else {
            j = 0;
        }
        if(formerStepsPoints) {
            glBegin(GL_POINTS);
        }
        else {
            glBegin(GL_LINE_STRIP);
        }
        // We assess wether this marker is selected or not
        colorIndex = selectedMarkerIndexes.indexOf(i);
        if(colorIndex != -1) {
            color = new QColor(Qt::GlobalColor(colorsAvailable.at(colorIndex)));
            glColor4f(color->red() / 255.0, color->green() / 255.0, color->blue() / 255.0, 1.0);
            delete color;
            colorIndex++;
            while(j <= currentStep) {
                glVertex3f(data->get1Marker(j, i).getX() / 1500, data->get1Marker(j, i).getY() / 1500, data->get1Marker(j, i).getZ() / 1500);
                j++;
            }
            glColor4f(1.0, 1.0, 1.0, 1.0);
        }
        else if(!formerStepsSelectedMarkers){
            while(j <= currentStep) {
                glVertex3f(data->get1Marker(j, i).getX() / 1500, data->get1Marker(j, i).getY() / 1500, data->get1Marker(j, i).getZ() / 1500);
                j++;
            }
        }
        glEnd();
    }
}

void DisplayWindow::paintFurtherSteps() {
    makeCurrent();
    glPointSize(1.5);
    int colorIndex = 0;
    int indexSelected = 0;
    QColor *color;
    int j = 0;
    int limitOfStepsDisplayed = currentStep + numberOfFurtherStepsDisplayed;
    if(data->getDataCoordinatesSize() <= limitOfStepsDisplayed) {
        limitOfStepsDisplayed = data->getDataCoordinatesSize() - 1;
    }
    for(int i = 0; i < selectedMarkerIndexes.size() ; i++) {
        indexSelected = selectedMarkerIndexes.at(i);
        j = currentStep;
        color = new QColor(Qt::GlobalColor(colorsAvailable.at(colorIndex)));
        glBegin(GL_LINE_STRIP);
        glColor4f(color->red() / 255.0, color->green() / 255.0, color->blue() / 255.0, 1.0);
        delete color;
        colorIndex++;
        while(j < limitOfStepsDisplayed + 1) {
            glVertex3f(data->get1Marker(j, indexSelected).getX() / 1500, data->get1Marker(j, indexSelected).getY() / 1500, data->get1Marker(j, indexSelected).getZ() / 1500);
            j++;
        }
        glEnd();
    }
}

void DisplayWindow::paintLinkedMarkers() {
    makeCurrent();
    glBegin(GL_LINES);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    for(int i = 0 ; i < linkedMarkersIndexes.size() - 1 ; i++) {
        glVertex3f(data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getX() / 1500, data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getY() / 1500,
                   data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getZ() / 1500);
        glVertex3f(data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getX() / 1500, data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getY() / 1500,
                   data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getZ() / 1500);
    }
    glEnd();
}

const Marker& DisplayWindow::getMarkerWithCross() const {
    return data->get1Marker(currentStep,linkedMarkersIndexes.last()[0]);
}


int DisplayWindow::pickMarker() {
    QOpenGLWidget::makeCurrent();
    glPointSize(3.0);
    unsigned char pixelRead[3];
    glDrawBuffer(GL_BACK);
    glClearColor(0.0, 0.0 ,0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    for(auto marker : data->get1Vector(currentStep)) {
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glColor4f(marker.getRedId() / 255.0, marker.getGreenId() / 255.0, marker.getBlueId() / 255.0, 1.0);
        glVertex3f(marker.getX() / 1500, marker.getY() / 1500, marker.getZ() / 1500);
        glEnd();
    }
    glFlush();
    glReadPixels(mouseXStartPosition, height() * devicePixelRatio() - mouseYStartPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelRead);
    update();
    /* As the background is black, its index is going to be equal to zero (rgb (0, 0, 0) == black).
        this means that when the user is going to click on the background, the marker at the index 0 will be "picked".
        To prevent this from happening we add 1 to the color identifier of the markers when they are constructed and
        we subtract 1 from the returned result of this method. That way, when the background is clicked, -1 will be returned.
    */
    return (int)pixelRead[0] + (int)pixelRead[1] *256 + (int)pixelRead[2] * 256 *256 - 1;
}

void DisplayWindow::selectMarker() {
    int index = pickMarker();
    selectMarker(index);
}

void DisplayWindow::selectMarker(int index) {
    if(selectedMarkerIndexes.size() < 12 && index != -1 && selectedMarkerIndexes.indexOf(index) == -1) {
        selectedMarkerIndexes.append(index);
        emit markerPicked(index, colorsAvailable.at(selectedMarkerIndexes.size() - 1));
        for(int i = 0 ; i < 2 ; i++) {
            if(markersToBeSwapedIndexes[i] == index) {
                emit changeColorMarkerToBeSwapped(i, colorsAvailable.at(selectedMarkerIndexes.indexOf(index)));
            }
        }
    }
    update();
}

void DisplayWindow::linkMarkerLine() {
    makeCurrent();
    int index = pickMarker();
    linkMarkerLine(index);
}

void DisplayWindow::linkMarkerLine(int index) {
    if(index == -1 || index == linkedMarkersIndexes.last()[0]) {
        linkedMarkersIndexes.last()[0] = -1;
        lineBeingDrawn = false;
    }
    else {
        // if an index has already been picked
        if(linkedMarkersIndexes.last()[0] != -1) {
            linkedMarkersIndexes.last()[1] = index;
            // if these indexes are already linked, we can simply remove them
            if(alreadyLinkedMarkers(linkedMarkersIndexes.last())) {
                linkedMarkersIndexes.last()[0] = -1;
                linkedMarkersIndexes.last()[1] = -1;
            }
            // else we append a new array at the end of the list for the future link
            else {
                emit linkAdded(linkedMarkersIndexes.last()[0], linkedMarkersIndexes.last()[1]);
                linkedMarkersIndexes.append(std::array<int, 2>({-1, -1}));
            }
            lineBeingDrawn = false;
        }
        // if it is the first index/marker picked
        // lineBeingDrawn is set to true to paint a white cross on the screen to show that a marker has been picked
        else {
            linkedMarkersIndexes.last()[0] = index;
            lineBeingDrawn = true;
        }
    }
    update();
}


int DisplayWindow::pickLink() {
    makeCurrent();
    unsigned char pixelRead[3];
    glDrawBuffer(GL_BACK);
    glClearColor(0.0, 0.0 ,0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    int redId = 0;
    int greenId = 0;
    int blueId = 0;
    for(int i = 0 ; i < linkedMarkersIndexes.size() - 1 ; i++) {
        glBegin(GL_LINES);
        redId = ((i + 1) & 0x000000FF);
        greenId = ((i + 1) & 0x0000FF00) >>  8;
        blueId = ((i + 1) & 0x00FF0000) >> 16;
        glColor4f(redId / 255.0, greenId / 255.0, blueId / 255.0, 1.0);
        glVertex3f(data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getX() / 1500, data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getY() / 1500,
                   data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getZ() / 1500);
        glVertex3f(data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getX() / 1500, data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getY() / 1500,
                   data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getZ() / 1500);
        glEnd();
    }
    glFlush();
    glReadPixels(mouseXStartPosition, height() * devicePixelRatio() - mouseYStartPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelRead);
    update();
    return (int)pixelRead[0] + (int)pixelRead[1] *256 + (int)pixelRead[2] * 256 *256 - 1;
}

void DisplayWindow::removePickedLink() {
    int index = pickLink();
    if(index != -1) {
        emit linkRemoved(index);
        linkedMarkersIndexes.remove(index);
    }
}

bool DisplayWindow::alreadyLinkedMarkers(std::array<int, 2>& linkedMarkers) {
    for(int i = 0 ; i < linkedMarkersIndexes.size() - 1 ; i++) {
        if((linkedMarkers[0] == linkedMarkersIndexes.at(i)[0] && linkedMarkers[1] == linkedMarkersIndexes.at(i)[1]) ||
            (linkedMarkers[0] == linkedMarkersIndexes.at(i)[1] && linkedMarkers[1] == linkedMarkersIndexes.at(i)[0])) {
            return true;
        }
    }
    return false;
}
void DisplayWindow::resetLinkedMarkersIndexes() {
    linkedMarkersIndexes = QVector<std::array<int, 2>>();
    linkedMarkersIndexes.append(std::array<int, 2>({-1, -1}));
    lineBeingDrawn = false;
    update();
}

void DisplayWindow::swapMarkers() {
    int index = pickMarker();
    swapMarkers(index);
}

void DisplayWindow::swapMarkers(int index) {
    int color = -1;
    if(selectedMarkerIndexes.indexOf(index) != -1) {
        color = colorsAvailable.at(selectedMarkerIndexes.indexOf(index));
    }
    if(markersToBeSwapedIndexes.at(0) == -1){
        markersToBeSwapedIndexes.at(0)=index;
        if(index != -1) {
            emit markerToBeSwappedPicked(0, index, color);
        }
    }
    else {
        if(markersToBeSwapedIndexes.at(1) == -1) {
            if(index == markersToBeSwapedIndexes.at(0) || index == -1) {
                markersToBeSwapedIndexes.at(0) = -1;
                emit removeMarkerToBeSwapped(0);

            }
            else {
                markersToBeSwapedIndexes.at(1) = index;
                emit markerToBeSwappedPicked(1, index, color);
            }
        }
        else {
            if(index == markersToBeSwapedIndexes.at(1)) {
                markersToBeSwapedIndexes.at(1) = -1;
                emit removeMarkerToBeSwapped(1);
            }
            else if(index == markersToBeSwapedIndexes.at(0)){
                markersToBeSwapedIndexes.at(0)=markersToBeSwapedIndexes.at(1);
                markersToBeSwapedIndexes.at(1)=-1;
                emit removeMarkerToBeSwapped(0);
                emit markerToBeSwappedPicked(0, index, color);
                emit removeMarkerToBeSwapped(1);
            }
            else {
                markersToBeSwapedIndexes.at(1) = index;
                emit removeMarkerToBeSwapped(1);
                if(index != -1) {
                    emit markerToBeSwappedPicked(1, index, color);
                }
            }
        }
    }
    update();
}

void DisplayWindow::moveCamera(QMouseEvent *event) {
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    //int zAngle = 0;
    //int yAngle = 0;
    zAngle += (event->position().x() * devicePixelRatio() - mouseXStartPosition) / 4;
    zAngle = zAngle%360;
    //yAngle = (mouseYStartPosition - event->position().y() * devicePixelRatio()) / 4;
    //glRotatef(-yAngle, 0, 1, 0);
    //glRotatef(yAngle, 1, 0, 0);

    updateViewFromCameraPos();
    mouseXStartPosition = event->position().x() * devicePixelRatio();
    mouseYStartPosition = event->position().y() * devicePixelRatio();
    update();
}

void DisplayWindow::updateProjection(int width, int height, double fov) {
    if (height == 0) height = 1;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLdouble aspect = (GLdouble) width / (GLdouble) height;
    const double zNear = 1;
    const double zFar = 160000;
    gluPerspective(fov, aspect, zNear, zFar);
}

void DisplayWindow::resetCamera() {
    makeCurrent();
    setModelView();
    update();
}

void DisplayWindow::moveCameraToFrontSide() {
    cameraPos = QVector3D(2, 2, 0.5);
    cameraCenter = QVector3D(0,0,0);
    zAngle = 0;
    updateViewFromCameraPos();
}

void DisplayWindow::moveCameraToBackSide() {
    cameraPos = QVector3D(-2, -2, 0.5);
    cameraCenter = QVector3D(0,0,0);
    zAngle = 0;
    updateViewFromCameraPos();
}

void DisplayWindow::moveCameraToLeftSide() {
    cameraPos = QVector3D(2, -2, 0.5);
    cameraCenter = QVector3D(0,0,0);
    zAngle = 0;
    updateViewFromCameraPos();
}

void DisplayWindow::moveCameraToRightSide() {
    cameraPos = QVector3D(-2, 2, 0.5);
    cameraCenter = QVector3D(0,0,0);
    zAngle = 0;
    updateViewFromCameraPos();
}

void DisplayWindow::changeCameraPosition(int x, int y, int z) {
    cameraPos.setX(cameraPos.x() + x);
    cameraPos.setY(cameraPos.y() + y);
    cameraPos.setZ(cameraPos.z() + z);

    // cameraCenter.setX(cameraCenter.x() + x);
    // cameraCenter.setY(cameraCenter.y() + y);
    // cameraCenter.setZ(cameraCenter.z() + z);
    updateViewFromCameraPos();
}

void DisplayWindow::updateViewFromCameraPos() {
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    QMatrix4x4 view;
    view.lookAt(cameraPos, cameraCenter, QVector3D(0.0, 0.0, 1.0));
    view.rotate(zAngle, 0, 0, 1);
    float * tab = view.data();
    glMultMatrixf(tab);
    update();
}

void DisplayWindow::setLinkMarkerMode(bool boolean) {
    linkMarkerMode = boolean;
}

void DisplayWindow::setDisplayLinks(bool boolean) {
    displayLinks = boolean;
    update();
}

void DisplayWindow::setSelectMarkerMode(bool boolean) {
    selectMarkerMode = boolean;
}

void DisplayWindow::setDisplayFormerSteps(bool boolean) {
    displayFormerSteps = boolean;
    update();
}

void DisplayWindow::setNumberOfFormerStepsDisplayed(int number) {
    numberOfFormerStepsDisplayed = number;
    update();
}

void DisplayWindow::setDisplayFurtherSteps(bool boolean) {
    displayFurtherSteps = boolean;
    update();
}

void DisplayWindow::setNumberOfFurtherStepsDisplayed(int number) {
    numberOfFurtherStepsDisplayed = number;
    update();
}

void DisplayWindow::setFormerStepsPoints(bool boolean) {
    formerStepsPoints = boolean;
    update();
}


void DisplayWindow::setFormerStepsSelectedMarkers(bool boolean) {
    formerStepsSelectedMarkers = boolean;
    update();
}

void DisplayWindow::setEraseOneLinkMode(bool boolean) {
    eraseOneLinkMode = boolean;
}

void DisplayWindow::setSwapMode(bool boolean) {
    swapMode = boolean;
}

void DisplayWindow::setLinkedMarkersVector(QVector<std::array<int, 2>> linkedMarkers) {
    linkedMarkersIndexes = linkedMarkers;
}
