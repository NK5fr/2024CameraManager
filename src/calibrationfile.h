
#ifndef CALIBRATION_FILE_H
#define CALIBRATION_FILE_H

#include <QtCore/QtCore>
#include <vector>
#include "datastructs.h"

using namespace std;

// Only supporting calibration_summary-files...

class CalibrationFile {
public:
    CalibrationFile(QString filePath);
    ~CalibrationFile();
    
    inline int getNumCameras() { return this->numCameras; }
    inline int getNumCombinations() { return this->numCombinations; }
    inline const vector<TrackPoint::CameraCombination*>& getCameraCombinations() { return this->camCombs; }
    inline const QString& getFileContains() { return this->fileContain; }

private:
    QString filePath;
    QString fileContain;
    int numCameras;
    int numCombinations;
    vector<TrackPoint::CameraCombination*> camCombs;

    void readCalibrationFile();
    void parseCalibrationData(QString& data);

    void calculateFov();
};

#endif
