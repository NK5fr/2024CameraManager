#ifndef DATA_H
#define DATA_H

#include <QVector>
#include <QFile>
#include <iostream>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "marker.h"

/**
 * @brief The Data class
 * the Data class contains the data used by the program. It has a two-dimensional QVector of Markers as its attribute.
 */

class Data
{

private:
    // QVector that stores coordinates of the markers for each time step
    // 2 dimensionnal QVector where the rows correspond to the time steps and the columns correspond to the markers
    QVector<QVector<Marker>> dataCoordinates;
    QString fileName;

public:
    /**
     * @brief Data
     * Constructs a Data object.
     */
    Data();

    /**
     * @brief displayData
     * Prints in the console the data contained in the two-dimensional QVector attribute.
     */
    void displayData() const;

    /**
     * @brief loadData
     * Fills the dataCoordinates attribute with the data contained in the file whose pathname is passed as parameter.
     * @param fileName
     *              The pathname of the file.
     */
    void loadData(QString& fileName);

    /**
     * @brief saveData
     * Saves the data in a file whose name is the QString passed as parameter.
     * @param fileName
     *              The name of the file to be created.
     */
    void saveData(QString& fileName);

    /**
     * @brief saveData
     * Shows a QFileDialog and saves the data in a file whose name is composed of the path to the selected directory and the name entered.
     */
    void saveData();

    /**
     * @brief saveDataSkeleton
     * Saves QVector passed as parameter in a file.
     * @param linkedMarkersIndexes
     *              A QVector from DisplayWindow which contains arrays of two integers corresponding to the indexes of the markers which are linked by
     *              a line.
     */
    void saveDataSkeleton(const QVector<std::array<int, 2>>& linkedMarkersIndexes);

    /**
     * @brief loadSkeleton
     * Returns the skeleton that has been previously saved. This function is called when the program is started.
     * @return a QVector containing arrays of two indexes of the markers to be linked.
     */

    QVector<std::array<int, 2>> loadSkeleton();

    /**
     * @brief getDataCoordinates
     * Returns a pointer to the constant-cast attribute dataCoordinates.
     * @return a constant-cast pointer to the two-dimensional QVector of Markers dataCoordinates.
     */
    const QVector<QVector<Marker>>* getDataCoordinates() const;

    /**
     * @brief get1Vector
     * Returns a constant-cast reference to a single QVector of dataCoordinates for the given step.
     * @param index
     *          An index corresponding to the step to return.
     * @return a constant-cast reference to a QVector of Markers
     */
    const QVector<Marker>& get1Vector(int index) const;

    /**
     * @brief get1Marker
     * Returns a constant-cast reference to a Marker for the given step at the given index.
     * @param step
     *          The step at which the marker is.
     * @param index
     *          The index of the marker to return.
     * @return a constant-cast reference to the Marker to return.
     */
    const Marker& get1Marker(int step, int index) const;

    /**
     * @brief getDataCoordinatesSize
     * Returns the size (number of steps/rows) of the dataCoordinates attribute.
     * @return the size of dataCoordinates attribute.
     */
    int getDataCoordinatesSize() const;

    /**
     * @brief swapMarkersData
     * Swaps the markers that are stored in the given array at the given step.
     * @param markersIndexes
     *          the markers that must be swapped.
     * @param step
     *          the step at which the markers must be swapped.
     */
    void swapMarkersData(const std::array<int, 2>& markersIndexes, int step);
};

#endif // DATA_H

