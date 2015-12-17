#include "menubar.h"

MenuBar::MenuBar(){

    file = new QMenu(tr("File"));

    newProject = new QAction(tr("New project"), this);
    newProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));

    loadProject = new QAction(tr("Load project"), this);
    loadProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

    loadConfigFile = new QAction(tr("Load Config File"), this);
    loadConfigFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));

    saveConfigFile = new QAction(tr("Save Config File"), this);
    saveConfigFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

    file->addAction(newProject);
    file->addAction(loadProject);
    file->addSeparator();
    file->addAction(loadConfigFile);
    file->addAction(saveConfigFile);
    file->addSeparator();
    file->addAction("Quit");

    liveView = new QMenu(tr("Live View"));

    runLiveView = new QAction(tr("Run Live View"), this);
    runLiveView->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    runLiveView->setCheckable(true);

    updateImage = new QAction(tr("Update Image"), this);
    updateImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));

    camerasAutoDetection = new QAction(tr("Camera Autodetection"), this);
    camerasAutoDetection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    camerasAutoDetection->setCheckable(true);
    camerasAutoDetection->setChecked(true);

    coordinates = new QAction(tr("Activate Coordinates"), this);
    coordinates->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    coordinates->setCheckable(true);

    integerCoordinates = new QAction(tr("Integer Coordinates"), this);
    integerCoordinates->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    integerCoordinates->setCheckable(true);
    integerCoordinates->setDisabled(true);

    liveView->addAction(runLiveView);
    liveView->addAction(updateImage);
    liveView->addSeparator();
    liveView->addAction(camerasAutoDetection);
    liveView->addSeparator();
    liveView->addAction(coordinates);
    liveView->addAction(integerCoordinates);

    window = new QMenu("Window");

    mosaicView = new QAction(tr("Mosaic View"), this);
    mosaicView->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));

    highQuality = new QAction(tr("High Quality"), this);
    highQuality->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    highQuality->setCheckable(true);

    hideCameraWidget = new QAction(tr("Hide Left Menu"), this);
    hideCameraWidget->setShortcut(QKeySequence(tr("Ctrl+H, Ctrl+M")));
    hideCameraWidget->setCheckable(true);

    hideToolBarWidget = new QAction(tr("Hide ToolBar"), this);
    hideToolBarWidget->setShortcut(QKeySequence(tr("Ctrl+H, Ctrl+T")));
    hideToolBarWidget->setCheckable(true);

    window->addAction(mosaicView);
    window->addAction(highQuality);
    window->addSeparator();
    window->addAction(hideCameraWidget);
    window->addAction(hideToolBarWidget);

    trackpoint = new QMenu("Trackpoint");
    trackpoint->addAction("Run Trackpoint");
    trackpoint->addAction("Connect to Server");


    addMenu(file);
    addMenu(liveView);
    addMenu(window);
    addMenu(trackpoint);
}

QMenu *MenuBar::getFile(){ return file; }

QMenu *MenuBar::getLiveView(){ return liveView; }

QMenu *MenuBar::getWindow(){ return window; }

QMenu *MenuBar::getTrackPoint(){ return trackpoint; }

QAction *MenuBar::getActivateCoordinates(){ return coordinates; }

QAction *MenuBar::getIntegerCoordinates(){ return integerCoordinates; }

QAction *MenuBar::getHighQuality(){ return highQuality; }

QAction *MenuBar::getRunLiveView(){ return runLiveView; }

QAction *MenuBar::getUpdateImage(){ return updateImage; }

QAction *MenuBar::getCameraAutoDetection(){ return camerasAutoDetection; }

QAction *MenuBar::getHideCameraWidget(){ return hideCameraWidget; }

QAction *MenuBar::getHideToolBarWidget(){ return hideToolBarWidget; }

