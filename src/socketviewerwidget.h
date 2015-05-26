#ifndef SOCKETVIEWERWIDGET_H
#define SOCKETVIEWERWIDGET_H

#include <QMdiSubWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialog>
#include <qpushbutton.h>
#include <QScrollArea>
#include <QLabel>
#include <QToolTip>
#include <QGridLayout>
#include <QtNetwork\qtcpsocket.h>
#include <QtNetwork\qhostaddress.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "widgetgl.h"

using namespace std;

class QTcpSocket;
class QNetworkSession;

class CoordinatesLabel;
class WidgetGL;

struct Vector3d; // Implemented in datastructs.h

class SocketViewerWidget : public QMdiSubWindow {
    Q_OBJECT
public:
    SocketViewerWidget(QString path, QString nameFile, QString calibPath); // Loading from file
    SocketViewerWidget(); // Loading from network (Shows dialogbox for IP and Port input)
    ~SocketViewerWidget();

    void displayToolTip(CoordinatesLabel *label);
    void appendPoints(vector<Vector3d*>);
    QSlider *getTimeSlider();
    vector<Vector3d*> readLine(QString line);

    inline QTextEdit* getFileContain() { return this->fileContain; }
    inline WidgetGL* getWidgetGL() { return this->widgetGL; }


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
    void show3DView();

    void init();

    //int view;
    QString filename;
    QString fullPath;
    QString tmpPath;
    QString calibrationPath;
    QString fullText;
    QTextEdit* fileContain;

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
    /* Lines and columns number */
    int linesNumber;
    int rowNumber;
    /* The 2D array */
    vector<vector<Vector3d*>> pointData;
    /* The time shown */
    int coordinatesShown;
    bool hideButtonPanel;

    const char* ipAddress;
    const char* port;
    int sock;
    bool clientRunning;
    bool stopped;

    QSlider* slider;
    QSpinBox* spinBox;
    WidgetGL* widgetGL;
    QWidget* widgetGLWindow;
    QWidget* buttonPanel;
    QWidget textWidget;
};

class CoordinatesLabel : public QLabel {
    Q_OBJECT
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

class HostAddressDialog : public QDialog {
    Q_OBJECT
public:
    HostAddressDialog(QWidget* parent, SocketViewerWidget*);

private slots :
    void connectToServer();
    void readSocketLine();
    void displayError(QAbstractSocket::SocketError);
    //void sessionOpened();
    void socketConnected();
    void onTextEdited(const QString&);
    void stopClient();

private:
    SocketViewerWidget* socketWidget;
    QHostAddress hostAddress;
    QTcpSocket* tcpSocket = nullptr;
    QLineEdit* portLineEdit = nullptr;
    QComboBox* hostCombo = nullptr;
};
#endif // FILEVIEWERWIDGET_H
