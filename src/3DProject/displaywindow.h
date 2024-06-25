#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H


#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QMatrix4x4>
#include <QFile>
#include <QMouseEvent>
#include <QColor>
#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#endif
#include <GL/glu.h>
#include "data.h"
#include "marker.h"

/**
 * @brief The DisplayWindow class
 * DisplayWindow is a class which handles the 3D-rendering and processes all the events happening on the screen displaying the graphics.
 * It holds three different types of attributes :
 * <ul>
 *      <li> attributes setting the way the data are displayed (displayFormerSteps, displayLinks...).</li>
 *      <li> attributes defining which picking mode is currently used.</li>
 *      <li> QVectors or arrays storing the indexes of the markers picked for each specific picking mode </li>
 * </ul>
 */

class DisplayWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    // a pointer to the data from the Data class of the program
    const Data *data;
    // QVector storing the index of the markers that have been picked in select mode
    QVector<int> selectedMarkerIndexes;
    // QVector storing the color available to display the selected markers
    // colorsAvailable.at(0) is the color of selectedMarkerIndexes.at(0), colorsAvailable.at(1) is selectedMarkerIndexes.at(1) and so on.
    QVector<int> colorsAvailable;
    // QVector that stores arrays holding the two marker-indexes that are linked to each other by a line in link mode
    QVector<std::array<int, 2>> linkedMarkersIndexes;
    // QVector that stores the indexes of the markers that have been picked in swap mode
    std::array<int, 2> markersToBeSwapedIndexes;
    // Stores the current value of the slider (current time step)
    int currentStep;
    // sets the number of previous steps to be displayed
    int numberOfFormerStepsDisplayed;
    // sets the number of further steps to be displayed
    int numberOfFurtherStepsDisplayed;
    // a boolean that indicates whether the previous steps must be painted on the screen or not
    bool displayFormerSteps;
    // a boolean that indicates whether the previous steps should be displayed for selected markers only.
    bool formerStepsSelectedMarkers;
    // a boolean that shows whether the future steps must be painted on the screen or not
    bool displayFurtherSteps;
    // a boolean that indicates if the current picking mode is link mode to know how to process the marker picked
    bool linkMarkerMode;
    // a boolean that indicates whether skeleton of links must be painted on the screen or not
    bool displayLinks;
    // a boolean that shows if the current picking mode is select mode to know how to process the marker picked
    bool selectMarkerMode;
    // a boolean that indicates if the current picking mode is swap mode to know how to process the marker picked
    bool swapMode;
    // a boolean that sets if the current picking mode is erase one link
    bool eraseOneLinkMode;
    // a boolean that is used to know if the white cross should be painted on the screen
    bool lineBeingDrawn;
    // a boolean that indicates whether the previous steps should be displayed as points or lines
    bool formerStepsPoints;

    // int length of the floor
    float floorLength = 10.0;

    // New camera tracking
    float yaw;
    float pitch;

    // Simpler way to change colors if needed
    QColor backgroundColor = QColor(0,18,53);
    QColor gridColor = QColor(255,255,255);
    QColor dragColor = QColor(255, 204, 0);

    //dragNdropValues
    bool shiftPressed = false;
    int mouseClickX = -1;
    int mouseClickY = -1;
    int mouseDragPosX = -1;
    int mouseDragPosY = -1;
    bool dragSelectActive = false;
    // the x position of the mouse on the screen ON MOUSEPRESS
    int mouseXStartPosition;
    // the y position of the mouse on the screen ON MOUSEPRESS
    int mouseYStartPosition;

    QVector3D cameraPos;
    QVector3D cameraCenter;
    int zAngle = 0;


    QVector<QVector3D> points;
public:

    /**
     * @brief DisplayWindow
     * Construct a DisplayWindow object. the initializeGL() and paintGL() methods are called when the constructor is used.
     * @param parent
     *          The parent widget of the DisplayWindow instance.
     */
    DisplayWindow(QWidget *parent = Q_NULLPTR);

    /**
     * @brief setViewPort
     * Sets the viewPort (size of the drawing region)
     */
    void setViewPort();

    /**
     * @brief setProjection
     * Sets the projection matrix (sets the shape of the viewing volume)
     */
    void setProjection();

    /**
     * @brief setModelView
     * Sets the modelView matrix (sets the position of the model in the scene)
     */
    void setModelView();

    /**
     * @brief setData
     * Sets the data member variable to point to the Data instance used in the program.
     * @param pointerToData
     *                  the pointer to the constant-cast Data instance used in the program.
     */
    void setData(const Data * pointerToData);

    /**
     * @brief setCurrentStep
     * Sets the current step to the given index.
     * @param index
     *                  the current value of the slider from ProgramWindow.
     */
    void setCurrentStep(int index);

    /**
     * @brief getSelectedMarkerIndexes
     * Returns a const-cast reference to a QVector containing the indexes of the markers that have been picked in selection mode.
     * @return a const-cast reference to the QVector of selected indexes.
     */
    const QVector<int>& getSelectedMarkerIndexes() const;

    /**
     * @brief getMarkersToBeSwaped
     * Returns a const-cast reference to an array containing the indexes of the markers that have been picked in swap mode.
     * @return a const-cast reference to an array of the two markers to be swapped.
     */
    const std::array<int, 2>& getMarkersToBeSwaped() const;

    /**
     * @brief pickMarker
     * Returns the index of the marker picked. this method is used by the selectMarker(), linkMarkerLine() and swapMarkers() methods.
     * if the background is picked, the value returned is -1.
     * @return the index of the marker picked or -1 if the background is picked.
     */
    int pickMarker();
    /**
     * @brief selectMarker
     * Appends the index of the marker picked at the end of the selectedMarkerIndexes QVector. this method is called when the selection mode is enabled.
     * the number of marker selected must be below 12. A signal is sent to add the marker to the CoordinatesWindow. Another signal is sent to paint the
     * color of the marker in the SwapWindow.
     */
    void selectMarker();

    /**
     * @brief linkMarkerLine
     * Adds the index of the marker picked in an array of two integers contained in a QVector. if the array of integers holds two indexes, a line
     * can be drawn between the markers corresponding to these indexes.
     */
    void linkMarkerLine();

    /**
     * @brief pickLink
     * Returns the index in the linkedMarkersIndexes attribute of the link picked. this method is used by the removePickedLink() method.
     * if the background is picked, the value returned is -1.
     * @return the index of the link picked or -1 if the background is picked.
     */
    int pickLink();

    /**
     * @brief removePickedLink
     * remove the array of two integers located in linkedMarkersIndexes at the index corresponding to the link picked
     */
    void removePickedLink();

    /**
     * @brief swapMarkers
     * Adds the index of the marker picked in the markersToBeSwapedIndexes array. each integer is initialized to -1
     * if two markers are already contained in the array when a marker is
     * picked, then the index at index 1 is removed. if an already picked marker is picked again then it is removed from the array.
     * Clicking on the background sets the last element to -1 and the first element to -1 if the last element is already equal to -1.
     * Once a marker is added or removed, a signal is sent to the SwapWindow to add or remove a MarkerCoordinatesWidget.
     */
    void swapMarkers();

    /**
     * @brief setLinkedMarkersVector
     * Sets the linkedMarkersIndexes attribute to the given QVector
     * @param linkedMarkers
     *          the QVector to set linkedMarkersIndexes to
     */

    void setLinkedMarkersVector(QVector<std::array<int, 2>> linkedMarkers);

    /**
     * @brief setLinkMarkerMode
     * Sets the linkMarkerMode attribute to the given boolean.
     * @param boolean
     *          the state to set linkMarkerMode to.
     */
    void setLinkMarkerMode(bool boolean);

    /**
     * @brief setEraseOneLinkMode
     * Sets the eraseOneLinkMode attribute to the given boolean.
     * @param boolean
     *          the state to set eraseOneLinkMode to.
     */
    void setEraseOneLinkMode(bool boolean);

    /**
     * @brief setSwapMode
     * Sets the swapMode attribute to the given boolean.
     * @param boolean
     *          the state to set swapMode to.
     */
    void setSwapMode(bool boolean);

    /**
     * @brief setDisplayLinks
     * Sets the displayLinks attribute to the given boolean
     * @param boolean
     *          the state to set displayLinks to.
     */
    void setDisplayLinks(bool boolean);

    /**
     * @brief setSelectMarkerMode
     * Sets the selectMarkerMode attribute to the given boolean.
     * @param boolean
     *          the state to set selectMarkerMode to.
     */
    void setSelectMarkerMode(bool boolean);

    /**
     * @brief setDisplayFormerSteps
     * Sets the displayFormerSteps attribute to the given boolean.
     * @param boolean
     *          the state to set displayFormerSteps to.
     */
    void setDisplayFormerSteps(bool boolean);

    /**
     * @brief setDisplayFurtherSteps
     * Sets the displayFurtherSteps attribute to the given boolean.
     * @param boolean
     *          the state to set displayFurtherSteps to.
     */
    void setDisplayFurtherSteps(bool boolean);

    /**
     * @brief setFormerStepsPoints
     * Sets the formerStepsPoints attribute to the given boolean.
     * @param boolean
     *          the state to set formerStepsPoints to.
     */
    void setFormerStepsPoints(bool boolean);

    /**
     * @brief setFormerStepsSelectedMarkers
     * Sets the formerStepsSelectedMarkers attribute to the given boolean.
     * @param boolean
     *          the state to set formerStepsSelectedMarkers to.
     */
    void setFormerStepsSelectedMarkers(bool boolean);

    /**
     * @brief getMarkerWithCross
     * Returns a const-cast reference to the first marker that has been picked using link mode.
     * @return a const-cast reference to the marker that has been picked with link mode.
     */
    const Marker& getMarkerWithCross() const;

    /**
     * @brief getLinkedMarkersIndexes
     * Returns a const-cast reference to a QVector containing arrays of two integers.
     * these arrays contains the indexes of the markers that are linked by lines.
     * @return a const-cast reference to a QVector corresponding to the linkedMarkersIndexes attribute.
     */
    const QVector<std::array<int, 2>>& getLinkedMarkersIndexes() const;

    /**
     * @brief alreadyLinkedMarkers
     * Returns a boolean which indicates whether the two markers are already linked by a line.
     * @param linkedMarkers
     *              an array of two integers containing the two indexes of linked markers.
     * @return a boolean stating whether the array is already in the linkedMarkersIndexes member variable.
     */
    bool alreadyLinkedMarkers(std::array<int, 2>& linkedMarkers);
public slots:
    void selectMarker(int index);

    /**
     * @brief swapMarkers
     * Adds the index of the marker picked in the markersToBeSwapedIndexes array. each integer is initialized to -1
     * if two markers are already contained in the array when a marker is
     * picked, then the index at index 1 is removed. if an already picked marker is picked again then it is removed from the array.
     * Clicking on the background sets the last element to -1 and the first element to -1 if the last element is already equal to -1.
     * Once a marker is added or removed, a signal is sent to the SwapWindow to add or remove a MarkerCoordinatesWidget.
     */
    void swapMarkers(int index);

    /**
     * @brief linkMarkerLine
     * Adds the index of the marker picked in an array of two integers contained in a QVector. if the array of integers holds two indexes, a line
     * can be drawn between the markers corresponding to these indexes.
     */
    void linkMarkerLine(int index);

    /**
     * @brief resetCamera
     * Moves the camera to its initial position. Resets the modelView matrix.
     */
    void resetCamera();

    /**
     * @brief moveCameraToLeftSide
     * Moves the camera to the left side of the scene
     */
    void moveCameraToLeftSide();

    /**
     * @brief moveCameraToFrontSide
     * Moves the camera to the front of the scene.
     */
    void moveCameraToFrontSide();

    /**
     * @brief moveCameraToBackSide
     * Moves the camera to the back of the scene.
     */
    void moveCameraToBackSide();

    /**
     * @brief moveCameraToRightSide
     * Moves the camera to the right side of the scene.
     */
    void moveCameraToRightSide();

    /**
     * @brief removePickedIndex
     * Removes the markers located at the given index from the selectedMarkerIndexes attribute.
     * If the marker is also a marker to be swapped, a signal is emitted to alert SwapWindow to turn its color to white.
     * @param index
     *          the index at which the index of the marker is located.
     */
    void removePickedIndex(int index);

    /**
     * @brief resetLinkedMarkersIndexes
     * Removes every array of two indexes from the linkedMarkersIndexes attribute
     */
    void resetLinkedMarkersIndexes();

    /**
     * @brief setNumberOfFormerStepsDisplayed
     * Sets the numberOfFormerStepsDisplayed to the given number.
     * @param number
     *          the number to set numberOfFormerStepsDisplayed to.
     */
    void setNumberOfFormerStepsDisplayed(int number);

    /**
     * @brief setNumberOfFurtherStepsDisplayed
     * Sets the numberOfFurtherStepsDisplayed to the given number.
     * @param number
     *          the number to set numberOfFurtherStepsDisplayed to.
     */
    void setNumberOfFurtherStepsDisplayed(int number);

signals:

    /**
     * @brief markerPicked
     * Signal emitted when a marker is picked in selection mode. the signal indicates to CoordinatesWindow the index of the marker in the
     * dataCoordinates attribute of the Data class and its color.
     * @param index
     *          the index of the marker picked.
     * @param color
     *          the color of the marker once selected.
     */
    void markerPicked(int index, int color);

    void markerRemoved(int index);

    /**
     * @brief markerToBeSwappedPicked
     * Signal emitted when a marker is added to markersToBeSwapedIndexes in swap mode. the signals indicates to SwapWindow the position of the marker
     * in the array (0 or 1) the index of the marker and its color.
     * @param position
     *          the position of the marker in the markersToBeSwapedIndexes attribute.
     * @param index
     *          the index of the marker in the dataCoordinates attribute of the Data class.
     * @param color
     *          the color of the marker (set to white if the marker is not a selected marker).
     */
    void markerToBeSwappedPicked(int position, int index, int color);

    /**
     * @brief removeMarkerToBeSwapped
     * Signal emitted when a marker is removed from markersToBeSwapedIndexes in swap mode. The index of the marker is removed when :
     * <ul>
     *      <li> it is already in markersToBeSwapedIndexes </li>
     *      <li> it is at position 1 and another marker is picked </li>
     *      <li> the last element picked is the background of the scene </li>
     * </ul>
     * The signal indicates to SwapWindow the position of the MarkerCoordinatesWidget to remove.
     * @param position
     *          the position of the MarkerCoordinatesWidget to remove.
     */
    void removeMarkerToBeSwapped(int position);

    /**
     * @brief changeColorMarkerToBeSwapped
     * Signal emitted when a marker whose index is contained in markersToBeSwapedIndexes is selected in selection mode.
     * This signal is sent to SwapWindow to paint the color of the marker if a marker is selected or turn it to white if it is removed
     * from selectedMarkerIndexes.
     * @param position
     *          the index of the marker in the markersToBeSwapedIndexes array.
     * @param color
     *          the color to set the marker to in the SwapWindow.
     */
    void changeColorMarkerToBeSwapped(int position, int color);

protected:

    /**
     * @brief initializeGL
     * Method called when the OpenGLWidget is constructed.
     */
    void initializeGL();


    /**
     * @brief paintGL
     * Method that paints or repaints the scene when the update method is used.
     * Calls the following rendering methods :
     * <ul>
     *      <li> paintAxes() </li>
     *      <li> paintMarkers() </li>
     *      <li> paintSelectedMarkers() </li>
     *      <li> paintLinkedMarkers() </li>
     *      <li> paintFormerSteps() </li>
     *      <li> paintFurtherSteps() </li>
     *      <li> paintMarkerWithCross() </li>
     *      <li> paintMarkersWithRedCross() </li>
     * </ul>
     */
    void paintGL();

    /**
     * @brief paintAxes
     * Displays the x, y and z axes on the OpenGLWidget. the x axis is painted in red, the y axis is painted in green and the z axis is painted in blue.
     */
    void paintAxes();

    /**
     * @brief paintMarkers
     * Paints the markers on the screen according to their coordinates from the Data class.
     */
    void paintMarkers();

    /**
     * @brief paintSelectedMarkers
     * Paints the markers whose index is in selectedMarkerIndexes using their specific color.
     */
    void paintSelectedMarkers();

    /**
     * @brief paintLinkedMarkers
     * Paints a line between the markers whose indexes are in the linkedMarkersIndexes QVector.
     */
    void paintLinkedMarkers();

    /**
     * @brief paintFormerSteps
     * Paints the markers' previous positions if displayFormerSteps is set to true. The number of previous states displayed depends on the
     * numberOfFormerStepsDisplayed member variable. If formerStepsPoints is set to true, the previous positions are displayed using points
     * else they are linked by lines. If formerStepsSelectedMarkers is turned to true, then only the previous positions of the selected markers are
     * painted.
     */
    void paintFormerSteps();

    /**
     * @brief paintFurtherSteps
     * Paints the markers' next positions if displayFurtherSteps is set to true. the number of further positions displayed depends on the
     * numberOfFurtherStepsDisplayed member variable. The next positions are linked by lines and are displayed for the selected markers only.
     */
    void paintFurtherSteps();

    /**
     * @brief paintMarkerWithCross
     * Paints a white cross on the marker picked using link Mode.
     */
    void paintMarkerWithCross();

    /**
     * @brief paintMarkersWithRedCross
     * Paints a red cross on the markers picked using swap mode.
     */
    void paintMarkersWithRedCross();

    /**
     * @brief paintFloor
     * Paints a grid on z=0 of the view
     */
    void paintFloor();


    void drawLine(QVector3D begin, QVector3D end);

    /**
     * @brief mousePressEvent
     * Method called when one of the buttons of the mouse is pressed.
     * Enables the camera to be moved.
     * Depending on the current picking mode, calls swapMarkers(), selectMarker(), linkMarkerLine() or removePickedLink().
     * @param event
     *          the event corresponding to the mouse click.
     */
    void mousePressEvent(QMouseEvent *event);

    // /**
    //  * @brief mouseMoveEvent
    //  * Method called when the mouse is moved. As tracking is disabled, this method is called only when one of the buttons of the mouse is clicked.
    //  * Calls the moveCamera method.
    //  * @param event
    //  *          the event corresponding to the mouse last motion.
    //  */
    // void mouseMoveEvent(QMouseEvent *mouseEvent);

    /**
     * @brief moveCamera
     * Moves the camera according to the motion of the mouse.
     * @param event
     *          the QMouseEvent sent by the mouseMoveEvent method.
     */
    void moveCamera(QMouseEvent *event);

    void drawFloorLine(float x, float y);

    void paintDragZone();

    bool eventFilter(QObject *watched, QEvent *event);

    void drawSquare(QVector3D begin, QVector3D end);

    void paintPoints();

    QVector3D get3DPos(int x, int y);
    QVector3D transformToViewSpace(const QVector3D &point, const QMatrix4x4 &modelview);
    QVector3D transformFromViewSpace(const QVector3D &point, const QMatrix4x4 &inverseModelview);
    void updateViewMatrix();
    void updateProjection(int width, int height, double fov);
    void magicGL();
    void changeCameraPosition(int x, int y, int z);
    void updateViewFromCameraPos();
};

#endif // DISPLAYWINDOW_H

