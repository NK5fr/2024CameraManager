
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

struct Vector2d {
    double x;
    double y;
};

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

struct CircleF {
    Vector3d pos;
    double radius;
};

struct PointFileInfo {
    QString filePath;
    bool allPointsInOneImageFirst = true;
    bool sortByString = false;
};

namespace TrackPoint {
    //struct 

    enum CalibrationStatus {
        OK, Warning, Failed
    };

    struct PointInCamera {
        PointInCamera() {
            camNo = -1;
            pointX = -1;
            pointY = -1;
            isUsed = true;
            isInitialized = false;
        }
        PointInCamera(double x, double y, int camNo = -1) {
            pointX = x;
            pointY = y;
            this->camNo = camNo;
            isUsed = true;
            isInitialized = true;
        }
        int camNo;
        double pointX;
        double pointY;
        QString string;
        bool isUsed;
        bool isInitialized;
    };

    struct Camera {
        int camNo;
        unsigned int serialNo;
        int pixelWidth = 1280;
        int pixelHeight = 1024;
        double fovWidth = 0;    // In radians
        double fovHeight = 0;   // In radians
        Vector3d camPos;
        Orientation orient;
        double cameraConstant;  // In pixels, focal-length from camera-sensor.
        double cameraConstantStdDev;
        double XH;
        double XHStdDev;
        double YH;
        double YHStdDev;
        double AF;
        double AFStdDev;
        double ORT;
        double ORTStdDev;
        double F1;
        double F1StdDev;
        double F2;
        double F2StdDev;
        double F3;
        double F3StdDev;
        double P1;
        double P1StdDev;
        double P2;
        double P2StdDev;
        double RO;
        bool valueSet = false;
    };

    struct CameraCombination {
        int camNumbers[3];
        Camera* cameras[3];
        double s0;
        double mean;
        double max;
        int numFrames;
        CalibrationStatus status;
        QString textStatus;
    };
}

#endif