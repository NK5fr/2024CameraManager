#include "displaywindow.h"

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
}

void DisplayWindow::setViewPort() {
    qInfo() << "test";
    glViewport(0, 0, width(), height());
    qInfo() << "test!";
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
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    QMatrix4x4 view;
    view.lookAt(QVector3D(2, -2, 0.5), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0));
    float * tab = view.data();
    glMultMatrixf(tab);
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
    qDebug() << "initializeGL()";
}

void DisplayWindow::paintGL()
{
    qDebug() << "paintGL";
    glPointSize(3.0);
    glClearColor(0.0, 0.0 ,0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "4";
    if(data != NULL) {
        paintMarkers();
        paintMarkersWithRedCross();
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
        glFlush();
    }
}

void DisplayWindow::paintMarkers() {
    makeCurrent();
    glBegin(GL_POINTS);
    glColor3f(1.0, 1.0, 1.0);
    qInfo() << currentStep;
    QVector<Marker> vector = data->get1Vector(currentStep);
    QList<Marker> list = QList<Marker>(vector.begin(), vector.end());
    for(auto marker : list) {
        qInfo() << "x \t" << marker.getX() << "y \t" << marker.getY() << "z \t" << marker.getZ();
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
        glColor3f(color->red() / 255.0, color->green() / 255.0, color->blue() / 255.0);
        glVertex3f(data->get1Marker(currentStep, index).getX() / 1500, data->get1Marker(currentStep, index).getY() / 1500, data->get1Marker(currentStep, index).getZ() / 1500);
        delete color;
        colorIndex++;
    }
    glColor3f(1.0, 1.0, 1.0);
    glEnd();
}

void DisplayWindow::paintAxes() {
    makeCurrent();
    glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);
    glEnd();
}

void DisplayWindow::paintMarkerWithCross() {
    makeCurrent();
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);
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
    glColor3f(1.0, 0.0, 0.0);
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
            glColor3f(color->red() / 255.0, color->green() / 255.0, color->blue() / 255.0);
            delete color;
            colorIndex++;
            while(j <= currentStep) {
                glVertex3f(data->get1Marker(j, i).getX() / 1500, data->get1Marker(j, i).getY() / 1500, data->get1Marker(j, i).getZ() / 1500);
                j++;
            }
            glColor3f(1.0, 1.0, 1.0);
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
        glColor3f(color->red() / 255.0, color->green() / 255.0, color->blue() / 255.0);
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
    glColor3f(1.0, 1.0, 1.0);
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

void DisplayWindow::mousePressEvent(QMouseEvent *event) {
    mouseXStartPosition = event->position().x();
    mouseYStartPosition = event->position().y();
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

void DisplayWindow::mouseMoveEvent(QMouseEvent *event) {
    moveCamera(event);
}

int DisplayWindow::pickMarker() {
    makeCurrent();
    unsigned char pixelRead[3];
    glDrawBuffer(GL_BACK);
    glClearColor(0.0, 0.0 ,0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    for(auto marker : data->get1Vector(currentStep)) {
        glPointSize(3.0);
        glBegin(GL_POINTS);
            glColor3f(marker.getRedId() / 255.0, marker.getGreenId() / 255.0, marker.getBlueId() / 255.0);
            glVertex3f(marker.getX() / 1500, marker.getY() / 1500, marker.getZ() / 1500);
        glEnd();
    }
    glFlush();
    glReadPixels(mouseXStartPosition, height() - mouseYStartPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelRead);
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
    int index = pickMarker();
    // if the index is equal to -1 (the black background) or to the first index picked
    // then the array is reset
    if(index == -1 || index == linkedMarkersIndexes.last()[0]) {
        linkedMarkersIndexes.last()[0] = -1;
        lineBeingDrawn = false;
    }
    else {
        // if an index has already been picked
        if(linkedMarkersIndexes.last()[0] != -1) {
            linkedMarkersIndexes.last()[1] = index;
            // if these indexes are already linked, there is no need to put them again in the list
            if(alreadyLinkedMarkers(linkedMarkersIndexes.last())) {
                linkedMarkersIndexes.last()[0] = -1;
                linkedMarkersIndexes.last()[1] = -1;
            }
            // else we append a new array at the end of the list for the future link
            else {
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
        glColor3f(redId / 255.0, greenId / 255.0, blueId / 255.0);
        glVertex3f(data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getX() / 1500, data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getY() / 1500,
            data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[0])).getZ() / 1500);
            glVertex3f(data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getX() / 1500, data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getY() / 1500,
            data->get1Marker(currentStep,(linkedMarkersIndexes.at(i)[1])).getZ() / 1500);
        glEnd();
    }
    glFlush();
    glReadPixels(mouseXStartPosition, height() - mouseYStartPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelRead);
    update();
    return (int)pixelRead[0] + (int)pixelRead[1] *256 + (int)pixelRead[2] * 256 *256 - 1;
}

void DisplayWindow::removePickedLink() {
    int index = pickLink();
    if(index != -1) {
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
    update();
}

void DisplayWindow::swapMarkers() {
    int index = pickMarker();
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
    int zAngle = 0;
    int yAngle = 0;
    zAngle = (event->position().x() - mouseXStartPosition) / 4;
    yAngle = (mouseYStartPosition - event->position().x()) / 8;
    glRotatef(-yAngle, 0, 1, 0);
    glRotatef(yAngle, 1, 0, 0);

    glRotatef(zAngle, 0, 0, 1);
    mouseXStartPosition = event->position().x();
    mouseYStartPosition = event->position().y();
    update();
}

void DisplayWindow::resetCamera() {
    makeCurrent();
    setModelView();
    update();
}

void DisplayWindow::moveCameraToFrontSide() {
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    QMatrix4x4 view;
    view.lookAt(QVector3D(2, 2, 0.5), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0));
    float * tab = view.data();
    glMultMatrixf(tab);
    update();
}

void DisplayWindow::moveCameraToBackSide() {
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    QMatrix4x4 view;
    view.lookAt(QVector3D(-2, -2, 0.5), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0));
    float * tab = view.data();
    glMultMatrixf(tab);
    update();
}

void DisplayWindow::moveCameraToLeftSide() {
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    QMatrix4x4 view;
    view.lookAt(QVector3D(2, -2, 0.5), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0));
    float * tab = view.data();
    glMultMatrixf(tab);
    update();
}

void DisplayWindow::moveCameraToRightSide() {
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    QMatrix4x4 view;
    view.lookAt(QVector3D(-2, 2, 0.5), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0));
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
