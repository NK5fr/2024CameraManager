#include <QMenuBar>
#include <QMenu>
#include <QWidgetItem>
#include <QStandardItem>
#include <QAction>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPoint>
#include <QPushButton>
#include <QIcon>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>

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
#include "sleeper.h"
#include "qprocess.h" //gs
#include "qdebug.h"   //gs

using namespace std;

bool Ui::crosshair = false, Ui::crosshairReal = false, Ui::forceHighQuality = false;

/* Constructor of MainWindow*/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), selectedCameraManager(-1), tdc(this), detectCameras(true), tup(this) {

    // g jan 2015: icons og filnavn er definert i cameramanager.qrc
    propertiesIcons[0] = QIcon(":/icons/camera").pixmap(16, 16);
    propertiesIcons[1] = QIcon(":/icons/folder").pixmap(16, 16);
    propertiesIcons[2] = QIcon(":/icons/folder_camera").pixmap(16, 16);

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
    connect(ui->cameraTree, SIGNAL(clicked(const QModelIndex)), this, SLOT(on_CameraTree_itemClicked(const QModelIndex)));
    connect(bar->getFile(), SIGNAL(triggered(QAction*)), this, SLOT(menuProjectAction_triggered(QAction*)));
    connect(bar->getLiveView(), SIGNAL(triggered(QAction*)), this, SLOT(menuBarClicked(QAction*)));
    connect(bar->getWindow(), SIGNAL(triggered(QAction*)), this, SLOT(menuBarClicked(QAction*)));
    connect(bar->getTrackPoint(), SIGNAL(triggered(QAction*)), this, SLOT(menuBarClicked(QAction*)));

    // Lars Aksel - 05.02.2015
    connect(ui->loadDefaultCameraProperties, SIGNAL(released()), this, SLOT(on_LoadDefaults_Pushed()));

    /* Title */
    setWindowTitle("Qt Camera Manager");

    /* Thread to detect automatically new cameras*/
    tdc.start();

    setFocusPolicy(Qt::TabFocus);
}

/* Destructor of MainWindow */
MainWindow::~MainWindow() {
    //delete ui;
    exit(0);
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
    tup.start();
}

/* Clic on UpdateImage button */
void MainWindow::on_actionUpdateImages_triggered() {
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
    on_CameraTree_itemClicked(ui->cameraTree->currentIndex());
}

/* Click on an item in CameraTree */
void MainWindow::on_CameraTree_itemClicked(const QModelIndex index) {
    QString str = "";
    bool editable, deleteable;
    int icon = 0;
    cameraManagers.at(selectedCameraManager)->cameraTree_itemClicked(index, str, icon, editable, deleteable);

    ui->label->setText(str);
    //if( icon>=0 && icon < 3 ) ui->propertiesIcon->setPixmap(propertiesIcons[icon]);
    //ui->label->adjustSize();
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
    else if (action->text() == "Run Trackpoint"){
        /*if(defined(WIN64) || defined(WIN32)){*/
        QString executable = QFileDialog::getOpenFileName(this, "Launch the trackpoint exe", "/", "(*.exe)");
        //QProcess *process = new QProcess();
        process_file = "tmp.txt";
        QString path = QFileDialog::getExistingDirectory(this, "Trackpoint folder", "/"); // gs
        process.setWorkingDirectory(path);		                              // gs
        process.setProcessChannelMode(QProcess::MergedChannels);						// gs
        process.setStandardOutputFile(process_file);                        // gs


        text_edit = new QTextEdit();  // legger til et tekstfelt i hovedvinduet
        setCentralWidget(text_edit);
        text_edit->setText(process.readAllStandardOutput());  // gs	
        text_edit->setText("Kjøring av TrackPoint pågår"); // gs 

        //connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(executeFinished(process)));
        //connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOutput(process))); //gs
        //connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(executeError(QProcess::ProcessError)));
        process_timer.setInterval(100);
        process_timer.setSingleShot(false);
        //connect(&process_timer, SIGNAL(timeout()), this, SLOT(appendOutput()));


        //execute(executable, process);
        //process->start(executable);
        //process->waitForStarted();					  // gs

        //	process->startDetached(executable);		// gs
        //	process->waitForFinished(-1);			// gs
        //	process->close();						// gs

        /* }*/
        //QDebugStream cout(std::cout, textEdit);  // gs
        //  QDebugStream cerr(std::cerr, textEdit);  // gs

        connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput())); //gs
        options.clear();  // gs
        process.start(executable, options); // gs
        process.waitForFinished();  // gs

        //	process.setReadChannelMode(QProcess::MergedChannels);


        /* Problem - få trackpoint output til ui---kontrollen text_edit.. -  */
    }
}
//gs
void MainWindow::execute(QString command, QProcess &p) {
    text_edit->append(p.readAllStandardOutput());
    QFile::remove(process_file);
    process_file_pos = 0;
    process.start(command);
    process_timer.start();


}
//gs
void MainWindow::printOutput() {
    text_edit->setPlainText(process.readAllStandardOutput());
}
//gs
void MainWindow::readStdOutput(QProcess &p) {
    text_edit->append(p.readAllStandardOutput());
}

// gs
void MainWindow::appendOutput() {
    QFile file(process_file);
    if (!file.open(QIODevice::ReadOnly)) return;

    if (file.size() > process_file_pos) {
        file.seek(process_file_pos);
        text_edit->moveCursor(QTextCursor::End);
        text_edit->insertPlainText(file.readAll());
        process_file_pos = file.pos();
    }
    file.close();
}
//gs

void MainWindow::executeFinished(QProcess &p) {
    process_timer.stop();
    appendOutput();
}
//gs
void MainWindow::executeError(QProcess::ProcessError) {
    process_timer.stop();
    appendOutput();
}
// END GS TEST trackpoint output //

//////////////////////
/** THREAD METHODS **/
//////////////////////

/* When Thread DetectCamera is launched, it call this method to find new camera */
void MainWindow::startCameraDetection() {
    while (detectCameras){
        if (ui != nullptr) {
            ui->cameraTree->setExpanded(cameraManagers.at(selectedCameraManager)->detectNewCamerasAndExpand(), true);
            //cout << "Searching for new cameras..." << endl;
            Sleeper::sleep(1);
        } else {
            detectCameras = false;
        }
    }
}

/* When Thread UpdateProperties is launched, it call this method to update the selected camera properties */
void MainWindow::startUpdateProperties() {
    while (bar->getRunLiveView()->isChecked()){
        //cout << "Updating properties..." << endl;
        cameraManagers.at(selectedCameraManager)->updateProperties();
        cameraManagers.at(selectedCameraManager)->setTrackPointProperty(&trackPointProperty);
        Sleeper::sleep(1);
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
    //TODO change this condition to something that allows more than one name.
    if (fileName.contains("options")){
        /* If the item is Config File */
        ConfigFileViewerWidget *cfvw = new ConfigFileViewerWidget(selectedProjectPath + "/" + fileName);

        ui->centralwidget->addSubWindow(cfvw);
        cfvw->show();

    } else if (fileName.contains("socket")){
        /* Socket file, with 3D datas */
        SocketViewerWidget *svw = new SocketViewerWidget(selectedProjectPath, fileName.toUtf8().constData(), calibrationPath);
        ui->centralwidget->addSubWindow(svw);
        svw->show();

    } else if (fileName.contains("calibration")){
        /* Calibration file */
        CalibrationViewerWidget *cvw = new CalibrationViewerWidget(selectedProjectPath, fileName.toUtf8().constData());
        ui->centralwidget->addSubWindow(cvw);
        cvw->show();

    } else if (fileName.contains("grupper")){
        /* Grupper image file */

        /* Hide the left menu to have almost all the screen */
        bar->getHideCameraWidget()->setChecked(true);
        ui->CamerasWidget_2->setVisible(false);

        /* Calculating the top left point of the ImageViewerWidget */
        QSize size = ui->centralwidget->size();
        size.setWidth(size.width() + ui->CamerasWidget_2->width());

        ImageViewerWidget *ivw = new ImageViewerWidget(selectedProjectPath, fileName, size);
        ui->centralwidget->addSubWindow(ivw);
        ivw->showMaximized();
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

        if (folderName.toLower().contains("trackpoint")){
            /* Check if the project has already been loaded */
            for (int i = 0; i < ui->projectTree->invisibleRootItem()->childCount(); i++){
                if (ui->projectTree->invisibleRootItem()->child(i)->text(0) == folderName)
                    return;
            }
            /* Removing the '/' followed by the folder name for having only the path and not the name */
            QString projectPath = folderPath.mid(0, folderPath.lastIndexOf("/")); // grethe 2015.01.22, snu slashen
            cout << "Loading\n" << endl;

            createTreeFolder(ui->projectTree->invisibleRootItem(), projectPath, folderName);

            /* Expand trackpoint folder, his child named application, and his grandchild named input */
            ui->projectTree->invisibleRootItem()->child(0)->setExpanded(true);  // feiler her på setExpanded - GS 2015-01-19
            if (ui->projectTree->invisibleRootItem()->child(0)->child(0) != NULL && ui->projectTree->invisibleRootItem()->child(0)->child(0)->text(0) == "application"){
                ui->projectTree->invisibleRootItem()->child(0)->child(0)->setExpanded(true);
                if (ui->projectTree->invisibleRootItem()->child(0)->child(0)->child(0) != NULL &&
                    ui->projectTree->invisibleRootItem()->child(0)->child(0)->child(0)->text(0) == "input"){
                    ui->projectTree->invisibleRootItem()->child(0)->child(0)->child(0)->setExpanded(true);
                }
            }
        }
    }
    //else If action = closing a project
    else if (action->text() == "Close project"){
        QTreeWidgetItem *item = ui->projectTree->selectedItems().at(0);
        if (item != NULL && ui->projectTree->indexOfTopLevelItem(item) != -1 && item->text(0) != QString("Config File")){
            ui->projectTree->invisibleRootItem()->removeChild(item);
            cout << "Removed" << endl;
        }
        if (item == NULL) cout << "NULL" << endl;
    }
}

void MainWindow::createTreeFolder(QTreeWidgetItem *parent, QString path, QString name) {
    /* Creating path and opening DIR */
    QString fullPath = QString(path + "/" + name);  // grethe 2015-01-22, fjerner \\ og setter inn / istd
    //QString fullPath = QString(path);   // path og name er lik - fjerner derfor name .. GS 2015-01-2015

    QDir myFile(fullPath);
    if (!myFile.exists()){ return; }
    QFileInfoList list = myFile.entryInfoList();

    /* Creating the projectItem, reading the files contained by the project, and adding them into the projectItem */
    QTreeWidgetItem *rootItem = new QTreeWidgetItem();
    rootItem->setText(0, tr(name.toUtf8().constData()));
    rootItem->setIcon(0, QIcon(":/icons/folder"));   // G: kan her bruke icon fra cameramanager.qrc.
    parent->addChild(rootItem);

    for (int i = 0; i < list.size(); i++){
        if (list.at(i).fileName() != tr(".") && list.at(i).fileName() != tr("..")) {
            if (list.at(i).isDir())
                createTreeFolder(rootItem, fullPath, list.at(i).fileName());
            else
                createTreeItem(rootItem, list.at(i).fileName());
        }
    }
    rootItem->sortChildren(0, Qt::AscendingOrder);
}

void MainWindow::createTreeItem(QTreeWidgetItem *parent, QString name) {
    QTreeWidgetItem *childItem = new QTreeWidgetItem();
    childItem->setText(0, name);
    QString iconPath;
    if (name.contains("socket"))
        iconPath = "../CameraManager/img/coordinates.png";
    else if (name.contains("option") && name.endsWith(".txt"))
        iconPath = "../CameraManager/img/config.png";
    else if (name.contains("calibration_summary", Qt::CaseInsensitive)){
        iconPath = "../CameraManager/img/camera.png";
        QTreeWidgetItem *item = parent;
        calibrationPath = name;
        while (item != NULL){
            calibrationPath = item->text(0) + "/" + calibrationPath;
            item = item->parent();
        }
        calibrationPath = projectsPath + "/" + calibrationPath;
    } else if (name.endsWith(".exe"))
        iconPath = "../CameraManager/img/exe.png";
    else if (parent->text(0) == "output" && name.endsWith(".dat"))
        iconPath = "../CameraManager/img/2D.png";
    else if (name.contains("grupper"))
        iconPath = "../CameraManager/img/img.png";
    else
        iconPath = "../CameraManager/img/file.png";
    childItem->setIcon(0, QIcon(iconPath));
    parent->addChild(childItem);
}

void MainWindow::on_LoadDefaults_Pushed() {
    cameraManagers[selectedCameraManager]->loadPropertiesDefaults();
}

void MainWindow::loadDefaultTrackPointSettings() {
    loadTrackPointSettingsFromFile(QString(QDir::currentPath() + "/../props/defaultTrackPointSettings.ini"));
}

void MainWindow::on_TrackPointChecked(int state) {
    printf("TrackPoint Preview: %s\n", ((state == Qt::Unchecked) ? "DISABLED" : "ENABLED"));
    trackPointProperty.trackPointPreview = (state == Qt::Checked);
    /*
    ui->thresholdValueEdit->setEnabled((state != Qt::Checked));
    ui->subwinValueEdit->setEnabled((state != Qt::Checked));
    ui->minPointValueEdit->setEnabled((state != Qt::Checked));
    ui->maxPointValueEdit->setEnabled((state != Qt::Checked));
    */
}

void MainWindow::on_TrackPointValueChanged() {
    QLineEdit* lineEdit = (QLineEdit*) sender();
    if (lineEdit == ui->thresholdValueEdit) {
        trackPointProperty.thresholdValue = lineEdit->text().toInt();
    }
    if (lineEdit == ui->subwinValueEdit) {
        trackPointProperty.subwinValue = lineEdit->text().toInt();
    }
    if (lineEdit == ui->minPointValueEdit) {
        trackPointProperty.minPointValue = lineEdit->text().toInt();
    }
    if (lineEdit == ui->maxPointValueEdit) {
        trackPointProperty.maxPointValue = lineEdit->text().toInt();
    }
}

void MainWindow::on_TrackPointSliderValueChanged(int value) {
    QSlider* slider = (QSlider*) sender();
    if (slider == ui->thresholdSlider) {
        ui->thresholdValueEdit->setText(QString::number(value));
        trackPointProperty.thresholdValue = ui->thresholdValueEdit->text().toInt();
    }
    if (slider == ui->subwinSlider) {
        ui->subwinValueEdit->setText(QString::number(value));
        trackPointProperty.subwinValue = ui->subwinValueEdit->text().toInt();
    }
    if (slider == ui->minPointSlider) {
        ui->minPointValueEdit->setText(QString::number(value));
        trackPointProperty.minPointValue = ui->minPointValueEdit->text().toInt();
    }
    if (slider == ui->maxPointSlider) {
        ui->maxPointValueEdit->setText(QString::number(value));
        trackPointProperty.maxPointValue = ui->maxPointValueEdit->text().toInt();
    }
}

void MainWindow::loadTrackPointSettingsFromFile(QString& filepath) {
    QSettings settings(filepath, QSettings::IniFormat);
    TrackPointProperty prop;
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
    trackPointProperty = prop;
}

void MainWindow::saveTrackPointSettingsToFile(QString& filepath, TrackPointProperty& props) {
    QSettings settings(filepath, QSettings::IniFormat);
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
}

void MainWindow::setupTrackPointTab() {
    QGridLayout* trackPointLayout = new QGridLayout();
    QLabel* trackpointLabel = new QLabel("TrackPoint Preview:");
    trackPointLayout->addWidget(trackpointLabel, 0, 0);
    ui->trackPointEnabled = new QCheckBox();
    ui->trackPointEnabled->setChecked(trackPointProperty.trackPointPreview);
    connect(ui->trackPointEnabled, SIGNAL(stateChanged(int)), this, SLOT(on_TrackPointChecked(int)));
    trackPointLayout->addWidget(ui->trackPointEnabled, 0, 1);

    QLabel* thresholdLabel = new QLabel(trackPointProperty.thresholdText);
    ui->thresholdValueEdit = new QLineEdit(QString::number(trackPointProperty.thresholdValue));
    ui->thresholdValueEdit->setValidator(new QIntValidator(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax));
    ui->thresholdSlider = new QSlider(Qt::Horizontal);
    ui->thresholdSlider->setValue(trackPointProperty.thresholdValue);
    ui->thresholdSlider->setRange(trackPointProperty.thresholdMin, trackPointProperty.thresholdMax);
    trackPointLayout->addWidget(thresholdLabel, 1, 0);
    trackPointLayout->addWidget(ui->thresholdValueEdit, 1, 1);
    trackPointLayout->addWidget(ui->thresholdSlider, 1, 2);
    connect(ui->thresholdValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));

    QLabel* subwinLabel = new QLabel(trackPointProperty.subwinText);
    ui->subwinValueEdit = new QLineEdit(QString::number(trackPointProperty.subwinValue));
    ui->subwinValueEdit->setValidator(new QIntValidator(trackPointProperty.subwinMin, trackPointProperty.subwinMax));
    ui->subwinSlider = new QSlider(Qt::Horizontal);
    ui->subwinSlider->setValue(trackPointProperty.subwinValue);
    ui->subwinSlider->setRange(trackPointProperty.subwinMin, trackPointProperty.subwinMax);
    trackPointLayout->addWidget(subwinLabel, 2, 0);
    trackPointLayout->addWidget(ui->subwinValueEdit, 2, 1);
    trackPointLayout->addWidget(ui->subwinSlider, 2, 2);
    connect(ui->subwinValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->subwinSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));

    QLabel* minPointLabel = new QLabel(trackPointProperty.minPointText);
    ui->minPointValueEdit = new QLineEdit(QString::number(trackPointProperty.minPointValue));
    ui->minPointValueEdit->setValidator(new QIntValidator(trackPointProperty.minPointMin, trackPointProperty.minPointMax));
    ui->minPointSlider = new QSlider(Qt::Horizontal);
    ui->minPointSlider->setValue(trackPointProperty.minPointValue);
    ui->minPointSlider->setRange(trackPointProperty.minPointMin, trackPointProperty.minPointMax);
    trackPointLayout->addWidget(minPointLabel, 3, 0);
    trackPointLayout->addWidget(ui->minPointValueEdit, 3, 1);
    trackPointLayout->addWidget(ui->minPointSlider, 3, 2);
    connect(ui->minPointValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->minPointSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));

    QLabel* maxPointLabel = new QLabel(trackPointProperty.maxPointText);
    ui->maxPointValueEdit = new QLineEdit(QString::number(trackPointProperty.maxPointValue));
    ui->maxPointValueEdit->setValidator(new QIntValidator(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax));
    ui->maxPointSlider = new QSlider(Qt::Horizontal);
    ui->maxPointSlider->setValue(trackPointProperty.maxPointValue);
    ui->maxPointSlider->setRange(trackPointProperty.maxPointMin, trackPointProperty.maxPointMax);
    trackPointLayout->addWidget(maxPointLabel, 4, 0);
    trackPointLayout->addWidget(ui->maxPointValueEdit, 4, 1);
    trackPointLayout->addWidget(ui->maxPointSlider, 4, 2);
    connect(ui->maxPointValueEdit, SIGNAL(returnPressed()), this, SLOT(on_TrackPointValueChanged()));
    connect(ui->maxPointSlider, SIGNAL(valueChanged(int)), this, SLOT(on_TrackPointSliderValueChanged(int)));

    trackPointLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    ui->trackPointWidget->setLayout(trackPointLayout);
}