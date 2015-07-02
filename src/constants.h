
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <qstring.h>

using namespace std;

// Testing av SIMD-instruksjoner - Lars Aksel - 04.06.2015
#define SIMD_TESTING_128_BIT // Requires SSE3-support
//#define SIMD_TESTING_256_BIT // Requires AVX2-support, not finished

#if defined(SIMD_TESTING_128_BIT)
const int SIMD_NUM_ELEMENTS = 16;
#elif defined(SIMD_TESTING_256_BIT)
const int SIMD_NUM_ELEMENTS = 32;
#else 
const int SIMD_NUM_ELEMENTS = 1;
#endif

// Paths... (Not used yet)
const QString TRACKPOINT_BINARY_TO_PROJECT = "../";
const QString TRACKPOINT_OPTIONS_FILE = "OptionsOO.txt";
const QString TRACKPOINT_INPUT_DIR = TRACKPOINT_BINARY_TO_PROJECT + "input";
const QString TRACKPOINT_OUTPUT_DIR = TRACKPOINT_BINARY_TO_PROJECT + "output";
const QString TRACKPOINT_CALIBRATION_DIR = TRACKPOINT_INPUT_DIR;
const QString TRACKPOINT_SOCKET_DIR = TRACKPOINT_OUTPUT_DIR;

// Paths... (Is used!)
const QString PROPERTY_PATH = "props";
const QString CAMERA_PROPERTY_DEFAULT_FILE = "defaultCameraSettings.ini";
const QString CAMERA_PROPERTY_QUICK_FILE = "quickfile_camerasettings.ini";
const QString TRACKPOINT_PROPERTY_DEFAULT_FILE = "defaultTrackPointSettings.ini";
const QString TRACKPOINT_PROPERTY_QUICK_FILE = "quickfile_trackpoint.ini";


#endif // CONSTANTS_H
