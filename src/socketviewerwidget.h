#ifndef FILEVIEWERWIDGET_H
#define FILEVIEWERWIDGET_H

#include <QMdiSubWindow>
#include <QTextEdit>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>
#include <qpushbutton.h>
#include <QScrollArea>
#include <QLabel>
#include <QToolTip>
#include <QGridLayout>
#include <vector>

#include "widgetgl.h"

using namespace std;

class CoordinatesLabel;
class WidgetGL;

struct Vector3d; // Implemented in datastructs.h

class SocketViewerWidget : public QMdiSubWindow {
    Q_OBJECT
public:
    SocketViewerWidget(QString path, QString nameFile, QString calibPath);

    void displayToolTip(CoordinatesLabel *label);
    QSlider *getTimeSlider();

private slots:
    void valueChanged(int);
    void areaBarsMoved(int);
    void showXYPlaneFunc();
    void showXZPlaneFunc();
    void showYZPlaneFunc();
    void showSocketTextFunc();
    void show3DWidgetFunc();
    void hideButtonPanelFunc();
    void showPreceedingPointsStateChanged(int);
    void showLinesStateChanged(int);
    void showCamerasStateChanged(int);
    void showCameraLegsStateChanged(int);
    void showCameraFOVStateChanged(int);
    void showCoordinateAxisStateChanged(int);
    void showFloorStateChanged(int);
    void showOrthoStateChanged(int);
    void camDistanceValueChanged(int);
    void fovConeSizeValueChanged(int);

private:
    void readTextFromFile();
    void extractDataFromText();
    void showTextView();
    //void showTableView();
    void show3DView();

    void init();

    int view;
    QString name;
    QString fullPath;
    QString tmpPath;
    QString calibrationPath;
    QString fullText;
    QTextEdit fileContain;

    // Sliders
    QSlider* camDistanceSlider;
    QSlider* fovConeSizeSlider;

    // Checkboxes
    QCheckBox* showPreceedingPoints;
    QCheckBox* showLines;
    QCheckBox* showCameras;
    QCheckBox* showCameraLegs;
    QCheckBox* showCameraFOV;
    QCheckBox* showCoordinateAxis;
    QCheckBox* showFloor;
    QCheckBox* showOrtho;

    // Buttons
    QPushButton* showXYPlane;
    QPushButton* showXZPlane;
    QPushButton* showYZPlane;
    QPushButton* showSocketText;
    QPushButton* show3DWidget;
    QPushButton* hideButton;

    /* Table view */
    vector<QGridLayout*> coordinatesLayout;
    QScrollArea* timeArea;
    QScrollArea* coordinatesArea;
    QScrollArea* timeAreaBis;
    vector<bool> showPoints;

    /* Used for the 3D visualization */
    /* Min and max values for the scale */
    vector<float> minMax;
    /* Lines and columns number */
    int linesNumber;
    int columnsNumber;
    int rowNumber;
    /* The 2D array */
    vector<vector<Vector3d>> pointData;
    /* The time shown */
    int coordinatesShown;
    bool hideButtonPanel;

    QSlider* slider;
    QSpinBox* spinBox;
    WidgetGL* widgetGL;
    QWidget* widgetGLWindow;
    QWidget* buttonPanel;
    QWidget textWidget;
};

class CoordinatesLabel : public QLabel {
public:
    CoordinatesLabel(SocketViewerWidget* wi, QString string) : QLabel(string){
        socket = wi;
        setContextMenuPolicy(Qt::NoContextMenu);
    }
protected:
    void mouseMoveEvent(QMouseEvent *mouseEvent){
        socket->displayToolTip(this);
    }
private:
    SocketViewerWidget* socket;
};
#endif // FILEVIEWERWIDGET_H
