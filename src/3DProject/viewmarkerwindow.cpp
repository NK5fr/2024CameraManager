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
    QLabel *swapHeader = new QLabel("swap",this);
    QLabel *linkHeader = new QLabel("link",this);

    markerContainer->addWidget(numberHeader, 0, numberColumn, Qt::AlignCenter);
    markerContainer->addWidget(xCoordHeader, 0, xColumn, Qt::AlignCenter);
    markerContainer->addWidget(yCoordHeader, 0, yColumn, Qt::AlignCenter);
    markerContainer->addWidget(zCoordHeader, 0, zColumn, Qt::AlignCenter);
    markerContainer->addWidget(selectHeader, 0, selectColumn, Qt::AlignCenter);
    markerContainer->addWidget(swapHeader, 0, swapColumn, Qt::AlignCenter);
    markerContainer->addWidget(linkHeader, 0, linkColumn, Qt::AlignCenter);
}

ViewMarkerWindow::~ViewMarkerWindow() {
    // for (int rowIndex = 0; rowIndex < markerContainer->rowCount(); ++rowIndex) {
    //     for (int columnIndex = 0; columnIndex < markerContainer->columnCount(); ++columnIndex) {
    //         QLayoutItem* item = markerContainer->itemAtPosition(rowIndex,columnIndex);
    //         if (item != nullptr) {
    //             delete item;
    //         }
    //     }
    // }
    // delete markerContainer;
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
            selectButton->setObjectName(QString::number(i) + "-select");

            QPushButton *swapButton= new QPushButton("swap",this);
            swapButton->setObjectName(QString::number(i) + "-swap");

            QPushButton *linkButton= new QPushButton("link",this);
            linkButton->setObjectName(QString::number(i) + "-link");


            connect(selectButton, SIGNAL(clicked()), this, SLOT(selectMarker()));
            connect(swapButton, SIGNAL(clicked()), this, SLOT(swapMarkers()));
            connect(linkButton, SIGNAL(clicked()), this, SLOT(linkMarkers()));

            markerContainer->addWidget(numberLabel, i+1, numberColumn, Qt::AlignCenter);
            markerContainer->addWidget(xCoordLabel, i+1, xColumn, Qt::AlignCenter);
            markerContainer->addWidget(yCoordLabel, i+1, yColumn, Qt::AlignCenter);
            markerContainer->addWidget(zCoordLabel, i+1, zColumn, Qt::AlignCenter);
            markerContainer->addWidget(selectButton, i+1, selectColumn, Qt::AlignCenter);
            markerContainer->addWidget(swapButton, i+1, swapColumn, Qt::AlignCenter);
            markerContainer->addWidget(linkButton, i+1, linkColumn, Qt::AlignCenter);
        }
    }
}


int ViewMarkerWindow::getCorrectIndexFromButton(QPushButton* object)
{
    return object->objectName().first(1).toInt();
}

void ViewMarkerWindow::selectMarker() {
    QPushButton *senderButton = static_cast<QPushButton*>(sender());
    senderButton->setEnabled(false);
    emit markerPicked(getCorrectIndexFromButton(senderButton));
}

void ViewMarkerWindow::swapMarkers()
{
    QPushButton *senderButton = static_cast<QPushButton*>(sender());
    int index= getCorrectIndexFromButton(senderButton);
    emit swapMarker(index);
}

void ViewMarkerWindow::linkMarkers()
{
    QPushButton *senderButton = static_cast<QPushButton*>(sender());
    int index= getCorrectIndexFromButton(senderButton);
    if (alreadySelectedLink.isEmpty()) {
        qInfo() << alreadySelectedLink;
        senderButton->setEnabled(false);
        alreadySelectedLink = senderButton->objectName();
    } else {
        this->findChild<QPushButton*>(alreadySelectedLink)->setEnabled(true);
        alreadySelectedLink = QString();
    }
    emit linkMarker(index);
}
void ViewMarkerWindow::updateButtonColors(QPushButton* button, bool activated) {
    if (activated) {
        //button->setStyleSheet("background-color: red;");
        button->setText("activated");
    } else {
        //button->setStyleSheet("");
        button->setText("swap");
    }
}

void ViewMarkerWindow::updateSwapButtonColors() {
    for (int i= 0 ; i < data->get1Vector(0).size() ; i++) {
        QPushButton* button = this->findChild<QPushButton*>(QString::number(i)+"-swap");
        int *found =std::find(std::begin(selectedSwapMarkers), std::end(selectedSwapMarkers), i);
        if (found !=std::end(selectedSwapMarkers)) {
            button->setText("activated");
        } else {
            button->setText("swap");
        }
    }
}


/**
 * @brief ViewMarkerWindow::removedPickedMarker
 * This function is a SLOT that gets called when you click the "remove" button in coordinateWindow, it lets you be able to click the "select" button again.
 * @param index the index of the marker that got "un"-selected.
 */
void ViewMarkerWindow::removedPickedMarker(int index)
{
    this->findChild<QPushButton*>(QString::number(index)+"-select")->setEnabled(true);
}

void ViewMarkerWindow::addSwapMarker(int position, int index)
{
    this->selectedSwapMarkers[position] = index;
    updateSwapButtonColors();
}

void ViewMarkerWindow::removedSwapMarker(int position)
{
    this->selectedSwapMarkers[position] = -1;
    updateSwapButtonColors();
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
