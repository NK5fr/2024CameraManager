#include <iostream>
#include "coordinateswindow.h"

CoordinatesWindow::CoordinatesWindow(QWidget *parent) : QWidget(parent)
{
    layout = new QGridLayout(this);
    setWindowTitle("marker coordinates window");
    currentStep = 0;

    QLabel *subTitleCoordinates = new QLabel("marker coordinates", this);
    subTitleCoordinates->setMinimumHeight(20);
    QLabel *headerColor = new QLabel("color", this);
    QLabel *headerX = new QLabel("x", this);
    QLabel *headerY = new QLabel("y", this);
    QLabel *headerZ = new QLabel("z", this);

    layout->addWidget(subTitleCoordinates, 2, 0, 1, 6, Qt::AlignCenter);
    layout->addWidget(headerColor, 3, 1, Qt::AlignCenter);
    layout->addWidget(headerX, 3, 2, Qt::AlignCenter);
    layout->addWidget(headerY, 3, 3, Qt::AlignCenter);
    layout->addWidget(headerZ, 3, 4, Qt::AlignCenter);
    layout->setSpacing(0.0);

    this->setLayout(layout);
}

void CoordinatesWindow::addLineCoordinates(int index, int color) {
    selectedMarkersIndexes.append(index);
    int row = layout->rowCount();
    xyzVector.append(QVector<QLineEdit*>());
    //QPalette is used to put a color in the QLineEdit
    QPalette palette;
    labelVector.append(new QLabel("selected marker number " + QString::number(xyzVector.size()), this));
    labelVector.last()->setMinimumWidth(140);
    layout->addWidget(labelVector.last(), row, 0);
    for(int i = 1 ; i < 5 ; i++) {
        xyzVector.last().append(new QLineEdit(this));
        xyzVector.last().last()->setReadOnly(true);
        layout->addWidget(xyzVector.last().last(), row, i);
    }
    palette.setColor(QPalette::Base, QColor(Qt::GlobalColor(color)));

    xyzVector.last().at(0)->setPalette(palette);
    xyzVector.last().at(1)->setText(QString::number(data->get1Marker(currentStep, index).getX()));
    xyzVector.last().at(2)->setText(QString::number(data->get1Marker(currentStep, index).getY()));
    xyzVector.last().at(3)->setText(QString::number(data->get1Marker(currentStep, index).getZ()));
    //colorIndex++;
    QPushButton *button = new QPushButton("remove", this);
    button->setObjectName(QString::number(index));
    buttonVector.append(button);
    connect(buttonVector.last(), SIGNAL(clicked(bool)), this, SLOT(removeLineCoordinates()));
    layout->addWidget(buttonVector.last(), row, 5);
}

void CoordinatesWindow::removeLineCoordinates(int i) {
    // this removeLineCoordinates already knows which marker it wants to remove, so it needs to
    if (selectedMarkersIndexes.size() > i) {
        selectedMarkersIndexes.remove(i);
        emit lineRemoved(i);
        layout->removeWidget(labelVector.at(i));
        delete labelVector.at(i);
        labelVector.remove(i);
        updateLabelNumber(i);
        for(auto textField : xyzVector.at(i)) {
            layout->removeWidget(textField);
            delete textField;
        }
        xyzVector.remove(i);
        layout->removeWidget(buttonVector.at(i));
        delete buttonVector.at(i);
        buttonVector.remove(i);
    }

}

void CoordinatesWindow::setData(const Data *pointerToData) {
    data = pointerToData;
}

void CoordinatesWindow::setCurrentStep(int step) {
    currentStep = step;
    updateCoordinates();
}

void CoordinatesWindow::updateCoordinates() {
    int i = 0;
    for(auto row : xyzVector) {
        row.at(1)->setText(QString::number(data->get1Marker(currentStep, selectedMarkersIndexes.at(i)).getX()));
        row.at(2)->setText(QString::number(data->get1Marker(currentStep, selectedMarkersIndexes.at(i)).getY()));
        row.at(3)->setText(QString::number(data->get1Marker(currentStep, selectedMarkersIndexes.at(i)).getZ()));
        i++;
    }
}

void CoordinatesWindow::removeLineCoordinates() {
    // the index of the selected marker is stored in the sender's objectName
    int i = selectedMarkersIndexes.indexOf(sender()->objectName().toInt());
    // once we have found the index we can remove all the widget of the line that corresponds to this index
    selectedMarkersIndexes.remove(i);
    emit lineRemoved(i);
    emit removedMarker(sender()->objectName().toInt());
    layout->removeWidget(labelVector.at(i));
    delete labelVector.at(i);
    labelVector.remove(i);
    updateLabelNumber(i);
    for(auto textField : xyzVector.at(i)) {
        layout->removeWidget(textField);
        delete textField;
    }
    xyzVector.remove(i);
    layout->removeWidget(buttonVector.at(i));
    delete buttonVector.at(i);
    buttonVector.remove(i);
}

void CoordinatesWindow::updateLabelNumber(int index) {
    // When a line is removed, the number of the selected markers must be updated
    while(index < labelVector.size()) {
        labelVector.at(index)->setText("selected marker number " + QString::number(index + 1));
        index++;
    }
}

void CoordinatesWindow::swapCoordinates(const std::array<int, 2>& markersToBeSwaped) {
    // a temporary array used to store the coordinates of the first marker to be swapped
    std::array<QString, 3> temp({xyzVector.at(markersToBeSwaped.at(0)).at(1)->text(), xyzVector.at(markersToBeSwaped.at(0)).at(2)->text(),
                                 xyzVector.at(markersToBeSwaped.at(0)).at(3)->text()});
    for(int i = 0 ; i < 3 ; i++) {
        xyzVector.at(markersToBeSwaped.at(0)).at(i + 1)->setText(xyzVector.at(markersToBeSwaped.at(1)).at(i + 1)->text());
        xyzVector.at(markersToBeSwaped.at(1)).at(i + 1)->setText(temp.at(i));
    }

}

