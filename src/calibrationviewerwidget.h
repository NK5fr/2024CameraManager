#ifndef CALIBRATIONVIEWERWIDGET_H
#define CALIBRATIONVIEWERWIDGET_H

#include <QtGui/QAction>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGridLayout>
#include <QMouseEvent>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QHash>
#include <qdebug.h>

#include <iostream>

#include "calibrationfile.h"

using namespace std;

class CalibrationEdit;

/* This is the state of each camera combination
 * 0 : 'NO CONVERGENCE...' or reversed combination
 * 1 : Hide because 'NO CONVERGENCE...' or reversed
 * 2 : Useless because a combination with one or more cameras in common is selected
 * 3 : Hide because useless
 * 4 : Normal state, without anything special
 * 5 : Selected combination. To be a combination during the record */
enum Calibration{
    Failed = 0,
    HideByFail = 1,
    Useless = 2,
    HideByCalcul = 3,
    Normal = 4,
    Selected = 5
};

class CalibrationViewerWidget : public QMdiSubWindow {
    Q_OBJECT
public:
    CalibrationViewerWidget(QString path, QString name);

    /* Left clic */
    void clicOnCalibrationEdit(int lig);
    CalibrationEdit *getFileContain(){ return fileContain; }

private slots:
    void onRightClic();
    void menuProjectAction_triggered(QAction*);
    void menuCalibration(QAction*);
    
private:
    // Lars Aksel
    CalibrationFile* calibrationFile;

    QString fullPath;
    
    // Basic informations
    int view;

    // Function and bool to nicely initialize combinaisons
    void initializingCombinaisons();
    bool combinationsInitialised;
    int camerasNumber;

    // Function to calcul and display appropriate lines
    void calculUselessCombinations();
    void displayCombinations();

    bool changeFirstLine();
    void sortByColumnId();
    void sortCombo();
    void executeSortChanges(QHash<QString, int> order);
    QString getReversedKey(QString key);
    double getParameterByKey(QString key);
    void moveCursorTopCombinationsLines();
    void inverseFieldLines();

    bool isCombinationCamera(QString key);
    bool isLineCombinationCamera(int line);

    // Function to change combinations state
    void select();
    void calculateShowFailed();
    void calculateShowUseless();

    // Function to display view according to the private member 'view'
    void showTextView();
    void showTableView(int lig);

    // Information stored for the TextView
    CalibrationEdit *fileContain;
    QTextCharFormat format;

    // Hashmap to store the view of each of the combinaison
    QHash < QString, Calibration > combinations;
    bool showFailedBool;
    bool showUselessBool;
    int columnSort;
    bool comboSort;

    // Editable ?
    bool textEditable;
    QFrame *frame;
    
};


class CalibrationEdit : public QTextEdit {
public:
    CalibrationEdit(CalibrationViewerWidget *c) : calib(c){
        setReadOnly(true);
        setContextMenuPolicy(Qt::CustomContextMenu);
        setMouseTracking(true);
        setFont(QFont("Courier", 9));
    }

    QString getKeyByLine(int line){
        QStringList lines = calib->getFileContain()->toPlainText().split("\n");
        // Spliting to have the header, if there is one 
        QStringList header = lines[line].split(":");
        // If header size == 5 and header has 3 cells separated with '_'
        // Way of founding cameras combinations

        if(header[0].size()==5 && header[0].split("_").size()==3)
            return header[0];
        return "";
    }

protected:
    void mousePressEvent(QMouseEvent *event) {
        QTextEdit::mousePressEvent(event);
        if(event->button() != Qt::LeftButton)
            return;
        QTextCursor cursor = textCursor();
        int x = cursor.blockNumber();
        calib->clicOnCalibrationEdit(x);
    }
    // Deactivate it
    void mouseDoubleClickEvent(QMouseEvent *e){ }
private:
    CalibrationViewerWidget *calib;
};


#endif // CALIBRATIONVIEWERWIDGET_H
