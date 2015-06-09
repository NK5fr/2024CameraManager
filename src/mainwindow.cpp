#include <QtWidgets/qmenubar.h>
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidgetItem>
#include <QStandardItem>
#include <QtWidgets/QAction>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QPoint>
#include <QtWidgets/QPushButton>
#include <QIcon>
#include <QDebug>
#include <QDir>
#include <QtWidgets/QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QtWidgets/QMessageBox>
#include "qprocess.h" //gs
#include "qdebug.h"   //gs

#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "abstractcameramanager.h"
#include "testcameramanager.h"
#include "emptycameramanager.h"
#include "flycameramanager.h"
#include "configfileviewerwidget.h"
#include "socketviewerwidget.h"
#include "calibrationviewerwidget.h"
#include "imageviewerwidget.h"

using namespace std;


bool Ui::crosshair = false, Ui::crosshairReal = false, Ui::forceHighQuality = false;

/* Constructor of MainWindow*/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), selectedCameraManager(-1), tdc(this), detectCameras(true), tup(this) {

    // g jan 2015: icons og filnavn er definert i cameramanager.qrc
    propertiesIcons[0] = QIcon(":/icons/camera").pixmap(16, 16);
    propertiesIcons[1] = QIcon(":/icons/folder").pixmap(16, 16);
    propertiesIcons[2] = QIcon(":/icons/folder_camera").pixmap(16, 16);
    
    // Lars Aksel - 10.03.2015 - Save memory by loading icons only once...
    icons[0] = QIcon(":/icons/folder");
    icons[1] = QIcon(":/icons/coordinates");
    icons[2] = QIcon(":/icons/config");
    icons[3] = QIcon(":/icons/camera");
    icons[4] = QIcon(":/icons/exe");
    icons[5] = QIcon(":/icons/2d");
    icons[6] = QIcon(":/icons/img");
    icons[7] = QIcon(":/icons/file");

    ui->setupUi(this);

    // Lars Aksel - 05.02.2015 - Load TrackPoint-settings
    loadDefaultTrackPointSettings();
    setupTrackPointTab();

    cameraManagers.push_back(new FlyCameraManager());
    cameraManagers.push_back(new TestCameraManager());

    for (unsigned int i = 0; i < cameraManagers.size(); ++i){
        AbstractCameraManager* manager = cameraManagers.at(i);
        manager->setMainWindow(this);
        ui->selectCameraManager->addItem(manager->getName().c_str());
    }

    /* Disable left menu header */
    ui->CamerasWidget_2->setTitleBarWidget(new QWidget(this));

    /* Disable right click on ToolBar, and on the left QDockMenu, because the right click
    provides an undesirable menu*/
    ui->toolBar->toggleViewAction()->setVisible(false);
    ui->CamerasWidget_2->toggleViewAction()->setVisible(false);

    /* MenuBar */
    bar = new MenuBar();
    setMenuBar(bar);

    /* SIGNALS/SLOTS */
    connect(ui->cameraTree, SIGNAL(clicked(const QModelIndex)), this, SLOT(cameraTree_itemClicked(const QModelIndex)));
    connect(ui->cameraTree, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(cameraTree_itemDoubleClicked(const QModelIndex)));
    connect(bar->getFile(), SIGNAL(triggered(QAction*)), this, SLOT(menuProjectAction_triggered(QAction*)));
    connect(bar->getLiveView(), SIGNAL(triggered(QAction*)), this, SLOT(menuBarClicked(QAction*)));
    connect(bar->getWindow(), SIGNAL(triggered(QAction*)), this, SLOT(menuBarClicked(QAction*)));
    connect(bar->getTrackPoint(), SIGNAL(triggered(QAction*)), this, SLOT(menuBarClicked(QAction*)));
    connect(ui->projectTree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(expandFilePath_ProjectTree(QTreeWidgetItem*)));
    connect(ui->projectTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(collapsedFilePath_ProjectTree(QTreeWidgetItem*)));

    // Lars Aksel - 05.02.2015
    connect(ui->loadDefaultCameraProperties, SIGNAL(clicked()), this, SLOT(loadDefaultCameraProperties_clicked()));
    connect(ui->quickSaveCameraProperties, SIGNAL(clicked()), this, SLOT(quickSaveCameraSettings()));
    connect(ui->quickLoadCameraProperties, SIGNAL(clicked()), this, SLOT(quickLoadCameraSettings()));

    /* Title */
    setWindowTitle("Qt Camera Manager");

    /* Thread to detect automatically new cameras*/
    tdc.start();

    setFocusPolicy(Qt::TabFocus);
}

/* Destructor of MainWindow */
MainWindow::~MainWindow() {
    detectCameras = false;
    on_actionLiveView_toggled(false);
    tdc.wait();
    tup.wait();
    for (int i = 0; i < cameraManagers.size(); i++) {
        delete cameraManagers[i];
        cameraManagers[i] = nullptr;
    }
    //delete bar;
    //bar = nullptr;
    //delete ui;
}

/* Adding or removing the MdiSubWindow furnished in parameter according to the add parameter */
void MainWindow::modifySubWindow(QMdiSubWindow* in, bool add) {
    Q_CHECK_PTR(ui);
    Q_CHECK_PTR(ui->centralwidget);
    Q_CHECK_PTR(in);
    if (add){
        (ui->centralwidget->addSubWindow(in))->show();
    } else {
        ui->centralwidget->removeSubWindow(in);
        //delete in;
    }
}

/* Slot called when index of the cameras project changed */
void MainWindow::on_SelectCameras_currentIndexChanged(int index) {
    if (selectedCameraManager >= 0){
        AbstractCameraManager* camManager = cameraManagers.at(selectedCameraManager);
        camManager->activateLiveView(false);
        camManager->desactiveAllCameras();
        camManager->getPropertiesWidget()->hide();
    }
    selectedCameraManager = index;
    AbstractCameraManager* cm = cameraManagers.at(selectedCameraManager); 
    ui->cameraTree->setModel(cm->getModel());
    ui->propertiesContainer->addWidget(cm->getPropertiesWidget());
    cm->getPropertiesWidget()->show();
    /*on_Detect_clicked();*/
    if (ui->actionLiveView->isChecked()) cm->activateLiveView(true);
}

/** TOOLBAR FUNCTIONS **/
/* Clic on LiveView button */
void MainWindow::on_actionLiveView_toggled(bool arg1) {
    bar->getRunLiveView()->setChecked(arg1);
    ui->actionUpdateImages->setEnabled(!arg1);
    cameraManagers.at(selectedCameraManager)->activateLiveView(arg1);
    cameraManagers.at(selectedCameraManager)->setTrackPointProperty(&trackPointProperty);
    if (arg1) tup.start();
    //else 
}

/* Clic on UpdateImage button */
void MainWindow::on_actionUpdateImages_triggered() {
    cameraManagers.at(selectedCameraManager)->setTrackPointProperty(&trackPointProperty);
    cameraManagers.at(selectedCameraManager)->updateImages();
}

/* Clic on mosaic button */
void MainWindow::on_actionMosaic_triggered() {
    ui->centralwidget->tileSubWindows();
}

/* Clic on ActionCrossHair button */
void MainWindow::on_actionCrosshair_toggled(bool arg1) {
    Ui::crosshair = arg1;
    bar->getActivateCoordinates()->setChecked(arg1);

    /* Need to discheck and disable the real coordinates button */
    ui->actionCrosshairReal->setEnabled(arg1);
    bar->getIntegerCoordinates()->setEnabled(arg1);
    if (arg1 == false) {
        ui->actionCrosshairReal->setChecked(false);
        bar->getIntegerCoordinates()->setChecked(false);
    }
    emit activateCrosshair(Ui::crosshair);
}

/* Clic on ActionCrosshairReal button */
void MainWindow::on_actionCrosshairReal_toggled(bool arg1) {
    bar->getIntegerCoordinates()->setChecked(arg1);
    Ui::crosshairReal = arg1;
}

/* Clic on actionHighQuality button */
void MainWindow::on_actionHighQuality_toggled(bool arg1) {
    bar->getHighQuality()->setChecked(arg1);
    Ui::forceHighQuality = arg1;
}

/////////////////////////////////////////////
/** CAMERA TREE AND ASSIMILATED FUNCTIONS **/
/////////////////////////////////////////////

/* Create group in CameraTree */
void MainWindow::on_addGroup() {
    ui->cameraTree->edit(cameraManagers.at(selectedCameraManager)->addGroup());
}

/* Rename group or camera in Camera Tree */
void MainWindow::on_editItem() {
    ui->cameraTree->edit(ui->cameraTree->currentIndex());
}

/* Put back initial camera or group name in Camera Tree */
void MainWindow::on_resetItem() {
    cameraManagers.at(selectedCameraManager)->resetItem(ui->cameraTree->currentIndex());
}

/* Remove group in CameraTree */
void MainWindow::on_deleteGroup() {
    if (!ui->cameraTree->currentIndex().isValid()) return;
    cameraManagers.at(selectedCameraManager)->removeGroup(ui->cameraTree->currentIndex());
    cameraTree_itemClicked(ui->cameraTree->currentIndex());
}

/* Click on an item in CameraTree */
void MainWindow::cameraTree_itemClicked(const QModelIndex index) {
    QString str = "";
    bool editable, deleteable;
    int icon = 0;
    cameraManagers.at(selectedCameraManager)->cameraTree_itemClicked(index, str, icon, editable, deleteable);
    cameraManagers.at(selectedCameraManager)->setTrackPointProperty(&trackPointProperty);

    ui->label->setText(str);
    //if( icon>=0 && icon < 3 ) ui->propertiesIcon->setPixmap(propertiesIcons[icon]);
    //ui->label->adjustSize();
}

void MainWindow::cameraTree_itemDoubleClicked(const QModelIndex index) {
    QStandardItem* clicked = cameraManagers.at(selectedCameraManager)->getModel()->itemFromIndex(index);
    if (index.column() == 0) return;
    if (clicked->data(CameraRole).isValid()) {
        AbstractCamera* camera = (clicked == nullptr) ? nullptr : reinterpret_cast<AbstractCamera*>(clicked->data(CameraRole).value<quintptr>());
        for (int i = 0; i < cameraManagers.at(selectedCameraManager)->getActiveCameraEntries().size(); i++) {
            if (camera == cameraManagers.at(selectedCameraManager)->getActiveCameraEntries()[i].camera) {
                QMdiSubWindow* win = cameraManagers.at(selectedCameraManager)->getActiveCameraEntries()[i].window;
                if (win->isMaximized()) {
                    win->showNormal();
                } else {
                    win->showMaximized();
                }
                break;
            }
        }
    }
}

/* Right click in CameraTree */
void MainWindow::on_CameraTree_customContextMenuRequested(const QPoint &pos) {
    /* Creating a menu with allowed actions */
    QMenu *menu = new QMenu();
    menu->addAction("Add Group");
    menu->addAction("Remove group");
    menu->addSeparator();
    menu->addAction("Edit Name");
    menu->addAction("Reset Name");

    menu->popup(cursor().pos());
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuCameraAction_triggered(QAction*)));
}

/* Click on item in CameraTree popup Menu */    
void MainWindow::menuCameraAction_triggered(QAction *action) {
    if (action->text() == "Add Group")
        on_addGroup();
    else if (action->text() == "Remove group")
        on_deleteGroup();
    else if (action->text() == "Edit Name")
        on_editItem();
    else if (action->text() == "Reset Name")
        on_resetItem();
}

/* Click on item in MenuBar. The 'File' menu is not connected with this slot,
 * but with the
 * Mostly, this function calls already existring function or slots because the MenuBar
 * is only another way to do things */
void MainWindow::menuBarClicked(QAction* action) {
    if (action->text() == "Run Live View"){
        bool b = bar->getRunLiveView()->isChecked();
        on_actionLiveView_toggled(b);
        ui->actionLiveView->setChecked(b);
        bar->getUpdateImage()->setDisabled(b);
        if (!tup.isRunning())
            tup.start();

    } else if (action->text() == "Update Image")
        on_actionUpdateImages_triggered();

    else if (action->text() == "Camera Autodetection"){
        detectCameras = bar->getCameraAutoDetection()->isChecked();
        if (detectCameras)
            tdc.start();

    } else if (action->text() == "Activate Coordinates"){
        bool b = bar->getActivateCoordinates()->isChecked();
        ui->actionCrosshair->setChecked(b);
        on_actionCrosshair_toggled(b);

    } else if (action->text() == "Integer Coordinates"){
        bool b = bar->getIntegerCoordinates()->isChecked();
        ui->actionCrosshairReal->setChecked(b);
        on_actionCrosshairReal_toggled(b);

    } else if (action->text() == "Mosaic View")
        on_actionMosaic_triggered();

    else if (action->text() == "High Quality"){
        bool b = bar->getHighQuality()->isChecked();
        ui->actionHighQuality->setChecked(b);
        on_actionHighQuality_toggled(b);

    } else if (action->text() == "Hide Left Menu")
        ui->CamerasWidget_2->setVisible(!bar->getHideCameraWidget()->isChecked());

    else if (action->text() == "Hide ToolBar")
        ui->toolBar->setVisible(!bar->getHideToolBarWidget()->isChecked());
    else if (action->text() == "Connect to Server") {
        SocketViewerWidget* svw = new SocketViewerWidget();
        ui->centralwidget->addSubWindow(svw);
        svw->showMaximized();
    }
    else if (action->text() == "Run Trackpoint"){
        //QString trackPointPath = QFileDialog::getExistingDirectory(this, "Trackpoint folder", "/");
        QString executable = QFileDialog::getOpenFileName(this, "Launch the trackpoint exe", "/", "(*.exe)");

        trackPointProcess = new ExternalProcess();
        //trackPointProcess->setProcessChannelMode(QProcess::MergedChannels);
        //trackPointProcess->setWorkingDirectory(trackPointPath);
        //trackPointProcess->start("tracert www.google.com");
        trackPointProcess->start(executable);
        //ui->centralwidget->closeAllSubWindows();
        ui->centralwidget->addSubWindow(trackPointProcess->getTextEdit());
        trackPointProcess->getTextEdit()->showMaximized();
    }
}

//////////////////////
/** THREAD METHODS **/
//////////////////////

// When Thread DetectCamera is launched, it call this method to find new camera
void MainWindow::startCameraDetection() {
    while (detectCameras){
        ui->cameraTree->setExpanded(cameraManagers.at(selectedCameraManager)->detectNewCamerasAndExpand(), true);
        //printf("Searching for new cameras...\n");
        QThread::msleep(100);
        //ui->cameraTree->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}

// When Thread UpdateProperties is launched, it call this method to update the selected camera properties
void MainWindow::startUpdateProperties() {
    while (bar->getRunLiveView()->isChecked()){
        //printf("Updating properties...\n");
        
        cameraManagers.at(selectedCameraManager)->updateProperties();
        //QThread::
        QThread::msleep(250);
    }
}


///////////////////////////////////////////
/** PROJECT TREE AND ASSIMILATED METHODS */
///////////////////////////////////////////

/*Action for Double clicking on one of the Project Tree's Item */
void MainWindow::on_ProjectTree_doubleClicked(const QModelIndex &index) {
    QTreeWidgetItem *item = ui->projectTree->selectedItems().at(0);
    if (item->child(0) != NULL) return;
    QString fileName = item->text(0);
    item = item->parent();
    QString folderName = item->text(0);
    while (item->parent() != NULL){
        item = item->parent();
        folderName = item->text(0) + "/" + folderName;
    }
    QString selectedProjectPath = QString(projectsPath + "/" + folderName);
    ui->centralwidget->closeAllSubWindows();
    //TODO change this condition to something that allows more than one name.

    if (QFileInfo(selectedProjectPath + "/" + fileName).isDir()) {
        item->setExpanded(!item->isExpanded());
        return;
    }
    if (fileName.contains("options")){
        /* If the item is Config File */
        ConfigFileViewerWidget *cfvw = new ConfigFileViewerWidget(selectedProjectPath + "/" + fileName);
        ui->centralwidget->addSubWindow(cfvw);
        cfvw->showMaximized();
    } else if (fileName.contains("socket")){
        /* Socket file, with 3D datas */
        SocketViewerWidget* svw = new SocketViewerWidget(selectedProjectPath, fileName.toUtf8().constData(), calibrationPath);
        ui->centralwidget->closeAllSubWindows();
        ui->centralwidget->addSubWindow(svw);
        svw->showMaximized();
    } else if (fileName.contains("calibration_summary")){
        /* Calibration file */
        calibrationPath = selectedProjectPath + "/" + fileName;
        //CalibrationViewerWidget* cvw = new CalibrationViewerWidget(selectedProjectPath, fileName.toUtf8().constData());
        CalibrationFile* calibFile = new CalibrationFile(calibrationPath);
        CalibrationFileWidget* calibWidget = new CalibrationFileWidget(calibFile);
        ui->centralwidget->addSubWindow(calibWidget);
        calibWidget->showMaximized();
    } else if (fileName.endsWith(".pgm")){
        /* Grupper image file */
        ImageViewerWidget* ivw = new ImageViewerWidget(selectedProjectPath, fileName);
        ui->centralwidget->addSubWindow(ivw);
        ivw->showMaximized();
    } else {
        // Open as text-file... (Must create: TextFileViewerWidget-class)
        ConfigFileViewerWidget *cfvw = new ConfigFileViewerWidget(selectedProjectPath + "/" + fileName);
        ui->centralwidget->addSubWindow(cfvw);
        cfvw->showMaximized();
    }
}

/* Action for Right clicking on the project Tree zone
 * Will pop-up a menu listing all actions available */
void MainWindow::on_ProjectTree_customContextMenuRequested(const QPoint &pos) {
    /* Creating a menu with allowed actions */
    QMenu *menu = new QMenu();
    menu->addAction("Load project");
    menu->addAction("Close project");
    menu->popup(cursor().pos());
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuProjectAction_triggered(QAction*)));
}

/*List of all actions made on items in the ProjectTree Menu after having right clicked the project tree (customContextMenuRequested method)*/
void MainWindow::menuProjectAction_triggered(QAction *action) {
    if (action->text() == "Load project"){
        QString folderPath = QFileDialog::getExistingDirectory(this, "Open the trackpoint folder", "/", QFileDialog::ShowDirsOnly); // grethe 2015.01.22, snu slashen
        projectsPath = QFileInfo(folderPath).absolutePath();
        //  QString folderName = folderPath.split("\\").at(folderPath.split("\\").size()-1);
        QString folderName = folderPath.split("/").at(folderPath.split("/").size() - 1);  // grethe 2015.01.22, snu slashen

        cout << "Foldername: " + folderName.toStdString() << endl; // grethe 19.01.2015

        if (true) {
            /* Check if the project has already been loaded */
            for (int i = 0; i < ui->projectTree->invisibleRootItem()->childCount(); i++){
                if (ui->projectTree->invisibleRootItem()->child(i)->text(0) == folderName)
                    return;
            }
            /* Removing the '/' followed by the folder name for having only the path and not the name */
            QString projectPath = folderPath.mid(0, folderPath.lastIndexOf("/")); // grethe 2015.01.22, snu slashen
            cout << "Loading..\n" << endl;

            calibrationPath = QString(); // Temporary fix...
            createTreeFolder(ui->projectTree->invisibleRootItem(), projectPath, folderName);
            ui->projectTree->sortByColumn(3, Qt::AscendingOrder); // Sort by filetype...
            //ui->projectTree->resizeColumnToContents(0);
            /* Expand trackpoint folder, his child named application, and his grandchild named input */
            ui->projectTree->invisibleRootItem()->child(0)->setExpanded(true);  // feiler her på setExpanded - GS 2015-01-19
            /*
            if (ui->projectTree->invisibleRootItem()->child(0)->child(0) != NULL && ui->projectTree->invisibleRootItem()->child(0)->child(0)->text(0) == "application"){
                ui->projectTree->invisibleRootItem()->child(0)->child(0)->setExpanded(true);
                if (ui->projectTree->invisibleRootItem()->child(0)->child(0)->child(0) != NULL &&
                    ui->projectTree->invisibleRootItem()->child(0)->child(0)->child(0)->text(0) == "input"){
                    ui->projectTree->invisibleRootItem()->child(0)->child(0)->child(0)->setExpanded(true);
                }
            }*/
        } else {
            QMessageBox::information(this, "Error", "Foldername does not contain the word \"trackpoint\"!");
        }
    }
    //else If action = closing a project
    else if (action->text() == "Close project") {
        if (ui->projectTree->selectedItems().size() == 0) return; // No selected items...
        QTreeWidgetItem *item = ui->projectTree->selectedItems().at(0);
        if (item != NULL && ui->projectTree->indexOfTopLevelItem(item) != -1 && item->text(0) != QString("Config File")){
            ui->projectTree->invisibleRootItem()->removeChild(item);
            cout << "Removed" << endl;
        }
        if (item == NULL) cout << "NULL" << endl;
    }
}

// Lars Aksel - 10.03.2015 - Filter for "project-supported" files
bool isProjectSupported(QString& path) {
    if (path.endsWith(".exe", Qt::CaseInsensitive)) return true;
    if (path.endsWith(".pgm", Qt::CaseInsensitive)) return true;
    if (path.endsWith(".dat", Qt::CaseInsensitive)) return true;
    if (path.endsWith(".txt", Qt::CaseInsensitive)) return true;
    if (path.contains("socket", Qt::CaseInsensitive) && path.endsWith(".dat", Qt::CaseInsensitive)) return true;
    //if (path.endsWith(".log", Qt::CaseInsensitive)) return true;
    //if (path.endsWith(".trj", Qt::CaseInsensitive)) return true;
    //if (path.endsWith(".match", Qt::CaseInsensitive)) return true;
    return false;
}

void MainWindow::createTreeFolder(QTreeWidgetItem *parent, const QString& path, const QString& name) {
    /* Creating path and opening DIR */
    QString fullPath = QString(path + "/" + name);  // grethe 2015-01-22, fjerner \\ og setter inn / istd
    //QString fullPath = QString(path);   // path og name er lik - fjerner derfor name .. GS 2015-01-2015


    QDir myFile(fullPath);
    if (!myFile.exists()){ return; }
    QFileInfoList list = myFile.entryInfoList();

    /* Creating the projectItem, reading the files contained by the project, and adding them into the projectItem */
    QTreeWidgetItem *rootItem = new QTreeWidgetItem();
    rootItem->setText(0, tr(name.toUtf8().constData()));
    rootItem->setIcon(0, icons[0]);   // G: kan her bruke icon fra cameramanager.qrc.
    rootItem->setData(0, Qt::UserRole, fullPath);
    parent->addChild(rootItem);

    for (int i = 0; i < list.size(); i++){
        if (list.at(i).fileName() != tr(".") && list.at(i).fileName() != tr("..")) {
            if (list.at(i).isDir()) {
                QTreeWidgetItem* folderItem = new QTreeWidgetItem();
                folderItem->setText(0, list.at(i).fileName().toUtf8().constData());
                folderItem->setIcon(0, icons[0]);   // G: kan her bruke icon fra cameramanager.qrc.
                folderItem->setData(0, Qt::UserRole, list.at(i).absoluteFilePath().toUtf8().constData());
                rootItem->addChild(folderItem);
                QFileInfoList folderlist = QDir(list.at(i).absoluteFilePath()).entryInfoList();
                if (folderlist.size() > 0) {
                    QTreeWidgetItem* subFolderItem = new QTreeWidgetItem();
                    //subFolderItem->setText(0, "<PLACEHOLDER>");
                    subFolderItem->setData(0, Qt::UserRole, "<PLACEHOLDER>");
                    folderItem->addChild(subFolderItem);
                }
            } else if (list.at(i).isFile()) {
                if (isProjectSupported(list.at(i).absoluteFilePath())) {
                    createTreeItem(rootItem, list.at(i).fileName(), list.at(i).absoluteFilePath());
                }
            }
        }
    }
    rootItem->sortChildren(0, Qt::AscendingOrder);
}

QString getHumanReadableFileSize(qint64 fileSize) {
    qint64 B = 1;
    qint64 kB = 1024 * B;
    qint64 MB = 1024 * kB;
    qint64 GB = 1024 * MB;
    qint64 TB = 1024 * GB;
    QString res;
    if (fileSize > TB) {
        res = QString::number((double) fileSize / TB, 'f', 2) + " TB";
    } else if (fileSize > GB) {
        res = QString::number((double) fileSize / GB, 'f', 2) + " GB";
    } else if (fileSize > MB) {
        res = QString::number((double) fileSize / MB, 'f', 2) + " MB";
    } else if (fileSize > kB) {
        res = QString::number((double) fileSize / kB, 'f', 2) + " kB";
    } else {
        res = QString::number(fileSize) + " byte";
    }
    return res;
}

QString getFileTypeString(FileInfo::FileType fileType) {
    if (fileType == FileInfo::FileType::CalibrationFile) return QString("Calibration");
    if (fileType == FileInfo::FileType::ConfigurationFile) return QString("Configuration");
    if (fileType == FileInfo::FileType::Executable) return QString("Executable");
    if (fileType == FileInfo::FileType::Folder) return QString("Folder");
    if (fileType == FileInfo::FileType::ImageFile) return QString("Image");
    if (fileType == FileInfo::FileType::SocketFile) return QString("Socket");
    else return QString("Text");
}

FileInfo::FileType getFileType(QFileInfo file) {
    QString filename = file.fileName();
    if (file.isDir()) return FileInfo::FileType::Folder;
    if (filename.endsWith(".exe", Qt::CaseInsensitive)) return FileInfo::FileType::Executable;
    if (filename.contains("calibration_summary", Qt::CaseInsensitive)) return FileInfo::FileType::CalibrationFile;
    if (filename.contains("option", Qt::CaseInsensitive) && filename.endsWith(".txt")) return FileInfo::FileType::ConfigurationFile;
    if (filename.endsWith(".pgm", Qt::CaseInsensitive)) return FileInfo::FileType::ImageFile;
    if (filename.contains("socket", Qt::CaseInsensitive)) return FileInfo::FileType::SocketFile;
    else return FileInfo::FileType::TextFile;
}

void MainWindow::createTreeItem(QTreeWidgetItem *parent, QString name, QString path) {
    QTreeWidgetItem *childItem = new QTreeWidgetItem();
    childItem->setText(0, name);
    int iconptr = 7; // :/icons/file
    if (name.contains("socket")) {
        iconptr = 1; // :/icons/coordinates
    } else if (name.contains("option") && name.endsWith(".txt")) {
        iconptr = 2; // :/icons/config
    } else if (name.contains("calibration_summary", Qt::CaseInsensitive)){
        iconptr = 3; // :/icons/camera
        if (calibrationPath.isEmpty()) {
            QTreeWidgetItem *item = parent;
            calibrationPath = name;
            while (item != NULL){
                calibrationPath = item->text(0) + "/" + calibrationPath;
                item = item->parent();
            }
            calibrationPath = projectsPath + "/" + calibrationPath;
        }
    } else if (name.endsWith(".exe")) {
        iconptr = 4; // :/icons/exe
    } else if (parent->text(0) == "output" && name.endsWith(".dat")) {
        iconptr = 5; // :/icons/2d
    } else if (name.endsWith(".pgm")) {
        iconptr = 6; // :/icons/img
    }
    childItem->setIcon(0, icons[iconptr]);
    QFileInfo fileInfo(path);
    QString fileSizeText = getHumanReadableFileSize(fileInfo.size());

    childItem->setText(1, fileSizeText);
    childItem->setText(2, fileInfo.lastModified().toString("yyyy.MM.dd hh:mm:ss"));
    childItem->setText(3, getFileTypeString(getFileType(fileInfo)));
    parent->addChild(childItem);
}

void MainWindow::loadDefaultTrackPointSettings_clicked() {
    loadDefaultTrackPointSettings();
    ui->trackPointEnabled->setChecked(trackPointProperty.trackPointPreview);
    ui->filteredImagePreviewEnabled->setChecked(trackPointProperty.filteredImagePreview);
    ui->showCoordinateLabelEnabled->setChecked(trackPointProperty.showCoordinates);
    ui->showMinSepCircleEnabled->setChecked(trackPointProperty.showMinSepCircle);
    ui->thresholdValueEdit->setText(QString::number(trackPointProperty.thresholdValue));
    //delete ui->thresholdValueEdit->validator();
    ui->thresholdValueEdit->setValidator(new QIntValidator(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax, ui->thresholdValueEdit));
    ui->thresholdSlider->setValue(trackPointProperty.thresholdValue);
    ui->thresholdSlider->setRange(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax);
    ui->subwinValueEdit->setText(QString::number(trackPointProperty.subwinValue));
    //delete ui->subwinValueEdit->validator();
    ui->subwinValueEdit->setValidator(new QIntValidator(trackPointProperty.subwinMin, trackPointProperty.subwinMax, ui->subwinValueEdit));
    ui->subwinSlider->setValue(trackPointProperty.subwinValue);
    ui->subwinSlider->setRange(trackPointProperty.subwinMin, trackPointProperty.subwinMax);
    ui->minPointValueEdit->setText(QString::number(trackPointProperty.minPointValue));
    //delete ui->minPointValueEdit->validator();
    ui->minPointValueEdit->setValidator(new QIntValidator(trackPointProperty.minPointMin, trackPointProperty.minPointMax, ui->minPointValueEdit));
    ui->minPointSlider->setValue(trackPointProperty.minPointValue);
    ui->minPointSlider->setRange(trackPointProperty.minPointMin, trackPointProperty.minPointMax);
    ui->maxPointValueEdit->setText(QString::number(trackPointProperty.maxPointValue));
    //delete ui->maxPointValueEdit->validator();
    ui->maxPointValueEdit->setValidator(new QIntValidator(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax, ui->maxPointValueEdit));
    ui->maxPointSlider->setValue(trackPointProperty.maxPointValue);
    ui->maxPointSlider->setRange(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax);
    ui->minSepValueEdit->setText(QString::number(trackPointProperty.minSepValue));
    //delete ui->minSepValueEdit->validator();
    ui->minSepValueEdit->setValidator(new QIntValidator(trackPointProperty.minSepMin, trackPointProperty.minSepMax, ui->minSepValueEdit));
    ui->minSepSlider->setValue(trackPointProperty.minSepValue);
    ui->minSepSlider->setRange(trackPointProperty.minSepMin, trackPointProperty.minSepMax);
}

void MainWindow::loadDefaultCameraProperties_clicked() {
    //bool tempStop = cameraManagers[selectedCameraManager]->isUpdateProperties();
    //if (tempStop) cameraManagers[selectedCameraManager]->setUpdateProperties(false);
    cameraManagers[selectedCameraManager]->loadPropertiesDefaults();
    //if (tempStop) cameraManagers[selectedCameraManager]->setUpdateProperties(true);
}

void MainWindow::loadDefaultTrackPointSettings() {
    QString path = QDir::currentPath() + "/props/defaultTrackPointSettings.ini";
    loadTrackPointSettingsFromFile(path);
}

void MainWindow::on_TrackPointChecked(int state) {
    trackPointProperty.trackPointPreview = (state == Qt::Checked);
    cameraManagers[selectedCameraManager]->updateContainer();
}

void MainWindow::on_FilteredImageChecked(int state) {
    trackPointProperty.filteredImagePreview = (state == Qt::Checked);
    cameraManagers[selectedCameraManager]->updateContainer();
}

void MainWindow::on_ShowCoordinateLabelChecked(int state) {
    trackPointProperty.showCoordinates = (state == Qt::Checked);
    cameraManagers[selectedCameraManager]->updateContainer();
}

void MainWindow::on_RemoveDuplicatesChecked(int state) {
    trackPointProperty.removeDuplicates = (state == Qt::Checked);
    cameraManagers[selectedCameraManager]->updateContainer();
}

void MainWindow::on_ShowMinSepCircleChecked(int state) {
  trackPointProperty.showMinSepCircle = (state == Qt::Checked);
  cameraManagers[selectedCameraManager]->updateContainer();
}

void MainWindow::on_TrackPointValueChanged() {
    QLineEdit* lineEdit = (QLineEdit*) sender();
    if (lineEdit == ui->thresholdValueEdit) {
        trackPointProperty.thresholdValue = lineEdit->text().toInt();
        ui->thresholdSlider->setValue(lineEdit->text().toInt());
    }
    if (lineEdit == ui->subwinValueEdit) {
        trackPointProperty.subwinValue = lineEdit->text().toInt();
        ui->subwinSlider->setValue(lineEdit->text().toInt());
    }
    if (lineEdit == ui->minPointValueEdit) {
        if (lineEdit->text().toInt() > trackPointProperty.maxPointValue) {
            trackPointProperty.minPointValue = trackPointProperty.maxPointValue;
            ui->minPointSlider->setValue(trackPointProperty.maxPointValue);
        } else {
            trackPointProperty.minPointValue = lineEdit->text().toInt();
            ui->minPointSlider->setValue(lineEdit->text().toInt());
            ui->minPointSlider->setRange(trackPointProperty.minPointMin, min(trackPointProperty.maxPointValue, trackPointProperty.minPointMax));
        }
    }
    if (lineEdit == ui->maxPointValueEdit) {
        if (lineEdit->text().toInt() < trackPointProperty.minPointValue) {
            trackPointProperty.maxPointValue = trackPointProperty.minPointValue;
            ui->maxPointSlider->setValue(trackPointProperty.minPointValue);
        } else {
            trackPointProperty.maxPointValue = lineEdit->text().toInt();
            ui->maxPointSlider->setValue(lineEdit->text().toInt());
            ui->maxPointSlider->setRange(max(trackPointProperty.minPointValue, trackPointProperty.maxPointMin), trackPointProperty.maxPointMax);
        }
    }
    if (lineEdit == ui->minSepValueEdit) {
        trackPointProperty.minSepValue = lineEdit->text().toInt();
        ui->minSepSlider->setValue(lineEdit->text().toInt());
    }
}

void MainWindow::on_TrackPointSliderValueChanged(int value) {
    QSlider* slider = (QSlider*) sender();
    if (slider == ui->thresholdSlider) {
        ui->thresholdValueEdit->setText(QString::number(value));
        trackPointProperty.thresholdValue = ui->thresholdValueEdit->text().toInt();
        cameraManagers[selectedCameraManager]->updateContainer();
    }
    if (slider == ui->subwinSlider) {
        ui->subwinValueEdit->setText(QString::number(value));
        trackPointProperty.subwinValue = ui->subwinValueEdit->text().toInt();
        cameraManagers[selectedCameraManager]->updateContainer();
    }
    if (slider == ui->minPointSlider) {
        if (value > trackPointProperty.maxPointValue) {
            ui->minPointValueEdit->setText(QString::number(trackPointProperty.maxPointValue));
            trackPointProperty.minPointValue = trackPointProperty.maxPointValue;
            ui->minPointSlider->setRange(trackPointProperty.minPointMin, min(trackPointProperty.maxPointValue, trackPointProperty.minPointMax));
            ui->minPointSlider->update();
            cameraManagers[selectedCameraManager]->updateContainer();
        } else {
            ui->minPointValueEdit->setText(QString::number(min( trackPointProperty.maxPointValue, value)));
            trackPointProperty.minPointValue = ui->minPointValueEdit->text().toInt();
            ui->minPointSlider->setRange(trackPointProperty.minPointMin, min(trackPointProperty.maxPointValue, trackPointProperty.minPointMax));
            ui->minPointSlider->update();
            cameraManagers[selectedCameraManager]->updateContainer();
        }
    }
    if (slider == ui->maxPointSlider) {
        if (value < trackPointProperty.minPointValue) {
            ui->maxPointValueEdit->setText(QString::number(trackPointProperty.minPointValue));
            trackPointProperty.maxPointValue = trackPointProperty.minPointValue;
            ui->maxPointSlider->setRange(max(trackPointProperty.minPointValue, trackPointProperty.maxPointMin), trackPointProperty.maxPointMax);
            ui->minPointSlider->update();
            cameraManagers[selectedCameraManager]->updateContainer();
        } else {
            ui->maxPointValueEdit->setText(QString::number(max(trackPointProperty.minPointValue, value)));
            trackPointProperty.maxPointValue = ui->maxPointValueEdit->text().toInt();
            ui->maxPointSlider->setRange(max(trackPointProperty.minPointValue, trackPointProperty.maxPointMin), trackPointProperty.maxPointMax);
            ui->minPointSlider->update();
            cameraManagers[selectedCameraManager]->updateContainer();
        }
    }
    if (slider == ui->minSepSlider) {
        ui->minSepValueEdit->setText(QString::number(value));
        trackPointProperty.minSepValue = ui->minSepValueEdit->text().toInt();
        cameraManagers[selectedCameraManager]->updateContainer();
    }
}

void MainWindow::quickSaveCameraSettings() {
    QString quickFile(QDir::currentPath() + "/props/quickfile_camerasettings.ini");
    cameraManagers[selectedCameraManager]->savePropertiesToFile(quickFile);
}

void MainWindow::quickLoadCameraSettings() {
    QString quickFile(QDir::currentPath() + "/props/quickfile_camerasettings.ini");
    QFile file(quickFile);
    if (!file.exists()) {
        QMessageBox::information(this, "Error", "No quicksave-file exists!");
    } else {
        std::vector<CameraProperty> prop;
        cameraManagers[selectedCameraManager]->loadPropertiesFromFile(quickFile, prop);
        cameraManagers[selectedCameraManager]->updateProperties(prop);
    }
}

void MainWindow::quickSaveTrackPointSettings() {
    QString quickFile(QDir::currentPath() + "/props/quickfile_trackpoint.ini");
    saveTrackPointSettingsToFile(quickFile, trackPointProperty);
}

void MainWindow::quickLoadTrackPointSettings() {
    QString quickFile(QDir::currentPath() + "/props/quickfile_trackpoint.ini");
    QFile file(quickFile);
    if (!file.exists()) {
        QMessageBox::information(this, "Error", "No quicksave-file exists!");
    } else {
        loadTrackPointSettingsFromFile(quickFile);
        ui->trackPointEnabled->setChecked(trackPointProperty.trackPointPreview);
        ui->filteredImagePreviewEnabled->setChecked(trackPointProperty.filteredImagePreview);
        ui->showCoordinateLabelEnabled->setChecked(trackPointProperty.showCoordinates);
        ui->showMinSepCircleEnabled->setChecked(trackPointProperty.showMinSepCircle);
        ui->removeDuplicatPointsEnabled->setChecked(trackPointProperty.removeDuplicates);
        ui->thresholdValueEdit->setText(QString::number(trackPointProperty.thresholdValue));
        delete ui->thresholdValueEdit->validator();
        ui->thresholdValueEdit->setValidator(new QIntValidator(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax, ui->thresholdValueEdit));
        ui->thresholdSlider->setValue(trackPointProperty.thresholdValue);
        ui->thresholdSlider->setRange(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax);
        ui->subwinValueEdit->setText(QString::number(trackPointProperty.subwinValue));
        delete ui->subwinValueEdit->validator();
        ui->subwinValueEdit->setValidator(new QIntValidator(trackPointProperty.subwinMin, trackPointProperty.subwinMax, ui->subwinValueEdit));
        ui->subwinSlider->setValue(trackPointProperty.subwinValue);
        ui->subwinSlider->setRange(trackPointProperty.subwinMin, trackPointProperty.subwinMax);
        ui->minPointValueEdit->setText(QString::number(trackPointProperty.minPointValue));
        delete ui->minPointValueEdit->validator();
        ui->minPointValueEdit->setValidator(new QIntValidator(trackPointProperty.minPointMin, trackPointProperty.minPointMax, ui->minPointValueEdit));
        ui->minPointSlider->setValue(trackPointProperty.minPointValue);
        ui->minPointSlider->setRange(trackPointProperty.minPointMin, trackPointProperty.minPointMax);
        ui->maxPointValueEdit->setText(QString::number(trackPointProperty.maxPointValue));
        delete ui->maxPointValueEdit->validator();
        ui->maxPointValueEdit->setValidator(new QIntValidator(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax, ui->maxPointValueEdit));
        ui->maxPointSlider->setValue(trackPointProperty.maxPointValue);
        ui->maxPointSlider->setRange(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax);
        ui->minSepValueEdit->setText(QString::number(trackPointProperty.minSepValue));
        delete ui->minSepValueEdit->validator();
        ui->minSepValueEdit->setValidator(new QIntValidator(trackPointProperty.minSepMin, trackPointProperty.minSepMax, ui->minSepValueEdit));
        ui->minSepSlider->setValue(trackPointProperty.minSepValue);
        ui->minSepSlider->setRange(trackPointProperty.minSepMin, trackPointProperty.minSepMax);
    }
}

void MainWindow::loadTrackPointSettingsFromFile(QString& filepath) {
    QSettings settings(filepath, QSettings::IniFormat);
    TrackPointProperty prop;
    settings.beginGroup("TrackPoint");
    prop.trackPointPreview = settings.value("trackpoint_preview").toBool();
    prop.filteredImagePreview = settings.value("filtered_image_preview").toBool();
    prop.showCoordinates = settings.value("show_coordinate_labels").toBool();
    prop.showMinSepCircle = settings.value("show_minsep_circle").toBool();
    prop.removeDuplicates = settings.value("remove_duplicates").toBool();
    settings.endGroup();
    settings.beginGroup("Threshold");
    prop.thresholdText = settings.value("text").toString();
    prop.thresholdValue = settings.value("value").toInt();
    prop.thresholdMin = settings.value("min").toInt();
    prop.thresholdMax = settings.value("max").toInt();
    settings.endGroup();
    settings.beginGroup("SubWin");
    prop.subwinText = settings.value("text").toString();
    prop.subwinValue = settings.value("value").toInt();
    prop.subwinMin = settings.value("min").toInt();
    prop.subwinMax = settings.value("max").toInt();
    settings.endGroup();
    settings.beginGroup("MinimalPointSize");
    prop.minPointText = settings.value("text").toString();
    prop.minPointValue = settings.value("value").toInt();
    prop.minPointMin = settings.value("min").toInt();
    prop.minPointMax = settings.value("max").toInt();
    settings.endGroup();
    settings.beginGroup("MaximalPointSize");
    prop.maxPointText = settings.value("text").toString();
    prop.maxPointValue = settings.value("value").toInt();
    prop.maxPointMin = settings.value("min").toInt();
    prop.maxPointMax = settings.value("max").toInt();
    settings.endGroup();
    settings.beginGroup("MinimalSeparation");
    prop.minSepText = settings.value("text").toString();
    prop.minSepValue = settings.value("value").toInt();
    prop.minSepMin = settings.value("min").toInt();
    prop.minSepMax = settings.value("max").toInt();
    settings.endGroup();
    trackPointProperty = prop;
}

void MainWindow::saveTrackPointSettingsToFile(QString& filepath, TrackPointProperty& props) {
    QSettings settings(filepath, QSettings::IniFormat);
    settings.beginGroup("TrackPoint");
    settings.setValue("trackpoint_preview", props.trackPointPreview);
    settings.setValue("filtered_image_preview", props.filteredImagePreview);
    settings.setValue("show_coordinate_labels", props.showCoordinates);
    settings.setValue("show_minsep_circle", props.showMinSepCircle);
    settings.setValue("remove_duplicates", props.removeDuplicates);
    settings.endGroup();
    settings.beginGroup("Threshold");
    settings.setValue("text", props.thresholdText);
    settings.setValue("value", QString::number(props.thresholdValue));
    settings.setValue("min", QString::number(props.thresholdMin));
    settings.setValue("max", QString::number(props.thresholdMax));
    settings.endGroup();
    settings.beginGroup("SubWin");
    settings.setValue("text", props.subwinText);
    settings.setValue("value", QString::number(props.subwinValue));
    settings.setValue("min", QString::number(props.subwinMin));
    settings.setValue("max", QString::number(props.subwinMax));
    settings.endGroup();
    settings.beginGroup("MinimalPointSize");
    settings.setValue("text", props.minPointText);
    settings.setValue("value", QString::number(props.minPointValue));
    settings.setValue("min", QString::number(props.minPointMin));
    settings.setValue("max", QString::number(props.minPointMax));
    settings.endGroup();
    settings.beginGroup("MaximalPointSize");
    settings.setValue("text", props.maxPointText);
    settings.setValue("value", QString::number(props.maxPointValue));
    settings.setValue("min", QString::number(props.maxPointMin));
    settings.setValue("max", QString::number(props.maxPointMax));
    settings.endGroup();
    settings.beginGroup("MinimalSeparation");
    settings.setValue("text", props.minSepText);
    settings.setValue("value", QString::number(props.minSepValue));
    settings.setValue("min", QString::number(props.minSepMin));
    settings.setValue("max", QString::number(props.minSepMax));
    settings.endGroup();
}

void MainWindow::setupTrackPointTab() {
    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    QGridLayout* trackPointLayout = new QGridLayout();
    QLabel* trackpointLabel = new QLabel("TrackPoint Preview:");
    ui->trackPointEnabled = new QCheckBox();
    ui->trackPointEnabled->setChecked(trackPointProperty.trackPointPreview);
    trackPointLayout->addWidget(trackpointLabel, 0, 0);
    trackPointLayout->addWidget(ui->trackPointEnabled, 0, 1);
    connect(ui->trackPointEnabled, SIGNAL(stateChanged(int)), this, SLOT(on_TrackPointChecked(int)));

    QLabel* filteredImageLabel = new QLabel("Filtered Image Preview:");
    ui->filteredImagePreviewEnabled = new QCheckBox();
    ui->filteredImagePreviewEnabled->setChecked(trackPointProperty.filteredImagePreview);
    trackPointLayout->addWidget(filteredImageLabel, 1, 0);
    trackPointLayout->addWidget(ui->filteredImagePreviewEnabled, 1, 1);
    connect(ui->filteredImagePreviewEnabled, SIGNAL(stateChanged(int)), this, SLOT(on_FilteredImageChecked(int)));

    QLabel* showCoordinatesLabel = new QLabel("Show Coordinate Labels:");
    ui->showCoordinateLabelEnabled = new QCheckBox();
    ui->showCoordinateLabelEnabled->setChecked(trackPointProperty.showCoordinates);
    trackPointLayout->addWidget(showCoordinatesLabel, 2, 0);
    trackPointLayout->addWidget(ui->showCoordinateLabelEnabled, 2, 1);
    connect(ui->showCoordinateLabelEnabled, SIGNAL(stateChanged(int)), this, SLOT(on_ShowCoordinateLabelChecked(int)));

    QLabel* removeDuplicatesLabel = new QLabel("Remove Duplicates:");
    ui->removeDuplicatPointsEnabled = new QCheckBox();
    ui->removeDuplicatPointsEnabled->setChecked(trackPointProperty.removeDuplicates);
    trackPointLayout->addWidget(removeDuplicatesLabel, 3, 0);
    trackPointLayout->addWidget(ui->removeDuplicatPointsEnabled, 3, 1);
    connect(ui->removeDuplicatPointsEnabled, SIGNAL(stateChanged(int)), this, SLOT(on_RemoveDuplicatesChecked(int)));

    QLabel* showMinSepLabel = new QLabel("Show minimal separation circle:");
    ui->showMinSepCircleEnabled = new QCheckBox();
    ui->showMinSepCircleEnabled->setChecked(trackPointProperty.showMinSepCircle);
    trackPointLayout->addWidget(showMinSepLabel, 4, 0);
    trackPointLayout->addWidget(ui->showMinSepCircleEnabled, 4, 1);
    connect(ui->showMinSepCircleEnabled, SIGNAL(stateChanged(int)), this, SLOT(on_ShowMinSepCircleChecked(int)));

    QLabel* thresholdLabel = new QLabel(trackPointProperty.thresholdText);
    ui->thresholdValueEdit = new QLineEdit(QString::number(trackPointProperty.thresholdValue));
    ui->thresholdValueEdit->setValidator(new QIntValidator(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax, ui->thresholdValueEdit));
    ui->thresholdSlider = new QSlider(Qt::Horizontal);
    ui->thresholdSlider->setValue(trackPointProperty.thresholdValue);
    ui->thresholdSlider->setRange(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax);
    trackPointLayout->addWidget(thresholdLabel, 5, 0);
    trackPointLayout->addWidget(ui->thresholdValueEdit, 5, 1);
    trackPointLayout->addWidget(ui->thresholdSlider, 5, 2);
    connect(ui->thresholdValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));

    QLabel* subwinLabel = new QLabel(trackPointProperty.subwinText);
    ui->subwinValueEdit = new QLineEdit(QString::number(trackPointProperty.subwinValue));
    ui->subwinValueEdit->setValidator(new QIntValidator(trackPointProperty.subwinMin, trackPointProperty.subwinMax, ui->subwinValueEdit));
    ui->subwinSlider = new QSlider(Qt::Horizontal);
    ui->subwinSlider->setValue(trackPointProperty.subwinValue);
    ui->subwinSlider->setRange(trackPointProperty.subwinMin, trackPointProperty.subwinMax);
    trackPointLayout->addWidget(subwinLabel, 6, 0);
    trackPointLayout->addWidget(ui->subwinValueEdit, 6, 1);
    trackPointLayout->addWidget(ui->subwinSlider, 6, 2);
    connect(ui->subwinValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->subwinSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));

    QLabel* minPointLabel = new QLabel(trackPointProperty.minPointText);
    ui->minPointValueEdit = new QLineEdit(QString::number(trackPointProperty.minPointValue));
    ui->minPointValueEdit->setValidator(new QIntValidator(trackPointProperty.minPointMin, trackPointProperty.minPointMax, ui->minPointValueEdit));
    ui->minPointSlider = new QSlider(Qt::Horizontal);
    ui->minPointSlider->setValue(trackPointProperty.minPointValue);
    //ui->minPointSlider->setRange(trackPointProperty.minPointMin, trackPointProperty.minPointMax);
    ui->minPointSlider->setRange(trackPointProperty.minPointMin, min(trackPointProperty.maxPointValue, trackPointProperty.minPointMax));
    trackPointLayout->addWidget(minPointLabel, 7, 0);
    trackPointLayout->addWidget(ui->minPointValueEdit, 7, 1);
    trackPointLayout->addWidget(ui->minPointSlider, 7, 2);
    connect(ui->minPointValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->minPointSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));

    QLabel* maxPointLabel = new QLabel(trackPointProperty.maxPointText);
    ui->maxPointValueEdit = new QLineEdit(QString::number(trackPointProperty.maxPointValue));
    ui->maxPointValueEdit->setValidator(new QIntValidator(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax, ui->maxPointValueEdit));
    ui->maxPointSlider = new QSlider(Qt::Horizontal);
    ui->maxPointSlider->setValue(trackPointProperty.maxPointValue);
    //ui->maxPointSlider->setRange(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax);
    ui->maxPointSlider->setRange(max(trackPointProperty.minPointValue, trackPointProperty.maxPointMin), trackPointProperty.maxPointMax);
    trackPointLayout->addWidget(maxPointLabel, 8, 0);
    trackPointLayout->addWidget(ui->maxPointValueEdit, 8, 1);
    trackPointLayout->addWidget(ui->maxPointSlider, 8, 2);
    connect(ui->maxPointValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->maxPointSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));
    
    QLabel* minSepLabel = new QLabel(trackPointProperty.minSepText);
    ui->minSepValueEdit = new QLineEdit(QString::number(trackPointProperty.minSepValue));
    ui->minSepValueEdit->setValidator(new QIntValidator(trackPointProperty.minSepMin, trackPointProperty.minSepMax, ui->minSepValueEdit));
    ui->minSepSlider = new QSlider(Qt::Horizontal);
    ui->minSepSlider->setValue(trackPointProperty.minSepValue);
    ui->minSepSlider->setRange(trackPointProperty.minSepMin, trackPointProperty.minSepMax);
    trackPointLayout->addWidget(minSepLabel, 9, 0);
    trackPointLayout->addWidget(ui->minSepValueEdit, 9, 1);
    trackPointLayout->addWidget(ui->minSepSlider, 9, 2);
    connect(ui->minSepValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->minSepSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));
    trackPointLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

    vBoxLayout->addLayout(trackPointLayout);

    QGridLayout* buttonGridLayout = new QGridLayout();
    QPushButton* quickSave = new QPushButton("Quick Save");
    buttonGridLayout->addWidget(quickSave, 0, 0);
    connect(quickSave, SIGNAL(clicked()), this, SLOT(quickSaveTrackPointSettings()));
    QPushButton* quickLoad = new QPushButton("Quick Load");
    buttonGridLayout->addWidget(quickLoad, 0, 1);
    connect(quickLoad, SIGNAL(clicked()), this, SLOT(quickLoadTrackPointSettings()));
    buttonGridLayout->setAlignment(Qt::AlignBottom);
    QPushButton* defaultSettingsButton = new QPushButton("Load Defaults");
    connect(defaultSettingsButton, SIGNAL(clicked()), this, SLOT(loadDefaultTrackPointSettings_clicked()));
    buttonGridLayout->addWidget(defaultSettingsButton, 1, 0, 1, 2);

    vBoxLayout->addLayout(buttonGridLayout);

    ui->trackPointWidget->setLayout(vBoxLayout);
}

void MainWindow::expandFilePath_ProjectTree(QTreeWidgetItem* item) {
    QString fullPath = item->data(0, Qt::UserRole).value<QString>();
    if (item->childCount() == 1) {
        if (item->child(0)->data(0, Qt::UserRole).value<QString>().contains("<PLACEHOLDER>")) {
            item->removeChild(item->child(0));
        }
    } else return;

    QFileInfoList list = QDir(fullPath).entryInfoList();
    for (int i = 0; i < list.size(); i++){
        if (list.at(i).fileName() != tr(".") && list.at(i).fileName() != tr("..")) {
            if (list.at(i).isDir()) {
                QTreeWidgetItem* folderItem = new QTreeWidgetItem();
                folderItem->setText(0, list.at(i).fileName().toUtf8().constData());
                folderItem->setIcon(0, icons[0]);
                folderItem->setData(0, Qt::UserRole, list.at(i).absoluteFilePath().toUtf8().constData());
                item->addChild(folderItem);

                QDir folder(list.at(i).absoluteFilePath());
                if (folder.entryInfoList().size() > 0) {
                    QTreeWidgetItem* subFolderItem = new QTreeWidgetItem();
                    //subFolderItem->setText(0, "<PLACEHOLDER>");
                    subFolderItem->setData(0, Qt::UserRole, "<PLACEHOLDER>");
                    folderItem->addChild(subFolderItem);
                }
            } else if (list.at(i).isFile()) {
                if (isProjectSupported(list.at(i).absoluteFilePath())) {
                    createTreeItem(item, list.at(i).fileName(), list.at(i).absoluteFilePath());
                }
            }
        }
    }
    ui->projectTree->resizeColumnToContents(0);
    ui->projectTree->resizeColumnToContents(1);
    ui->projectTree->resizeColumnToContents(2);
}

void MainWindow::collapsedFilePath_ProjectTree(QTreeWidgetItem* item) {
    ui->projectTree->resizeColumnToContents(0);
    ui->projectTree->resizeColumnToContents(1);
    ui->projectTree->resizeColumnToContents(2);
}
