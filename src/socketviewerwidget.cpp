#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QLabel>
#include <QTextStream>
#include <QVBoxLayout>
#include <QFile>
#include <QString>
#include <QLineEdit>
#include <QFont>
#include <QtOpenGL/QtOpenGL>

#include <string.h>
#include <iostream>

#include "socketviewerwidget.h"

using namespace std;

/* Constructor */
SocketViewerWidget::SocketViewerWidget(QString path, QString n, QString calibPath)
    : name(n), fullPath(path + "/" + name), tmpPath(path), calibrationPath(calibPath), view(0), linesNumber(0), columnsNumber(0) {
    /* Creating QTextEdit, which need to be known to save file later if asked */
    //fileContain = new QTextEdit();
    readTextFromFile();
    extractDataFromText();
    fileContain.setReadOnly(true);
    fileContain.setContextMenuPolicy(Qt::CustomContextMenu);
    fileContain.setText(fullText);
    fileContain.setWindowTitle(name);
    fileContain.setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
    coordinatesShown = 0;
    hideButtonPanel = true;

    // WidgetGL
    widgetGL = new WidgetGL(&pointData, minMax, this, calibrationPath);

    init();
    show3DView();

    connect(&fileContain, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClick()));
    setWindowTitle(name);
}

void SocketViewerWidget::readTextFromFile() {
    QFile myFile(fullPath);
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    fullText = QString(myFile.readAll());
    myFile.close();
}

void SocketViewerWidget::extractDataFromText() {
    QStringList lineList = fullText.split("\n");
    linesNumber = lineList.size();

    //QRegularExpression coordsRegExr("(-?\\d+\.?\\d*)(?:\\s+)(-?\\d+\\.?\\d*)(?:\\s+)(-?\\d+\\.?\\d*)"); // TODO Change over to use regular expressions...
    
    QStringList rowList = lineList[0].split(" \t");
    columnsNumber = rowList.size() - 1;
    if (columnsNumber % 3 != 0) printf("Not correct number for tracked points!\n");
    int numPoints = columnsNumber / 3;
    //pointData.resize(linesNumber);
    int rowNumberLocal = 0;
    for (int row = 0; row < linesNumber; row++) {
        if (lineList[row].isEmpty()) continue;
        rowList = lineList[row].split(" ");
        vector<Vector3d> points;
        points.resize(numPoints);
        //pointData[row].resize(numPoints);
        for (int col = 0; col < numPoints; col++) {
            points[col].x = rowList[(col * 3) + 0].toDouble();
            points[col].y = rowList[(col * 3) + 2].toDouble();
            points[col].z = rowList[(col * 3) + 1].toDouble();
        }
        pointData.push_back(points);
        rowNumberLocal++;
    }
    rowNumber = rowNumberLocal;
}

// Right click on the main widget of the viewer
void SocketViewerWidget::onRightClick() {
    // Creating a menu with allowed actions
    QMenu *menu = new QMenu();
    menu->addAction("3D View");
    menu->popup(cursor().pos());
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuProjectAction_triggered(QAction*)));
}

/* Click on a menu item */
void SocketViewerWidget::menuProjectAction_triggered(QAction* action) {
    if (action->text() == "3D View") {
        show3DView();
    }
}

/* Show files as text view */
void SocketViewerWidget::showTextView() {
    setFocusPolicy(Qt::NoFocus);
    setWidget(&fileContain);
}

void SocketViewerWidget::init() {
    // Line Edit and Slider to choose time to show
    spinBox = new QSpinBox();
    spinBox->setMinimum(0);
    spinBox->setMaximum(rowNumber - 1);
    spinBox->setMinimumWidth(200);
    spinBox->setValue(coordinatesShown);

    slider = new QSlider(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(10);
    slider->setMinimum(0);
    slider->setMaximum(rowNumber - 1);
    //slider->setMaximumWidth(200);
    slider->setValue(coordinatesShown);

    QHBoxLayout* menuLayout = new QHBoxLayout();
    menuLayout->addWidget(slider);
    menuLayout->addWidget(spinBox);

    showPreceedingPoints = new QCheckBox("Show Preceeding Points");
    showPreceedingPoints->setCheckState((widgetGL->isShowPreceedingPoints()) ? Qt::Checked : Qt::Unchecked);
    showLines = new QCheckBox("Show Lines");
    showLines->setCheckState((widgetGL->isShowLines()) ? Qt::Checked : Qt::Unchecked);
    showCameras = new QCheckBox("Show Cameras");
    showCameras->setCheckState((widgetGL->isShowCameras()) ? Qt::Checked : Qt::Unchecked);
    showCameraLegs = new QCheckBox("Show Camera Tripod");
    showCameraLegs->setCheckState((widgetGL->isShowCameraLegs()) ? Qt::Checked : Qt::Unchecked);
    showCameraLegs->setEnabled(widgetGL->isShowCameras());
    showCameraFOV = new QCheckBox("Show Camera FOV");
    showCameraFOV->setCheckState((widgetGL->isShowFovCone()) ? Qt::Checked : Qt::Unchecked);
    showCameraFOV->setEnabled(widgetGL->isShowCameras());
    showCoordinateAxis = new QCheckBox("Show Coordinate Axis");
    showCoordinateAxis->setCheckState((widgetGL->isShowCoordinateSystem()) ? Qt::Checked : Qt::Unchecked);
    showFloor = new QCheckBox("Show Floor");
    showFloor->setCheckState((widgetGL->isShowFloorLines()) ? Qt::Checked : Qt::Unchecked);
    showOrtho = new QCheckBox("Show Orthographic Projection");
    showOrtho->setCheckState((widgetGL->isShowOrtho()) ? Qt::Checked : Qt::Unchecked);

    QVBoxLayout* checkBoxLayout1 = new QVBoxLayout();
    checkBoxLayout1->addWidget(showPreceedingPoints);
    checkBoxLayout1->addWidget(showLines);
    checkBoxLayout1->addWidget(showFloor);
    checkBoxLayout1->addWidget(showCoordinateAxis);
    checkBoxLayout1->setAlignment(Qt::AlignRight);
    //checkBoxLayout1

    QVBoxLayout* checkBoxLayout2 = new QVBoxLayout();
    checkBoxLayout2->addWidget(showCameras);
    checkBoxLayout2->addWidget(showCameraLegs);
    checkBoxLayout2->addWidget(showCameraFOV);
    checkBoxLayout2->addWidget(showOrtho);
    checkBoxLayout2->setAlignment(Qt::AlignRight);
    //checkBoxLayout2->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

    showXYPlane = new QPushButton("Show XY-plane");
    showXZPlane = new QPushButton("Show XZ-plane");
    showYZPlane = new QPushButton("Show YZ-plane");
    showSocketText = new QPushButton("Show Socketfile");
    showSocketText->setMinimumHeight(50);

    camDistanceSlider = new QSlider(Qt::Orientation::Horizontal);
    camDistanceSlider->setMaximum(20000);
    camDistanceSlider->setMinimum(200);
    camDistanceSlider->setSliderPosition((int) widgetGL->getCamDistance());
    camDistanceSlider->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
    fovConeSizeSlider = new QSlider(Qt::Orientation::Horizontal);
    fovConeSizeSlider->setEnabled(widgetGL->isShowFovCone() && widgetGL->isShowCameras());
    fovConeSizeSlider->setMaximum(4000);
    fovConeSizeSlider->setMinimum(200);
    fovConeSizeSlider->setSliderPosition((int) widgetGL->getFOVConeSize());
    //fovConeSizeSlider->setMaximumSize(QSize(400, 100));
    fovConeSizeSlider->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

    QGridLayout* sliderLayout = new QGridLayout();
    sliderLayout->addWidget(new QLabel("Camera Distance:"), 0, 0);
    sliderLayout->addWidget(new QLabel("FOV Camera-cone:"), 1, 0);
    sliderLayout->addWidget(camDistanceSlider, 0, 1);
    sliderLayout->addWidget(fovConeSizeSlider, 1, 1);

    QVBoxLayout* showPlaneButtonLayout = new QVBoxLayout();
    showPlaneButtonLayout->addWidget(showXYPlane);
    showPlaneButtonLayout->addWidget(showXZPlane);
    showPlaneButtonLayout->addWidget(showYZPlane);
    showPlaneButtonLayout->addLayout(sliderLayout);

    QHBoxLayout* buttonCheckBoxLayout = new QHBoxLayout();
    buttonCheckBoxLayout->addLayout(showPlaneButtonLayout);
    buttonCheckBoxLayout->addLayout(checkBoxLayout1);
    buttonCheckBoxLayout->addLayout(checkBoxLayout2);
    buttonCheckBoxLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* combinedLayout = new QVBoxLayout();
    //combinedLayout->addLayout(menuLayout);
    combinedLayout->addLayout(buttonCheckBoxLayout);
    combinedLayout->addWidget(showSocketText);
    combinedLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    buttonPanel = new QWidget();
    buttonPanel->setLayout(combinedLayout);
    buttonPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    buttonPanel->setContextMenuPolicy(Qt::CustomContextMenu);
    //buttonPanel->setContentsMargins(QMargins(0, 0, 0, 0));

    hideButton = new QPushButton("Hide/Show Button-Panel");

    // Main layout, and main widget
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(widgetGL);
    layout->addLayout(menuLayout);
    layout->addWidget(hideButton);
    layout->addWidget(buttonPanel);

    hideButtonPanelFunc();

    widgetGLWindow = new QWidget();
    widgetGLWindow->setLayout(layout);

    connect(showPreceedingPoints, SIGNAL(stateChanged(int)), this, SLOT(showPreceedingPointsStateChanged(int)));
    connect(showLines, SIGNAL(stateChanged(int)), this, SLOT(showLinesStateChanged(int)));
    connect(showCameras, SIGNAL(stateChanged(int)), this, SLOT(showCamerasStateChanged(int)));
    connect(showCameraLegs, SIGNAL(stateChanged(int)), this, SLOT(showCameraLegsStateChanged(int)));
    connect(showCameraFOV, SIGNAL(stateChanged(int)), this, SLOT(showCameraFOVStateChanged(int)));
    connect(showCoordinateAxis, SIGNAL(stateChanged(int)), this, SLOT(showCoordinateAxisStateChanged(int)));
    connect(showFloor, SIGNAL(stateChanged(int)), this, SLOT(showFloorStateChanged(int)));
    connect(showOrtho, SIGNAL(stateChanged(int)), this, SLOT(showOrthoStateChanged(int)));
    connect(showSocketText, SIGNAL(clicked()), this, SLOT(showSocketTextFunc()));
    connect(showXYPlane, SIGNAL(clicked()), this, SLOT(showXYPlaneFunc()));
    connect(showXZPlane, SIGNAL(clicked()), this, SLOT(showXZPlaneFunc()));
    connect(showYZPlane, SIGNAL(clicked()), this, SLOT(showYZPlaneFunc()));
    connect(hideButton, SIGNAL(clicked()), this, SLOT(hideButtonPanelFunc()));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(camDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(camDistanceValueChanged(int)));
    connect(fovConeSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(fovConeSizeValueChanged(int)));
}

// Show 3D view of coordinates
void SocketViewerWidget::show3DView() {
    setFocusPolicy(Qt::TabFocus);
    // Initializing coordinatesShown, which is the int to know which time has to be displayed
    coordinatesShown = 0;
    setWidget(widgetGLWindow);
}

// Make all the sroll bars moving at the same time
// with the same value
void SocketViewerWidget::areaBarsMoved(int newValue) {
    timeArea->verticalScrollBar()->setValue(newValue);
    coordinatesArea->verticalScrollBar()->setValue(newValue);
    timeAreaBis->verticalScrollBar()->setValue(newValue);
}

// Display a tooltip the show the point number, the axis, the value and the time
void SocketViewerWidget::displayToolTip(CoordinatesLabel *label) {
    int layoutIndex = -1, indLayout = -1;
    while (layoutIndex == -1){
        indLayout++;
        layoutIndex = coordinatesLayout.at(indLayout)->indexOf(label);
    }
    int row, column, rowSpan, columnSpan;
    coordinatesLayout.at(indLayout)->getItemPosition(layoutIndex, &row, &column, &rowSpan, &columnSpan);
    QString axis;
    if (column % 3 == 0) axis = "x";
    else if (column % 3 == 1) axis = "y";
    else if (column % 3 == 2) axis = "z";
    // Creating the QString to de displayed
    QString degreeSign = QString::fromUtf8("\u00b0");
    QString pointNb = QString("%1").arg(indLayout);
    QString time = QString("%1").arg(row);
    QString string = QString("Point n" + degreeSign + pointNb + "\n"
                             + axis + " : " + label->text() + "\n" +
                             "t : " + time);
    QToolTip::showText(cursor().pos(), string, label);
}

// time changed into the 3D view
void SocketViewerWidget::valueChanged(int value) {
    coordinatesShown = value;
    slider->setValue(coordinatesShown);
    spinBox->setValue(coordinatesShown);
    widgetGL->showView(coordinatesShown);
}

QSlider *SocketViewerWidget::getTimeSlider() {
    return slider;
}

void SocketViewerWidget::showXYPlaneFunc() {
    widgetGL->showXYPlane();
}

void SocketViewerWidget::showXZPlaneFunc() {
    widgetGL->showXZPlane();
}

void SocketViewerWidget::showYZPlaneFunc() {
    widgetGL->showYZPlane();
}

void SocketViewerWidget::showSocketTextFunc() {
    showTextView();
}

void SocketViewerWidget::hideButtonPanelFunc() {
    if (hideButtonPanel) buttonPanel->hide();
    else buttonPanel->show();
    hideButtonPanel = !hideButtonPanel;
}

void SocketViewerWidget::showPreceedingPointsStateChanged(int state) {
    widgetGL->setShowPreceedingPoints(state == Qt::Checked);
    widgetGL->updateGL();
}

void SocketViewerWidget::showLinesStateChanged(int state) {
    widgetGL->setShowLines(state == Qt::Checked);
    widgetGL->updateGL();
}

void SocketViewerWidget::showCamerasStateChanged(int state) {
    widgetGL->setShowCameras(state == Qt::Checked);
    showCameraFOV->setEnabled(widgetGL->isShowCameras());
    showCameraLegs->setEnabled(widgetGL->isShowCameras());
    fovConeSizeSlider->setEnabled(widgetGL->isShowFovCone() && state == Qt::Checked);
    widgetGL->updateGL();
}

void SocketViewerWidget::showCameraLegsStateChanged(int state) {
    widgetGL->setShowCameraLegs(state == Qt::Checked);
    widgetGL->updateGL();
}

void SocketViewerWidget::showCameraFOVStateChanged(int state) {
    widgetGL->setShowFovCone(state == Qt::Checked);
    fovConeSizeSlider->setEnabled(state == Qt::Checked);
    widgetGL->updateGL();
}

void SocketViewerWidget::showCoordinateAxisStateChanged(int state) {
    widgetGL->setShowCoordinateSystem(state == Qt::Checked);
    widgetGL->updateGL();
}

void SocketViewerWidget::showFloorStateChanged(int state) {
    widgetGL->setShowFloorLines(state == Qt::Checked);
    widgetGL->updateGL();
}

void SocketViewerWidget::showOrthoStateChanged(int state) {
    widgetGL->setShowOrtho(state == Qt::Checked);
    widgetGL->updateGL();
}
void SocketViewerWidget::camDistanceValueChanged(int value) {
    widgetGL->setCamDistance((double) value);
    widgetGL->updateGL();
}

void SocketViewerWidget::fovConeSizeValueChanged(int value) {
    widgetGL->setFOVConeSize((double) value);
    widgetGL->updateGL();
}