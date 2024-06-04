#ifndef PROGRAMWINDOW_H
#define PROGRAMWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QKeyEvent>
#include <QTabWidget>
#include "displaywindow.h"
#include "data.h"
#include "coordinateswindow.h"
#include "filewindow.h"
#include "swapwindow.h"
#include "viewmarkerwindow.h"
/**
 * @brief The ProgramWindow class
 * ProgramWindow is the main class of the program. It contains all the different widgets, buttons and classes involved in the application.
 * It also connects together the different parts of the program thanks to the signal/slot mechanism.
 */

class ProgramWindow : public QWidget
{
    Q_OBJECT

private:
    QSlider *slider;
    DisplayWindow *screen;
    CoordinatesWindow *coordinatesWindow;
    FileWindow *filewindow;
    SwapWindow *swapWindow;
    ViewMarkerWindow *viewMarkerWindow;
    Data data;
    QLabel *label;
    QLabel *swapLabel;
    QPushButton *demoButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *stepForward;
    QPushButton *stepBackward;
    QPushButton *frontSideCamera;
    QPushButton *backSideCamera;
    QPushButton *leftSideCamera;
    QPushButton *rightSideCamera;
    QPushButton *saveDataButton;
    QPushButton *displayFileCoordinates;
    QPushButton *selectModeButton;
    QPushButton *linkModeButton;
    QPushButton *eraseLinks;
    QPushButton *eraseOneLinkButton;
    QPushButton *saveSkeletonButton;
    QPushButton *resetButton;
    QCheckBox *displayLinksButton;
    QCheckBox *displayFormerStepsSelectedMarkers;
    QPushButton *formerSteps;
    QPushButton *furtherSteps;
    QPushButton *swapModeButton;
    QPushButton *swapMarkersButton;
    QRadioButton *displayChoice1;
    QRadioButton *displayChoice2;
    QRadioButton *formerStepsLine;
    QRadioButton *formerStepsPoints;
    QComboBox *numberOfFormerSteps;
    QComboBox *numberOfFurtherSteps;
    QComboBox *numberOfSwapedSteps;
    QTimer *timer;

public:

    /**
     * @brief ProgramWindow
     * Constructs a ProgramWindow with an empty Data object.
     */
    ProgramWindow(QWidget *parent = Q_NULLPTR);

    /**
     * @brief ProgramWindow
     * Constructs a ProgramWindow with a Data object whose dataCoordinates attribute is filled with the data contained in the file
     * given in parameter.
     * @param fileName
     *          the pathname of the file which contains the data of the program.
     */
    ProgramWindow(QString& fileName, QWidget *parent = Q_NULLPTR);

    /**
     * @brief configureScreen
     * Configures the DisplayWindow attribute. Calls the DisplayWindow methods setViewPort, setProjection and setModelView.
     */
    void configureScreen();

    /**
     * @brief connectWidgets
     * Connects all the widgets of this ProgramWindow to the different parts of the program (connection between signals and slots).
     */
    void connectWidgets();

    /**
     * @brief setData
     *          Sets the dataCoordinates attribute of the Data object of this ProgramWindow to the data contained in the given file
     * @param fileName
     *          the pathname of the file which contains the data of the program.
     */
    void setData(QString& fileName);

protected:
    void keyPressEvent(QKeyEvent *event);

public slots:

    /**
     * @brief changeStep
     * Sets the currentStep attribute of DisplayWindow, CoordinatesWindow, and SwapWindow to the new value of the slider
     * @param index
     *              the new value of the slider
     */
    void changeStep(int index);

    /**
     * @brief incrementSlider
     * Adds one to the current value of the slider
     */
    void incrementSlider();

    /**
     * @brief decrementSlider
     * subtract one to the current value of the slider
     */
    void decrementSlider();

    /**
     * @brief fillCoordinatesWindow
     * Adds this selected marker to the CoordinatesWindow. this method is called when a marker is picked using the selection mode.
     * @param index
     *              the index of the marker in the dataCoordinates attribute of the Data class.
     * @param color
     *              the color of the selected marker.
     */
    void fillCoordinatesWindow(int index, int color);

    /**
     * @brief demoPlaying
     * Methods called when the demo is playing. Adds one to the value of slider
     */
    void demoPlaying();

    /**
     * @brief startDemo
     * Methods that starts a QTimer which emits a signal at a fixed rate that calls demoPlaying().
     * Enables the use of the pause button and the stop button.
     */
    void startDemo();

    /**
     * @brief pauseDemo
     * deletes the timer created with startDemo() to pause the demo.
     */
    void pauseDemo();
    /**
     * @brief stopDemo
     * deletes the timer created with startDemo() and reset the value of the slider to 0.
     */
    void stopDemo();
    void displayFile();
    void changeChoice1();
    void changeChoice2();

    /**
     * @brief pickMode
     * Assesses which picking mode button has been clicked (selectModeButton, linkModeButton, swapModeButton, eraseOneLinkButton) then sets the attribute
     * corresponding to that mode in the DisplayWindow to true and sets the attributes corresponding to the other modes to false if the button is checked.
     * Sets the attribute corresponding to that mode in the DisplayWindow to false otherwise.
     */
    void pickMode();

    /**
     * @brief enableDisplayLinks
     * Sets the DisplayWindow attribute displayLinks to true if the displayLinksButton is checked, sets it to false otherwise.
     */
    void enableDisplayLinks();

    /**
     * @brief enableDisplayFormerSteps
     * Sets the DisplayWindow attribute displayFormerSteps to true if the formerSteps button is checked, sets it to false otherwise.
     */
    void enableDisplayFormerSteps();

    /**
     * @brief enableDisplayFormerStepsSelectedMarkers
     * Sets the DisplayWindow attribute formerStepsSelectedMarkers to true if the displayFormerStepsSelectedMarkers is checked, sets it to false otherwise.
     */
    void enableDisplayFormerStepsSelectedMarkers();

    /**
     * @brief enableDisplayFurtherSteps
     * Sets the DisplayWindow attribute displayFurtherSteps to true if the furtherSteps button is checked, sets it to false otherwise.
     */
    void enableDisplayFurtherSteps();

    /**
     * @brief choosePointsLinesFormerSteps
     * Sets the DisplayWindow attribute formerStepsPoints to true if the formerStepsPoints button is checked, sets it to false otherwise.
     */
    void choosePointsLinesFormerSteps();

    /**
     * @brief saveSkeleton
     * Calls the saveDataSkeleton Data method passing the DisplayWindow attribute linkedMarkerIndexes.
     */
    void saveSkeleton();

    /**
     * @brief saveData
     * Saves the current state of the data attribute in a file.
     */

    void saveData();

    /**
     * @brief swapMarkers
     * Calls the swapMarkersData Data method. Uses the markersToBeSwappedIndexes DisplayWindow attribute to get the markers to be swapped and uses the numberOfStepsToUpdate
     * SwapWindow attribute to swap the correct amount of steps. After swapping, the CoordinatesWindow and the SwapWindow objects are updated.
     */
    void swapMarkers();

    void removeMarkerCoordinatesWindow(int index);


signals:

    /**
     * @brief stopTimer
     * Signal emitted to stop the timer from firing its signal
     */
    void stopTimer();
};

#endif // PROGRAMWINDOW_H
