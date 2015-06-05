#ifndef TRACKPOINT_PROPERTY_H
#define TRACKPOINT_PROPERTY_H

#include <qstring.h>

struct TrackPointProperty {
    bool trackPointPreview = false;
    bool filteredImagePreview = false;
    bool removeDuplicates = true;
    bool showCoordinates = false;
    bool showMinSepCircle = false; // Testing...
    QString thresholdText;  // Descriptive text for "Threshold"
    int thresholdValue = 0; // Actual value of the threshold
    int thresholdMin = 0;   // Minimal value, used for sliders
    int thresholdMax = 0;   // Maximal value, used for sliders
    QString subwinText;
    int subwinValue = 0;
    int subwinMin = 0;
    int subwinMax = 0;
    QString minPointText;
    int minPointValue = 0;
    int minPointMin = 0;
    int minPointMax = 0;
    QString maxPointText;
    int maxPointValue = 0;
    int maxPointMin = 0;
    int maxPointMax = 0;
    QString minSepText;
    int minSepValue = 0;
    int minSepMin = 0;
    int minSepMax = 0;
};
#endif