#include "markercoordinateswidget.h"
#include <iostream>

MarkerCoordinatesWidget::MarkerCoordinatesWidget(int number, int color, QWidget *parent) : QFrame(parent)
{
    setFrameShadow(QFrame::Plain);
    markerNumber = new QLabel("Marker n°" + QString::number(number + 1), this);
    markerNumber->setMinimumWidth(70);
    markerNumber->setAlignment(Qt::AlignCenter);
    x = new QLabel("x = ", this);
    y = new QLabel("y = ", this);
    z = new QLabel("z = ", this);
    timeStep = new QLabel("Time step n° ",this);
    x->setMinimumWidth(60);
    x->setAlignment(Qt::AlignCenter);
    y->setMinimumWidth(60);
    y->setAlignment(Qt::AlignCenter);
    z->setMinimumWidth(60);
    z->setAlignment(Qt::AlignCenter);
    timeStep->setAlignment(Qt::AlignCenter);
    markerColor = new QLineEdit("", this);
    markerColor->setReadOnly(true);
    QPalette palette;
    palette.setColor(QPalette::Base, QColor(Qt::GlobalColor(color)));
    markerColor->setPalette(palette);
    layout = new QGridLayout(this);
    layout->setSpacing(0);
    setLayout(layout);
    layout->addWidget(timeStep, 0, 1);
    layout->addWidget(markerNumber, 1, 0);
    layout->addWidget(markerColor, 2, 0);
    layout->addWidget(x, 1, 1);
    layout->addWidget(y, 2, 1);
    layout->addWidget(z, 3, 1);
}

MarkerCoordinatesWidget::~MarkerCoordinatesWidget() {
    for(auto label : timeStepsLabels) {
        delete label;
    }
    delete markerColor;
    delete markerNumber;
    delete x;
    delete y;
    delete z;
    for(auto row : coordinatesColumns) {
        for(auto column : row) {
            delete column;
        }
    }
    delete layout;
}

void MarkerCoordinatesWidget::setStepCoordinates(int step, const Marker& marker) {
    coordinatesColumns[0][step]->setText(QString::number(marker.getX()));
    coordinatesColumns[1][step]->setText(QString::number(marker.getY()));
    coordinatesColumns[2][step]->setText(QString::number(marker.getZ()));
}

void MarkerCoordinatesWidget::addStepCoordinates() {
    for(int i = 0 ; i < 3 ; i++) {
        coordinatesColumns[i].append(new QLineEdit(this));
        coordinatesColumns[i].last()->setReadOnly(true);
        layout->addWidget(coordinatesColumns.at(i).last(), i + 1, coordinatesColumns.at(i).size() + 1);
    }
}

void MarkerCoordinatesWidget::removeStepCoordinates() {
    for(auto& row : coordinatesColumns) {
        layout->removeWidget(row.last());
        delete row.last();
        row.remove(row.size() - 1);
    }
}

int MarkerCoordinatesWidget::getSizeSteps() {
    return coordinatesColumns[0].size();
}

void MarkerCoordinatesWidget::setMarkerNumber(int number) {
    markerNumber->setText("Marker n°" + QString::number(number));
}

void MarkerCoordinatesWidget::setMarkerColor(int color) {
    QPalette palette;
    palette.setColor(QPalette::Base, QColor(Qt::GlobalColor(color)));
    markerColor->setPalette(palette);
}

void MarkerCoordinatesWidget::addStepLabel() {
    timeStepsLabels.append(new QLabel("", this));
    timeStepsLabels.last()->setAlignment(Qt::AlignCenter);
    layout->addWidget(timeStepsLabels.last(), 0, 1 + timeStepsLabels.size());
}

void MarkerCoordinatesWidget::removeStepLabel() {
    layout->removeWidget(timeStepsLabels.last());
    delete timeStepsLabels.last();
    timeStepsLabels.remove(timeStepsLabels.size() - 1);
}

void MarkerCoordinatesWidget::updateStepLabel(int index, int stepNumber) {
    timeStepsLabels.at(index)->setText(QString::number(stepNumber));
}

