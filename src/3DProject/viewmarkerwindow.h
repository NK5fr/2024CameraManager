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
private:
    QGridLayout *markerContainer;
    const Data *data;
    void populateContainer();

    const int numberColumn = 0;
    const int xColumn = 1;
    const int yColumn = 2;
    const int zColumn = 3;
    const int selectColumn = 5;
    const int swapColumn = 6;
    const int linkColumn = 7;

    int selectedSwapMarkers[2] = {-1,-1};
    QString alreadySelectedLink;

    void loadData(int step);
    int getCorrectIndexFromButton(QPushButton* object);
    void updateButtonColors(QPushButton *button, bool activated);
    void updateSwapButtonColors();

signals:
    void markerPicked(int index);
    void swapMarker(int index);
    void linkMarker(int index);
private slots:
    void selectMarker();
    void swapMarkers();
    void linkMarkers();
    void removedPickedMarker(int index);

    void addSwapMarker(int position, int index);
    void removedSwapMarker(int position);
};

#endif // VIEWMARKERWINDOW_H
