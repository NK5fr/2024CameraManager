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
    void addLink(int index1, int index2);
    void removeLink(int indexOfLink);
    void resetLinks();
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


    QVector<bool> selectedMarkers;
    int selectedSwapMarkers[2] = {-1,-1};
    QVector<std::array<int, 2>>alreadySelectedLink;
    bool isFirstLinkClicked = true;

    void loadData(int step);
    int getCorrectIndexFromButton(QPushButton* object);
    void updateButtonColors(QPushButton *button, bool activated);
    void updateSwapButtonColors();
    void updateLinkButton(int index);
    void resetLinkIndicators();

    QPushButton* findButton(int index,QString suffix);
    void selectPickedMarker(int index);
signals:
    void markerPicked(int index);
    void markerRemoved(int index);
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
