#ifndef COORDINATESWINDOW_H
#define COORDINATESWINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <cmath>
#include "data.h"
#include "marker.h"

/**
 * @brief The CoordinatesWindow class
 *
 * The CoordinatesWindow class is used to display the markers picked in " selection " mode. This class contains a two-dimensional QVector of
 * QLineEdit which are used to display the x, y and z coordinates of the markers as well as their color. It also contains a QVector of QPushButtons
 * which enable the user to remove the various markers.
 */

class CoordinatesWindow : public QWidget
{
    Q_OBJECT

private:
    int currentStep;
    QVector<int> selectedMarkersIndexes;
    const Data * data;
    QGridLayout *layout;
    // QVector that stores "remove" buttons
    QVector<QPushButton*> buttonVector;
    // QVector that stores the label showing the number of the marker in the list of selected markers
    QVector<QLabel*> labelVector;
    // QVector that stores the QLineEdit that are used to display the coordinates of the markers
    QVector<QVector<QLineEdit*>> xyzVector;

public:

    /**
     * @brief CoordinatesWindow
     *
     * Constructs a CoordinatesWindow.
     *
     * @param parent
     *              the parent widget which contains this instance of CoordinatesWindow
     */
    CoordinatesWindow(QWidget *parent = Q_NULLPTR);

    /**
     * @brief setData
     *
     * Sets the data member variable to point to the Data instance used in the program.
     *
     * @param pointerToData
     *                  the pointer to the constant-cast Data instance used in the program.
     */
    void setData(const Data *pointerToData);

    /**
     * @brief setCurrentStep
     *
     * Sets the currenStep member variable to the given paramater.
     *
     * @param step
     *              the current value of the slider contained in the ProgramWindow class.
     */
    void setCurrentStep(int step);

    /**
     * @brief addLineCoordinates
     *
     * Adds a line in the CoordinatesWindow when a marker is picked in " selection " mode
     * when a line is added, four QLineEdit corresponding to the color and the x,y and z coordinates of the markers are added
     * to the layout and the QVectors. A QPushButton is added to enable the user to remove this line.
     *
     * @param index
     *              the index of the selected marker in the data attribute.
     * @param color
     *              the color used to highlight the selected marker.
     */
    void addLineCoordinates(int index, int color);

    /**
     * @brief updateCoordinates
     * Sets the x, y and z fields to the values corresponding to the x, y and z values of the marker at the currentStep.
     */
    void updateCoordinates();

    /**
     * @brief updateLabelNumber
     * Updates the marker number in front of each line following the removed marker.
     * @param index
     *              the index of the removed line.
     */
    void updateLabelNumber(int index);

    /**
     * @brief swapCoordinates
     * Swaps the x, y and z fields of the two lines corresponding to the markers which have been swapped
     * @param markersToBeSwaped
     *              the indexes of the two markers which have been swapped.
     */
    void swapCoordinates(const std::array<int, 2>& markersToBeSwaped);

public slots:

    /**
     * @brief removeLineCoordinates
     * Method which assesses which remove button has been pressed, and deletes every widget that are positionned on the line of the button.
     * the signal lineRemoved() is also emitted in that function
     * @see lineRemoved();
     */
    void removeLineCoordinates();

    void removeLineCoordinates(int i);

    void removeMarkerCoordinates(int index);
signals:

    /**
     * @brief lineRemoved
     * A signal emitted when a remove button has been pushed. Alerts the DisplayWindow class to update its selectedMarkersIndexes QVector
     * to remove the index located at index.
     * @param index
     *              the index at which is the index to be removed.
     */
    void lineRemoved(int index);

    void removedMarker(int index);
};

#endif // COORDINATESWINDOW_H
