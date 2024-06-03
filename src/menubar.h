#ifndef MENUBAR_H
#define MENUBAR_H

#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>

class MenuBar : public QMenuBar {
public:
    MenuBar();

    QMenu *getFile();
    QMenu *getLiveView();
    QMenu *getWindow();
    QMenu *getTrackPoint();

    QAction *getRunLiveView();
    QAction *getActivateCoordinates();
    QAction *getUpdateImage();
    QAction *getCameraAutoDetection();
    QAction *getIntegerCoordinates();
    QAction *getHighQuality();

    QAction *getHideCameraWidget();
    QAction *getHideToolBarWidget();



private:
    QMenu *file;
    QMenu *liveView;
    QMenu *window;
    QMenu *trackpoint;

    QAction *newProject;
    QAction *loadProject;
    QAction *closeProject;
    QAction *loadConfigFile;
    QAction *saveConfigFile;

    QAction *runLiveView;
    QAction *updateImage;
    QAction *camerasAutoDetection;
    QAction *coordinates;
    QAction *integerCoordinates;

    QAction *mosaicView;
    QAction *highQuality;
    QAction *hideCameraWidget;
    QAction *hideToolBarWidget;
};

#endif // MENUBAR_H
