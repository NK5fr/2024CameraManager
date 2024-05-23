#ifndef SWAPWINDOW_H
#define SWAPWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <cmath>
#include "markercoordinateswidget.h"
#include "data.h"

/**
 * @brief The SwapWindow class
 * SwapWindow is a class which contains and handles the markerCoordinatesWidget. It allows for the update of the coordinates and of the size of the MarkerCoordinatesWidget.
 */

class SwapWindow : public QWidget
{

    Q_OBJECT

private:
    QVector<MarkerCoordinatesWidget*> markerCoordinatesRows;
    QVBoxLayout *layout;
    const Data *data;
    std::array<int, 2> markersToBeSwappedIndexes;
    int currentStep;
    int numberOfFurtherSteps;
    int numberOfFurtherStepsToUpdate;

public:

    /**
     * @brief SwapWindow
     * Constructs a SwapWindow object.
     * @param parent
     *             the parent widget of the SwapWindow
     */
    SwapWindow(QWidget *parent = Q_NULLPTR);

    /**
     * @brief setData
     * Sets the data member variable to point to the Data instance used in the program.
     * @param pointerToData
     *                  the pointer to the constant-cast Data instance used in the program.
     */
    void setData(const Data *pointerToData);

    /**
     * @brief updateCoordinates
     * Updates the x, y and z coordinates shown in the QVector of QLineEdits. Also updates the QLabel displaying the step number at the top of each QLineEdit column
     */
    void updateCoordinates();

    /**
     * @brief setCurrentStep
     * Sets the currentStep attribute to the given step. Depending on the current step and the numberOfFurtherSteps attributes, removes or adds QLineEdit columns in the
     * coordinatesColumns attribute of MarkerCoordinatesWidget. Updates the coordinates shown in the QLineEdits.
     * @param step
     *          the step used to set currentStep to.
     */
    void setCurrentStep(int step);

    /**
     * @brief getNumberOfFurtherStepsToUpdate
     * Returns the numberOfFurtherStepsToUpdate member variable.
     * @return the numberOfFurtherStepsToUpdate member variable.
     */
    int getNumberOfFurtherStepsToUpdate();

public slots:

    /**
     * @brief setNumberOfFurtherSteps
     * Sets the numberOfFurtherSteps attribute to number. Depending on the current step and the numberOfFurtherSteps attributes, removes or adds QLineEdit columns in the
     * coordinatesColumns attribute of MarkerCoordinatesWidget. Updates the coordinates shown in the QLineEdits.
     * @param number
     *          the number to set the numberOfFurtherSteps attribute to.
     */
    void setNumberOfFurtherSteps(int number);

    /**
     * @brief addSelectedMarker
     * Method called when a marker is picked in the DisplayWindow using the swap mode. When a marker is picked, a signal is emitted and a new
     * MarkerCoordinatesWidget is created and added to this object. the number of QLineEdit added to the MarkerCoordinatesWidget is equal to
     * numberOfFurtherStepsToUpdate.
     * @param position
     *          the position of the index in the markersToBeSwappedIndexes DisplayWindow array (0 or 1).
     * @param index
     *          the index of the marker in the dataCoordinates attribute of Data.
     * @param color
     *          the color of the index if it is selected. if not, the color is set to white
     */
    void addSelectedMarker(int position, int index, int color);

    /**
     * @brief removeSelectedMarker
     * Method called when the removeMarkerToBeSwapped signal of DisplayWindow is emitted. Removes the MarkerCoordinatesWidget from the SwapWindow.
     * @param position
     *          the position of MarkerCoordinatesWidget to remove.
     */
    void removeSelectedMarker(int position);

    /**
     * @brief changeMarkerColorToBeSwapped
     * Method called if one of the marker displayed in the SwapWindow is picked in select mode or if it is deselected.
     * When the marker is selected, changes its color to the given color, otherwise it is turned to white.
     * @param position
     *          the position of the marker.
     * @param color
     *          the color to set the marker to. Is equal to -1 if the marker is deselected.
     */
    void changeMarkerColorToBeSwapped(int position, int color);
};

#endif // SWAPWINDOW_H
