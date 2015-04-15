

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qstring.h>
#include "trackpointproperty.h"

class Settings {
public:
    static void loadDefaultTrackPointSettings();
    static void loadTrackPointSettingsFromFile(QString& filepath);
    static void saveTrackPointSettingsToFile(QString& filepath, TrackPointProperty& props);
};

#endif