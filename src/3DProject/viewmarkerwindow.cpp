#include "viewmarkerwindow.h"

#include "marker.h"
#include <QException>
#include <QLabel>
#include <QPushButton>

ViewMarkerWindow::ViewMarkerWindow(QWidget *parent)
    : QWidget{parent}
{
    markerContainer = new QVBoxLayout(this);
}
/**
 * @brief ViewMarkerWindow::setData
 * Simply sets the data attribute to whatever you put as an argument and then tried to populate the layour with the default data.
 * @param pointerToData pointer to the data you wish to load.
 */
void ViewMarkerWindow::setData(const Data *pointerToData) {
    data = pointerToData;
    populateContainer(0);
}

/**
 * @brief ViewMarkerWindow::populateContainer
 * This function makes a Layout filled with the data we need to display for every marker.
 * @param step the current frame the data is on
 */
void ViewMarkerWindow::populateContainer(int step) {
    clearContainer();
    QVector<Marker> listMarkers = data->get1Vector(step);
    for (int i = 0 ; i < listMarkers.size() ; i++) {
        QHBoxLayout *markerLayout = addMarker(listMarkers.at(i), i);
        markerContainer->addLayout(markerLayout);
    }
}
/**
 * @brief ViewMarkerWindow::clearContainer
 * This function empties markerContainer.
 */
void ViewMarkerWindow::clearContainer() {
    //QList<QHBoxLayout*> markerLayouts = this->layout()->findChildren<QHBoxLayout*>();
    QHBoxLayout *currentItem;
    while (currentItem = static_cast<QHBoxLayout*>(markerContainer->takeAt(0))) {
        QLayoutItem *currentNestedItem;
        while (currentNestedItem = currentItem->takeAt(0)) {
            delete currentNestedItem->widget();
            delete currentNestedItem;
        }
        delete currentItem->widget();
        delete currentItem;
    }
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
    QHBoxLayout *goodLayout = static_cast<QHBoxLayout*>(markerContainer->itemAt(index));
    for (int i = 0 ; i < goodLayout->count() ; i++) {
        QLayoutItem *item = goodLayout->itemAt(i);
        if (item->widget()->objectName() == QString::number(index)) {
            item->widget()->setEnabled(true);
        }
    }
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
    populateContainer(newIndex);
}
