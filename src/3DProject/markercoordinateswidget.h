#ifndef MARKERCOORDINATESWIDGET_H
#define MARKERCOORDINATESWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include "marker.h"

/**
 * @brief The MarkerCoordinatesWidget class
 * MarkerCoordinatesWidget is a class which is used to display coordinates of the Markers which have been picked in swap mode. It also displays the
 * color of the marker if it is selected.
 */

class MarkerCoordinatesWidget : public QFrame
{

private:
    QLabel * markerNumber;
    QLabel * x;
    QLabel * y;
    QLabel * z;
    QLabel *timeStep;
    QLineEdit * markerColor;
    std::array<QVector<QLineEdit *>, 3> coordinatesColumns;
    QVector<QLabel *> timeStepsLabels;
    QGridLayout *layout;

public:

    /**
     * @brief MarkerCoordinatesWidget
     * Constructs a MarkerCoordinatesWidget with the given number (0 or 1), its color (white if the marker is not selected) and a parent widget.
     * @param number
     *          the position of the marker in the marksToBeSwappedIndexes attribute of the DisplayWindow class.
     * @param color
     *          the color of the marker if it is highlighted.
     * @param parent
     *          the parent of the widget.
     */
    MarkerCoordinatesWidget(int number, int color, QWidget *parent = Q_NULLPTR);

    /**
     * Destructs the MarkerCoordinatesWidget. When the destructor is called, all the elements of the widget are deleted.
     */
    ~MarkerCoordinatesWidget();

    /**
     * @brief setStepCoordinates
     * Sets the QLineEdits corresponding to the x, y and z coordinates of the marker at the given step to the x, y and z coordinates of marker.
     * @param step
     *          the step at which the coordinates must be set.
     * @param marker
     *          the marker used to set the QLineEdit text to.
     */
    void setStepCoordinates(int step, const Marker& marker);

    /**
     * @brief addStepCoordinates
     * Adds a new column of three QLineEdits displaying the x, y and z coordinates to the MarkerCoordinatesWidget.
     */
    void addStepCoordinates();

    /**
     * @brief removeStepCoordinates
     * Removes a column of three QLineEdits displaying the x, y and z coordinates from the MarkerCoordinatesWidget.
     */
    void removeStepCoordinates();

    /**
     * @brief getSizeSteps
     * Returns the number of steps shown in the MarkerCoordinatesWidget.
     * @return the number of steps shown in the MarkerCoordinatesWidget.
     */
    int getSizeSteps();

    /**
     * @brief setMarkerNumber
     * Sets the QLabel attribute markerNumber to number.
     * @param number
     *          the number to set markerNumber to (1 or 2).
     */
    void setMarkerNumber(int number);

    /**
     * @brief setMarkerColor
     * Sets the color of the QLineEdit markerColor attribute to color.
     * @param color
     *          the color to set markerColor to.
     */
    void setMarkerColor(int color);

    /**
     * @brief addStepLabel
     * Appends a new label showing the step number at the top of the QLineEdit columns.
     */
    void addStepLabel();

    /**
     * @brief removeStepLabel
     * Removes a label showing the step number at the top of the QLineEdit columns.
     */
    void removeStepLabel();

    /**
     * @brief updateStepLabel
     * Sets the text of stepLabel at the given index of timeStepsLabels to stepNumber.
     * @param index
     *          the index where the label is located.
     * @param stepNumber
     *          the number which the label must be set to.
     */
    void updateStepLabel(int index, int stepNumber);
};

#endif // MARKERCOORDINATESWIDGET_H
