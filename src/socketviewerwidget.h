#ifndef FILEVIEWERWIDGET_H
#define FILEVIEWERWIDGET_H

#include <QMdiSubWindow>
#include <QTextEdit>
#include <QSlider>
#include <QSpinBox>
#include <QScrollArea>
#include <QLabel>
#include <QToolTip>
#include <QGridLayout>
#include <vector>

#include "widgetgl.h"

using namespace std;

class CoordinatesLabel;
class WidgetGL;

struct Vector3d; // Implemented in widgetgl.h

class SocketViewerWidget : public QMdiSubWindow{
    Q_OBJECT
public:
    SocketViewerWidget(QString path, QString nameFile, QString calibPath);

    void displayToolTip(CoordinatesLabel *label);
    QSlider *getTimeSlider();

private slots:
    void onRightClick();
    void menuProjectAction_triggered(QAction*);
    void valueChanged(int);
    void areaBarsMoved(int);

private:
    void readTextFromFile();
    void extractDataFromText();
    void showTextView();
    void showTableView();
    void show3DView();

    int view;
    QString name;
    QString fullPath;
    QString tmpPath;
    QString calibrationPath;
    QString fullText;
    QTextEdit fileContain;

    /* Table view */
    vector<QGridLayout*> coordinatesLayout;
    QScrollArea *timeArea;
    QScrollArea *coordinatesArea;
    QScrollArea *timeAreaBis;
    vector<bool> showPoints;

    /* Used for the 3D visualization */
    /* Min and max values for the scale */
    vector<float> minMax;
    /* Lines and columns number */
    int linesNumber;
    int columnsNumber;
    /* The 2D array */
    vector<vector<Vector3d>> pointData;
    /* The time shown */
    int coordinatesShown;

    QSlider* slider;
    QSpinBox* spinBox;
    WidgetGL* widgetGL;
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
