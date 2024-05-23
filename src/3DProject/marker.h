#ifndef MARKER_H
#define MARKER_H


/**
 * @brief The Marker class
 * Marker is a class that represents a marker displayed on the screen. It contains an array of three floats which holds the x, y and z coordinates
 * in space of the marker. It also has three color attributes that are used to identify the marker when picking is involved. These identifiers are
 * coded according to the index of the marker in the two-dimensional QVector from the Data class.
 */
class Marker
{

private:
    float markerCoordinates[3];
    int redId;
    int greenId;
    int blueId;

public:

    /**
     * @brief Marker
     * Constructs a default Marker object
     */
    Marker();

    /**
     * @brief Marker
     * Constructs a Marker object using id to code the color identifiers and x, y, and z for the coordinates.
     * @param id
     *          the index of the Marker in the dataCoordinates attribute of the Data class.
     * @param x
     *          the x coordinate value of the marker.
     * @param y
     *          the y coordinate value of the marker.
     * @param z
     *          the z coordinate value of the marker.
     */
    Marker(int id, float x, float y, float z);

    /**
     * @brief Marker
     * Constructs a Marker object using id to code the color identifiers and marker to set the x, y and z coordinates.
     * @param id
     *          the index of the Marker in the dataCoordinates attribute of the Data class.
     * @param marker
     *          a marker used to set the x, y and z coordinates of this marker.
     */
    Marker(int id, const Marker& marker);

    /**
     * @brief getX
     * Returns the x coordinate value of the Marker.
     * @return the x coordinate value of the Marker.
     */
    float getX() const;

    /**
     * @brief getY
     * Returns the y coordinate value of the Marker.
     * @return the y coordinate value of the Marker.
     */
    float getY() const;

    /**
     * @brief getZ
     * Returns the z coordinate value of the Marker.
     * @return the z coordinate value of the Marker.
     */
    float getZ() const;

    /**
     * @brief getRedId
     * Returns the red component of the color identifier of this Marker.
     * @return the red component of the color identifier of this Marker.
     */
    int getRedId() const;

    /**
     * @brief getGreenId
     * Returns the green component of the color identifier of this Marker.
     * @return the green component of the color identifier of this Marker.
     */
    int getGreenId() const;

    /**
     * @brief getBlueId
     * Returns the blue component of the color identifier of this Marker.
     * @return the blue component of the color identifier of this Marker.
     */
    int getBlueId() const;
};

#endif // MARKER_H
