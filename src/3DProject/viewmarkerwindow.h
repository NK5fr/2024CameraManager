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
    ~ViewMarkerWindow();
    void setData(const Data *pointerToData);
    void setCurrentStep(int newIndex);
public slots:
    void updateContainer(int step);
private:
    QGridLayout *markerContainer;
    const Data *data;
    QHBoxLayout* addMarker(Marker marker, int index);
    void populateContainer();

    const int numberColumn = 0;
    const int xColumn = 1;
    const int yColumn = 2;
    const int zColumn = 3;
    const int selectColumn = 5;

    void loadData(int step);
signals:
    void markerPicked(int index);
private slots:
    void selectMarker();
    void removedPickedMarker(int);
};

#endif // VIEWMARKERWINDOW_H
