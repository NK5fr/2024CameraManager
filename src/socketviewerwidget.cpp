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
    QStringList rowList = lineList[0].split(" \t");
    columnsNumber = rowList.size() - 1;
    if (columnsNumber % 3 != 0) printf("Not correct number for tracked points!\n");
    int numPoints = columnsNumber / 3;
    pointData.resize(linesNumber);
    for (int row = 0; row < linesNumber; row++) {
        if (lineList[row].isEmpty()) continue;
        rowList = lineList[row].split(" ");
        pointData[row].resize(numPoints);
        for (int col = 0; col < numPoints; col++) {
            pointData[row][col].x = rowList[(col * 3) + 0].toDouble();
            pointData[row][col].y = rowList[(col * 3) + 2].toDouble();
            pointData[row][col].z = rowList[(col * 3) + 1].toDouble();
        }
    }
}

/* Right click on the main widget of the viewer */
void SocketViewerWidget::onRightClick() {
    /* Creating a menu with allowed actions */
    QMenu *menu = new QMenu();
    menu->addAction("Raw View");
    menu->addAction("Table View");
    menu->addAction("3D View");
    if (view == 1) {
        menu->addSeparator();
        QString degree = QString::fromUtf8("\u00b0");
        QString string;
        for (int i = 0; i < showPoints.size(); i++){
            string = QString("Point n" + degree + "%1").arg(i);
            QAction *action = new QAction(string, menu);
            action->setCheckable(true);
            action->setChecked(showPoints[i]);
            menu->addAction(action);
        }
    }
    menu->popup(cursor().pos());
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuProjectAction_triggered(QAction*)));
}

/* Click on a menu item */
void SocketViewerWidget::menuProjectAction_triggered(QAction* action) {
    if (action->text() == "Raw View") {
        showTextView();
    } else if (action->text() == "Table View") {
        showTableView();
    } else if (action->text() == "3D View") {
        show3DView();
    }
    /*
    if (action->text() == "Change view"){
        // Checking view ID and file type to display the right view of the file
        if (view == 0){
            showTableView();
            view++;
        } else if (view == 1){
            view++;
        } else if (view == 2){
            showTextView();
            view = 0;
        }
    } else {
        // It is a point number which has been triggered
        // Getting the point number
        QString degree = QString::fromUtf8("\u00b0"); // Grabbing the degree symbol
        QStringList part = action->text().split(degree);
        // Changing its bool value
        showPoints.at(part.at(1).toInt()) = !showPoints.at(part.at(1).toInt());
        showTableView();
    }
    */
}

/** VIEWS */
/* Show files as text view */
void SocketViewerWidget::showTextView() {
    setFocusPolicy(Qt::NoFocus);
    setWidget(&fileContain);

    /* Initializing hide points */
    showPoints.resize(columnsNumber / 3);
    for (int i = 0; i < showPoints.size(); i++) {
        showPoints[i] = true;
    }
}

/* Show 3D datas in a table */
void SocketViewerWidget::showTableView() {
    QVBoxLayout *timeLayout = new QVBoxLayout();
    QVBoxLayout *timeLayoutBis = new QVBoxLayout();

    /* Initializing minMax array */
    /*
    minMax.resize(6);
    minMax[0] = 0; // Min x
    minMax[1] = 0; // Min y
    minMax[2] = 0; // Min z
    minMax[3] = 0; // Max x
    minMax[4] = 0; // Max y
    minMax[5] = 0; // Max z
    */

    // Initializing vector < QGridLayout > coordinateLayout
    coordinatesLayout.resize(columnsNumber / 3);
    for (int i = 0; i < coordinatesLayout.size(); i++) {
        coordinatesLayout.at(i) = new QGridLayout();
    }
    /*
    // Reading file
    // list.size()-1 because last line is always empty
    for (int lig = 0; lig < linesNumber; lig++){
        // list.size()-1 because last column is always empty
        for (int i = 0; i < columnsNumber / 3; i++){
            // If Point n°i is shown
            QString coordinateValue(list.at(i));
            if (showPoints[i / 3]) {
                // Coordinate
                QString coordinateValue(list.at(i));
                coordinateValue.replace("\t", "");
                CoordinatesLabel* coordinatesLabel = new CoordinatesLabel(this, coordinateValue + "  ");
                coordinatesLabel->setAutoFillBackground(true);
                coordinatesLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                coordinatesLabel->setMouseTracking(true);
                coordinatesLayout.at(i / 3)->addWidget(coordinatesLabel, lineNumber, i % 3, 1, 1);

                if (i % 3 == 0) {
                    coordinatesLayout.at(i / 3)->addWidget(new QLabel("    "), lineNumber, i % 3 + 3, 1, 1);
                }
            }
            pointData[lineNumber][i] = coordinateValue.toFloat();

            // Updating min and max coordinates
            // i%3==0 means x coordinate
            if (i % 3 == 0 && minMax[0] > pointData[lineNumber][i]) {
                minMax[0] = pointData[lineNumber][i];
            }
            if (i % 3 == 0 && minMax[3] < pointData[lineNumber][i]) {
                minMax[3] = pointData[lineNumber][i];
            }

            // i%3==1 means y coordinate
            if (i % 3 == 1 && minMax[1] > pointData[lineNumber][i]) {
                minMax[1] = pointData[lineNumber][i];
            }
            if (i % 3 == 1 && minMax[4] < pointData[lineNumber][i]) {
                minMax[4] = pointData[lineNumber][i];
            }

            // i%3==2 means z coordinate
            if (i % 3 == 2 && minMax[2] > pointData[lineNumber][i]) {
                minMax[2] = pointData[lineNumber][i];
            }
            if (i % 3 == 2 && minMax[5] < pointData[lineNumber][i]) {
                minMax[5] = pointData[lineNumber][i];
            }
            // Separate points coordinates by adding somes spaces
        }
        // Time
        QString time = QString("%1").arg(lineNumber);
        timeLayout->addWidget(new QLabel(time));
        timeLayoutBis->addWidget(new QLabel(time));
        lineNumber++;
    }
    */
    /* Adding space for the scroll bar to be synchronized
     * with specified size to be sure all scrolls have exactly the same size */
    QLabel *emptyLabel = new QLabel("  ");
    QLabel *emptyLabelBis = new QLabel("  ");
    QFont font = emptyLabel->font();
    font.setPixelSize(1);
    emptyLabel->setFont(font);
    emptyLabelBis->setFont(font);

    timeLayout->addWidget(emptyLabel);
    timeLayoutBis->addWidget(emptyLabelBis);

    /* Time scroll areas
     * There are two because unable to add twice a widget to another widget
     * Same problem for layout even if they are set to different widgets */
    QWidget *timeWidget = new QWidget();
    timeWidget->setLayout(timeLayout);
    timeArea = new QScrollArea();
    timeArea->setWidget(timeWidget);
    timeArea->setMaximumWidth(65);

    QWidget *timeWidgetBis = new QWidget();
    timeWidgetBis->setLayout(timeLayoutBis);
    timeAreaBis = new QScrollArea();
    timeAreaBis->setWidget(timeWidgetBis);
    timeAreaBis->setMaximumWidth(65);

    vector <QWidget *> widgets;
    widgets.resize(coordinatesLayout.size());

    for (int i = 0; i < widgets.size(); i++) {
        widgets.at(i) = new QWidget();
        widgets.at(i)->setLayout(coordinatesLayout.at(i));
        if (i % 2) widgets.at(i)->setStyleSheet("QWidget { background-color : lightgray }");
        else widgets.at(i)->setStyleSheet("QWidget { background-color : white }");
    }

    /* Coordinates scroll area */
    QHBoxLayout* coordinatesHLayout = new QHBoxLayout();
    for (int i = 0; i < widgets.size(); i++) {
        coordinatesHLayout->addWidget(widgets.at(i));
    }
    QWidget *coordinatesWidget = new QWidget();
    coordinatesWidget->setLayout(coordinatesHLayout);
    coordinatesArea = new QScrollArea();
    coordinatesArea->setWidget(coordinatesWidget);
    coordinatesArea->verticalScrollBar()->hide();

    /* Main Widget and main window */
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(timeArea);
    mainLayout->addWidget(coordinatesArea);
    mainLayout->addWidget(timeAreaBis);

    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(mainLayout);
    mainWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    /* Setting main widget, and connecting signal/slot */
    setWidget(mainWidget);

    connect(mainWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClick()));
    connect(timeArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(areaBarsMoved(int)));
    connect(coordinatesArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(areaBarsMoved(int)));
    connect(timeAreaBis->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(areaBarsMoved(int)));
}

/* Show 3D view of coordinates */
void SocketViewerWidget::show3DView() {
    setFocusPolicy(Qt::TabFocus);
    /* Initializing coordinatesShown, which is the int
     * to know which time has to be displayed */
    coordinatesShown = 0;

    /* Line Edit and Slider to choose time to show */
    spinBox = new QSpinBox();
    spinBox->setMinimum(0);
    spinBox->setMaximum(linesNumber - 1);
    spinBox->setValue(coordinatesShown);

    slider = new QSlider(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(10);
    slider->setMinimum(0);
    slider->setMaximum(linesNumber - 1);
    slider->setMaximumWidth(200);
    slider->setValue(coordinatesShown);

    /* Menu buttons to change time to display */
    /*QPushButton *play = new QPushButton("Play");*/

    QHBoxLayout *menuLayout = new QHBoxLayout();
    menuLayout->addWidget(spinBox);
    menuLayout->addWidget(slider);
    /*menuLayout->addWidget(play);*/

    QWidget *menuWidget = new QWidget();
    menuWidget->setLayout(menuLayout);
    menuWidget->setMaximumHeight(minimumHeight() + 50);
    menuWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    /* WidgetGL */
    widgetGL = new WidgetGL(&pointData, minMax, this, calibrationPath);

    /* Main layout, and main widget */
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(widgetGL);
    layout->addWidget(menuWidget);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setWidget(widget);

    /* Show maximized because there is no interest about
     * showing 3D coordinates in a small window/widget */
    //showMaximized();

    /*connect(play, SIGNAL(clicked()),
            this, SLOT(liveView()));*/

    connect(menuWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClick()));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

/* Make all the sroll bars moving at the same time
 * with the same value */
void SocketViewerWidget::areaBarsMoved(int newValue) {
    timeArea->verticalScrollBar()->setValue(newValue);
    coordinatesArea->verticalScrollBar()->setValue(newValue);
    timeAreaBis->verticalScrollBar()->setValue(newValue);
}

/* Display a tooltip the show the point number, the axis, the value and the time */
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
    /* Creating the QString to de displayed */
    QString degreeSign = QString::fromUtf8("\u00b0");
    QString pointNb = QString("%1").arg(indLayout);
    QString time = QString("%1").arg(row);
    QString string = QString("Point n" + degreeSign + pointNb + "\n"
                             + axis + " : " + label->text() + "\n" +
                             "t : " + time);
    QToolTip::showText(cursor().pos(), string, label);
}

/* time changed into the 3D view */
void SocketViewerWidget::valueChanged(int value) {
    coordinatesShown = value;
    slider->setValue(coordinatesShown);
    spinBox->setValue(coordinatesShown);
    widgetGL->showView(coordinatesShown);
}

QSlider *SocketViewerWidget::getTimeSlider() {
    return slider;
}
