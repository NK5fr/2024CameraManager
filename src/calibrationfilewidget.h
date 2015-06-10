
#ifndef CALIBRATIONFILE_WIDGET_H
#define CALIBRATIONFILE_WIDGET_H

#include <qwidget.h>
#include <qtreewidget.h>
#include "calibrationfile.h"
#include "calibrationfileopenglwidget.h"

class CalibrationFileWidget : public QWidget {
    Q_OBJECT
public:
    CalibrationFileWidget(CalibrationFile* file);

    void initUI();

private slots:
    void combinationClicked(QTreeWidgetItem* item, int column);
    void combinationSelectionChanged();

private:
    CalibrationFile* calibFile;
    CalibrationFileOpenGLWidget* combinationPreviewWidget;
    QTreeWidget* filterList;
    QTreeWidget* combinationList;
    QTreeWidget* cameraTable;

    void updateCameraTable(TrackPoint::CameraCombination*);
};

#endif