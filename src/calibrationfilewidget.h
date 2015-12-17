
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef CALIBRATIONFILE_WIDGET_H
#define CALIBRATIONFILE_WIDGET_H

#include <qwidget.h>
#include <qtreewidget.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <QMdiSubWindow>
#include "calibrationfile.h"
#include "calibrationfileopenglwidget.h"

class CalibrationFileWidget : public QMdiSubWindow {
    Q_OBJECT
public:
    CalibrationFileWidget(QWidget* parent, CalibrationFile* file);

    void initUI();

    inline bool isValid() { return this->valid; }

private slots:
    void filtersChanged();
    void combinationClicked(QTreeWidgetItem* item, int column);
    void cameraClicked(QTreeWidgetItem* item, int column);
    void combinationSelectionChanged();
    void cameraSelectionChanged();
    void showTextClicked();
    void showVisualizeClicked();

private:
    CalibrationFile* calibFile;
    CalibrationFileOpenGLWidget* combinationPreviewWidget;
    QTreeWidget* filterList;
    QTreeWidget* combinationList;
    QTreeWidget* selectedCombinationTable;
    QTreeWidget* cameraTable;
    QColor failedColor;
    QColor warningColor;
    QColor okColor;
    QPushButton* showTextButton;
    QPushButton* showVisualizeButton;
    QWidget* visualizeWidget;
    QWidget* textWidget;
    bool valid;
    TrackPoint::CameraCombination* lastSelectedCombination = nullptr;

    void updateCameraCombinationTable(TrackPoint::CameraCombination*);
    void updateCombinationTable();
    void updateCameraTable();
    //void updateFilters();
    void updateFiltersCamerasOnly(TrackPoint::CameraCombination*);
};

#endif