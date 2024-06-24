#include "viewmarkerwindow.h"

#include "marker.h"
#include <QException>
#include <QLabel>
#include <QPushButton>
#include <qmenu.h>

ViewMarkerWindow::ViewMarkerWindow(QWidget *parent)
    : QWidget{parent}
{
    markerContainer = new QGridLayout(this);
    QLabel *numberHeader = new QLabel("number",this);
    QLabel *xCoordHeader = new QLabel("(X,",this);
    QLabel *yCoordHeader = new QLabel("Y,",this);
    QLabel *zCoordHeader = new QLabel("Z)",this);
    QLabel *selectHeader = new QLabel("selection",this);

    markerContainer->addWidget(numberHeader, 0, numberColumn, Qt::AlignCenter);
    markerContainer->addWidget(xCoordHeader, 0, xColumn, Qt::AlignCenter);
    markerContainer->addWidget(yCoordHeader, 0, yColumn, Qt::AlignCenter);
    markerContainer->addWidget(zCoordHeader, 0, zColumn, Qt::AlignCenter);
    markerContainer->addWidget(selectHeader, 0, selectColumn, Qt::AlignCenter);
}

ViewMarkerWindow::~ViewMarkerWindow() {
    for (int rowIndex = 0; rowIndex < markerContainer->rowCount(); ++rowIndex) {
        for (int columnIndex = 0; columnIndex < markerContainer->columnCount(); ++columnIndex) {
            QLayoutItem* item = markerContainer->itemAtPosition(rowIndex,columnIndex);
            if (item != nullptr) {
                delete item;
            }
        }
    }
    delete markerContainer;
}
/**
 * @brief ViewMarkerWindow::setData
 * Simply sets the data attribute to whatever you put as an argument and then tried to populate the layour with the default data.
 * @param pointerToData pointer to the data you wish to load.
 */
void ViewMarkerWindow::setData(const Data *pointerToData) {
    data = pointerToData;
    populateContainer();
    loadData(0);
}

void ViewMarkerWindow::loadData(int step) {
    QVector<Marker> currentMarkerVector = data->get1Vector(step);
    for (int i = 0 ; i < currentMarkerVector.size() ; i++) {
        Marker marker = currentMarkerVector.at(i);
        this->findChild<QLabel*>(QString::number(xColumn*10+i))->setText(QString::number(marker.getX()));
        this->findChild<QLabel*>(QString::number(yColumn*10+i))->setText(QString::number(marker.getY()));
        this->findChild<QLabel*>(QString::number(zColumn*10+i))->setText(QString::number(marker.getZ()));
    }
}

/**
 * @brief ViewMarkerWindow::populateContainer
 * This function makes a Layout filled with the data we need to display for every marker.
 * @param step the current frame the data is on
 */
void ViewMarkerWindow::populateContainer() {
    if (data != nullptr && data->getDataCoordinatesSize() > 0) {
        QVector<Marker> firstVector = data->get1Vector(0);
        for (int i = 0 ; i < firstVector.size() ; i++) {
            QLabel *numberLabel= new QLabel(QString::number(i),this);
            numberLabel->setObjectName(QString::number(numberColumn*10 + i));
            QLabel *xCoordLabel= new QLabel("X",this);
            xCoordLabel->setObjectName(QString::number(xColumn*10 + i));
            QLabel *yCoordLabel= new QLabel("Y",this);
            yCoordLabel->setObjectName(QString::number(yColumn*10 + i));
            QLabel *zCoordLabel= new QLabel("Z",this);
            zCoordLabel->setObjectName(QString::number(zColumn*10 + i));
            QPushButton *selectButton= new QPushButton("select",this);
            selectButton->setObjectName(QString::number(i));

            connect(selectButton, SIGNAL(clicked()), this, SLOT(selectMarker()));
            markerContainer->addWidget(numberLabel, i+1, numberColumn, Qt::AlignCenter);
            markerContainer->addWidget(xCoordLabel, i+1, xColumn, Qt::AlignCenter);
            markerContainer->addWidget(yCoordLabel, i+1, yColumn, Qt::AlignCenter);
            markerContainer->addWidget(zCoordLabel, i+1, zColumn, Qt::AlignCenter);
            markerContainer->addWidget(selectButton, i+1, selectColumn, Qt::AlignCenter);
        }
    }
}

void ViewMarkerWindow::updateContainer(int step) {
}
/**
 * @brief ViewMarkerWindow::selectMarker
 * This function sends a signal so that coordinateWindow so that it knows which marker has been selected.
 * It also locks the button that lets you select a marker
 */
void ViewMarkerWindow::selectMarker() {
    QPushButton *senderButton = static_cast<QPushButton*>(sender());
    senderButton->setEnabled(false);
    int index = sender()->objectName().toInt();
    emit markerPicked(index);
}

/**
 * @brief ViewMarkerWindow::removedPickedMarker
 * This function is a SLOT that gets called when you click the "remove" button in coordinateWindow, it lets you be able to click the "select" button again.
 * @param index the index of the marker that got "un"-selected.
 */
void ViewMarkerWindow::removedPickedMarker(int index)
{
    this->findChild<QPushButton*>(QString::number(index))->setEnabled(true);
}

/**
 * @brief ViewMarkerWindow::addMarker
 * This function creates a Layout containing the name, coordinates and buttons associated with a marker.
 * @param marker the instance of the selected marker, it contains the necessary data.
 * @param index the index of the marker
 * @return an instance of QHBoxLayout that's then added to markerContainer.
 */
QHBoxLayout* ViewMarkerWindow::addMarker(Marker marker, int index) {
    QHBoxLayout *markerLayout = new QHBoxLayout();
    // Add number
    QLabel *number = new QLabel(QString::number(index), this);

    // Add coordinates
    QLabel *coordX = new QLabel(QString::number(marker.getX()), this);
    coordX->setObjectName(QString("coordX"));
    QLabel *coordY = new QLabel(QString::number(marker.getY()), this);
    coordY->setObjectName(QString("coordY"));
    QLabel *coordZ = new QLabel(QString::number(marker.getZ()), this);
    coordZ->setObjectName(QString("coordZ"));

    // Add ActionButtons
    QPushButton *select = new QPushButton("select", this);
    select->setObjectName(QString::number(index));


    // Add to layout
    markerLayout->addWidget(number);
    markerLayout->addWidget(coordX);
    markerLayout->addWidget(coordY);
    markerLayout->addWidget(coordZ);
    markerLayout->addWidget(select);

    connect(select, SIGNAL(clicked()), this, SLOT(selectMarker()));
    return markerLayout;
}
/**
 * @brief ViewMarkerWindow::setCurrentStep
 * Simply changes what markerContainer contains.
 * @param newIndex the new current index of the data we're on.
 */
void ViewMarkerWindow::setCurrentStep(int newIndex)
{
    loadData(newIndex);
}
