
#ifndef CALIBRATIONFILE_WIDGET_H
#define CALIBRATIONFILE_WIDGET_H

#include <qwidget.h>
#include <qtreewidget.h>
#include <qlineedit.h>
#include "calibrationfile.h"
#include "calibrationfileopenglwidget.h"

class CalibrationFileWidget : public QWidget {
    Q_OBJECT
public:
    CalibrationFileWidget(QWidget* parent, CalibrationFile* file);

    void initUI();

private slots:
    void filtersChanged();
    void combinationClicked(QTreeWidgetItem* item, int column);
    void cameraClicked(QTreeWidgetItem* item, int column);
    void combinationSelectionChanged();
    void cameraSelectionChanged();

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
    TrackPoint::CameraCombination* lastSelectedCombination = nullptr;

    void updateCameraCombinationTable(TrackPoint::CameraCombination*);
    void updateCombinationTable();
    void updateCameraTable();
    //void updateFilters();
    void updateFiltersCamerasOnly(TrackPoint::CameraCombination*);
};

#endif