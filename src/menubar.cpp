#include "menubar.h"

MenuBar::MenuBar(){

    file = new QMenu(tr("File"));

    newProject = new QAction(tr("New project"), this);
    newProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    connect(newProject, &QAction::triggered, this, [this](){emit actionTriggered(newProject);});

    loadProject = new QAction(tr("Load project"), this);
    loadProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(loadProject, &QAction::triggered, this, [this](){emit actionTriggered(loadProject);});

    closeProject = new QAction(tr("Close project"), this);
    closeProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    connect(closeProject, &QAction::triggered, this, [this](){emit actionTriggered(closeProject);});

    lastProject = new QAction(tr("Last project"), this);
    lastProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    connect(lastProject, &QAction::triggered, this, [this](){emit actionTriggered(lastProject);});

    loadConfigFile = new QAction(tr("Load Config File"), this);
    loadConfigFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    connect(loadConfigFile, &QAction::triggered, this, [this](){emit actionTriggered(loadConfigFile);});

    saveConfigFile = new QAction(tr("Save Config File"), this);
    saveConfigFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(saveConfigFile, &QAction::triggered, this, [this](){emit actionTriggered(saveConfigFile);});

    quit = new QAction(tr("Quit"), this);
    quit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quit, &QAction::triggered, this, [this](){emit actionTriggered(quit);});

    file->addAction(newProject);
    file->addAction(loadProject);
    file->addAction(closeProject);
    file->addAction(lastProject);
    file->addSeparator();
    file->addAction(loadConfigFile);
    file->addAction(saveConfigFile);
    file->addSeparator();
    file->addAction(quit);

    liveView = new QMenu(tr("Live View"));

    runLiveView = new QAction(tr("Run Live View"), this);
    runLiveView->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    runLiveView->setCheckable(true);
    connect(runLiveView, &QAction::triggered, this, [this](){emit actionTriggered(runLiveView);});

    updateImage = new QAction(tr("Update Image"), this);
    updateImage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    connect(updateImage, &QAction::triggered, this, [this](){emit actionTriggered(updateImage);});

    takePicture = new QAction(tr("Take Picture"), this);
    takePicture->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    takePicture->setDisabled(true);
    connect(takePicture, &QAction::triggered, this, [this](){emit actionTriggered(takePicture);});

    camerasAutoDetection = new QAction(tr("Camera Autodetection"), this);
    camerasAutoDetection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    camerasAutoDetection->setCheckable(true);
    camerasAutoDetection->setChecked(true);
    connect(camerasAutoDetection, &QAction::triggered, this, [this](){emit actionTriggered(camerasAutoDetection);});

    coordinates = new QAction(tr("Activate Crosshair"), this);
    coordinates->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    coordinates->setCheckable(true);
    connect(coordinates, &QAction::triggered, this, [this](){emit actionTriggered(coordinates);});

    integerCoordinates = new QAction(tr("Integer Coordinates"), this);
    integerCoordinates->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    integerCoordinates->setCheckable(true);
    integerCoordinates->setChecked(true);
    integerCoordinates->setDisabled(false);
    connect(integerCoordinates, &QAction::triggered, this, [this](){emit actionTriggered(integerCoordinates);});

    liveView->addAction(runLiveView);
    liveView->addAction(updateImage);
    liveView->addAction(takePicture);
    liveView->addSeparator();
    liveView->addAction(camerasAutoDetection);
    liveView->addSeparator();
    liveView->addAction(coordinates);
    liveView->addAction(integerCoordinates);

    window = new QMenu("Window");

    mosaicView = new QAction(tr("Mosaic View"), this);
    mosaicView->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    connect(mosaicView, &QAction::triggered, this, [this](){emit actionTriggered(mosaicView);});

    highQuality = new QAction(tr("High Quality"), this);
    highQuality->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    highQuality->setCheckable(true);
    connect(highQuality, &QAction::triggered, this, [this](){emit actionTriggered(highQuality);});

    hideCameraWidget = new QAction(tr("Hide Left Menu"), this);
    hideCameraWidget->setShortcut(QKeySequence(tr("Ctrl+H, Ctrl+M")));
    hideCameraWidget->setCheckable(true);
    connect(hideCameraWidget, &QAction::triggered, this, [this](){emit actionTriggered(hideCameraWidget);});

    hideToolBarWidget = new QAction(tr("Hide ToolBar"), this);
    hideToolBarWidget->setShortcut(QKeySequence(tr("Ctrl+H, Ctrl+T")));
    hideToolBarWidget->setCheckable(true);
    connect(hideToolBarWidget, &QAction::triggered, this, [this](){emit actionTriggered(hideToolBarWidget);});

    window->addAction(mosaicView);
    window->addAction(highQuality);
    window->addSeparator();
    window->addAction(hideCameraWidget);
    window->addAction(hideToolBarWidget);

    trackpoint = new QMenu("Trackpoint");

    runTrackpoint = new QAction(tr("Run Trackpoint"), this);
    runTrackpoint->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    connect(runTrackpoint, &QAction::triggered, this, [this](){emit actionTriggered(runTrackpoint);});

    connectToServer = new QAction(tr("Connect to Server"), this);
    connectToServer->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    connect(connectToServer, &QAction::triggered, this, [this](){emit actionTriggered(connectToServer);});

    trackpoint->addAction(runTrackpoint);
    trackpoint->addAction(connectToServer);


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

QAction *MenuBar::getTakePicture(){ return takePicture; }

QAction *MenuBar::getCameraAutoDetection(){ return camerasAutoDetection; }

QAction *MenuBar::getHideCameraWidget(){ return hideCameraWidget; }

QAction *MenuBar::getHideToolBarWidget(){ return hideToolBarWidget; }

