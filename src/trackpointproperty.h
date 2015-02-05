#ifndef TRACKPOINT_PROPERTY_H
#define TRACKPOINT_PROPERTY_H

#include <qstring.h>

struct TrackPointProperty {
    bool trackPointPreview = false;
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
};
#endif