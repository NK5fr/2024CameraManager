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

void ViewMarkerWindow::setData(const Data *pointerToData) {
    data = pointerToData;
    populateContainer(0);
}

void ViewMarkerWindow::populateContainer(int step) {
    clearContainer();
    QVector<Marker> listMarkers = data->get1Vector(step);
    for (int i = 0 ; i < listMarkers.size() ; i++) {
        QHBoxLayout *markerLayout = addMarker(listMarkers.at(i), i);
        markerContainer->addLayout(markerLayout);
    }
}

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

void ViewMarkerWindow::selectMarker() {
    QPushButton *senderButton = static_cast<QPushButton*>(sender());
    senderButton->setEnabled(false);
    int index = sender()->objectName().toInt();
    emit markerPicked(index);
}

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

QHBoxLayout* ViewMarkerWindow::addMarker(Marker marker, int index) {
    QHBoxLayout *markerLayout = new QHBoxLayout();
    // Add number
    QLabel *number = new QLabel(QString::number(index), this);

    // Add coordinates
    QLabel *coordX = new QLabel(QString::number(marker.getX()), this);
    QLabel *coordY = new QLabel(QString::number(marker.getY()), this);
    QLabel *coordZ = new QLabel(QString::number(marker.getZ()), this);

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

void ViewMarkerWindow::setCurrentStep(int newIndex)
{
    populateContainer(newIndex);
}
