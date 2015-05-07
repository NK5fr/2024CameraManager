
#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

struct Vector3d {
    double x;
    double y;
    double z;
};

struct Orientation {
    double alpha;   // Orientation around X-axis
    double beta;    // Orientation around Y-axis
    double kappa;   // Orientation around Z-axis
};

namespace TrackPoint {
    //struct 

    struct PointInCamera {
        PointInCamera() {
            camNo = -1;
            pointX = -1;
            pointY = -1;
            isUsed = true;
            isInitialized = false;
        }
        PointInCamera(int x, int y, int camNo) {
            pointX = x;
            pointY = y;
            this->camNo = camNo;
            isUsed = true;
            isInitialized = true;
        }
        int camNo;
        int pointX;
        int pointY;
        bool isUsed;
        bool isInitialized;
    };

    struct Camera {
        int camNo;
        int serialNo;
        int pixelWidth = 1280;
        int pixelHeight = 1024;
        double fovWidth = 0;    // In radians
        double fovHeight = 0;   // In radians
        Vector3d camPos;
        Orientation orient;
        double cameraConstant;  // In pixels, focal-length from camera-sensor.
        double cameraConstantStdDev;
        bool valueSet = false;
    };

    struct CameraCombination {
        int camNumbers[3];
        Camera* cameras[3];
        double s0;
        double mean;
        double max;
        int numFrames;
        bool valid;
    };
}

#endif