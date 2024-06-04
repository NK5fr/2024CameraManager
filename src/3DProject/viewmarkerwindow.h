#ifndef VIEWMARKERWINDOW_H
#define VIEWMARKERWINDOW_H

#include "data.h"
#include <QWidget>
#include "QVBoxLayout"
class ViewMarkerWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ViewMarkerWindow(QWidget *parent = nullptr);
    void setData(const Data *pointerToData);
    void setCurrentStep(int newIndex);
private:
    QVBoxLayout *markerContainer;
    const Data *data;
    void clearContainer();
    QHBoxLayout* addMarker(Marker marker, int index);
    void populateContainer(int step);
signals:
    void markerPicked(int index);
private slots:
    void selectMarker();
    void removedPickedMarker(int);
};

#endif // VIEWMARKERWINDOW_H
