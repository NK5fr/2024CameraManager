/**
 * \file mainwindow.h
 * \author Virgile Wozny
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QFrame>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>  // gs
#include <QTimer> // gs
#include <QProcess> // gs
#include <qsettings.h> // Lars Aksel
#include "trackpointproperty.h" // Lars Aksel
#include "externalprocess.h"

#include "menubar.h"
#include "abstractcameramanager.h"
#include "systemmanager.h"



namespace Ui {
    class MainWindow;

    enum PropertiesWidgetPosition {
        PropertyName = 0, PropertyAuto = 1, PropertyWriteValue = 2, PropertyReadValue = 3, PropertySlider = 4
    };

    extern bool crosshair, crosshairReal, forceHighQuality;
}

namespace FileInfo {
    enum FileType {
        ConfigurationFile, SocketFile, Folder, ImageFile, CalibrationFile, Executable, TextFile
    };
}

class AbstractCameraManager;

/**
 * MainWindow
 * \brief handle ui events.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /** CONSTRUCTOR - DESTRUCTOR **/
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //Armand & Nathan on 13/05/2024 : Useless because we only use Spinnaker

    //Constant to get if the selected manager is Fly or SPIN
    //wrote on 11/06/2019 by French students
    // static int selection;



    /** ACCESSORS **/
    void modifySubWindow(QMdiSubWindow* in, bool add);
    Ui::MainWindow *getUi();
    std::vector<AbstractCameraManager*> getCameraManagers();
    int getSelectedCameraManager();

    /** THREAD METHODS **/
    void startCameraDetection(SystemManager *sm);
    void startUpdateProperties();

    // gs test
    //void execute(QString command, QProcess &p);

    bool isColorModeActivate(){ return colorModeActivate; }
    bool isCrosshairActivate();
    bool isRealCoordinateActivate();
private slots:
    /* All of them could probably be private */
    /** TOOL BAR **/
    void on_actionLiveView_toggled(bool arg1);
    void on_actionUpdateImages_triggered();
    void on_actionCrosshair_toggled(bool arg1);
    void on_actionColor_toggled(bool arg1);
    void on_actionCrosshairReal_toggled(bool arg1);
    void on_actionMosaic_triggered();
    void on_actionRemoveWindows_triggered();
    void on_actionHighQuality_toggled(bool arg1);

    /** MENU BAR **/
    void menuBarClicked(QAction*);

    /** CAMERA TREE **/
    void on_CameraTree_customContextMenuRequested(const QPoint &);
    void menuCameraAction_triggered(QAction *);
    void cameraTree_itemClicked(const QModelIndex);
    void cameraTree_itemDoubleClicked(const QModelIndex);
    void on_SelectCameras_currentIndexChanged(int index);

    /** PROJECT TREE **/
    void projectTree_customContextMenuRequested(const QPoint &);
    void projectTree_doubleClicked(const QModelIndex &index);
    void projectTree_clicked(const QModelIndex &index);
    void menuProjectAction_triggered(QAction *);

    // Lars Aksel - 20.04.2015
    void quickLoadTrackPointSettings();
    void quickSaveTrackPointSettings();
    void loadDefaultTrackPointSettings_clicked();
    void loadDefaultCameraProperties_clicked();
    void on_TrackPointChecked(Qt::CheckState state);
    void on_FilteredImageChecked(Qt::CheckState state);
    void on_ShowCoordinateLabelChecked(Qt::CheckState state);
    void on_RemoveDuplicatesChecked(Qt::CheckState state);
    void on_ShowMinSepCircleChecked(Qt::CheckState state);
    void on_TrackPointValueChanged();
    void on_TrackPointSliderValueChanged(int);

    // Lars Aksel - Loading/Saving Camera Properties
    void quickLoadCameraSettings();
    void quickSaveCameraSettings();

    /* grethe - output fra trackpoint */
    /*
    void executeFinished(QProcess &p);
    void executeError(QProcess::ProcessError);
    void appendOutput();
    void readStdOutput(QProcess &p);
    void printOutput();
    */

signals:
    void activateCrosshair(bool);
    void activateRealCoordinate(bool);
protected:
    /** CAMERA TREE **/
    /* Could probably be private */
    void on_addGroup();
    void on_deleteGroup();
    void on_editItem();
    void on_resetItem();

private:
    bool colorModeActivate = false;

    // gs TEST for ? real time output fra trackpoint
    ExternalProcess* trackPointProcess; // Lars Aksel
    //QTextEdit* trackPointOutput; //gs
    //QStringList options;  // gs
    /*
    QProcess process;
    QTimer process_timer;
    QString process_file;
    qint64 process_file_pos;
    */
    TrackPointProperty trackPointProperty; // Lars Aksel

    /** CREATING FOLDER AND FILE ITEMS **/
    //void createTreeItem(QTreeWidgetItem *parent, QString name, QString filepath);
    //QTreeWidgetItem* createTreeFolder(QTreeWidgetItem *parent, const QString& path, const QString& name);
    //QTreeWidgetItem* updateTreeFolder(QTreeWidgetItem *parent, const QString& path, const QString& name);

    MenuBar *bar;
    Ui::MainWindow *ui;
    std::vector<AbstractCameraManager*> cameraManagers;
    int selectedCameraManager;
    QPixmap propertiesIcons[3];
    QIcon icons[8]; // Lars Aksel - Handle for shared icons

    // Lars Aksel - Loading TrackPoint-Settings into TrackPoint-tab
    void loadDefaultTrackPointSettings();
    void loadTrackPointSettingsFromFile(QString& filepath);
    void saveTrackPointSettingsToFile(QString& filepath, TrackPointProperty& props);
    void setupTrackPointTab();

    // Armand & Nathan - Checks if files exist and notify user if not
    void checkFiles();

    QString projectsPath;
    QString calibrationPath;
    bool isProjectLoaded = false;

    //SystemManager *system;

    QLabel *label;
    std::vector<QString> ConfigFilesPaths;
    /* Internal class to detect cameras */
    //classe a remettre en commentaire si bug
    class ThreadDetectCamera : public QThread {
    public:
        ThreadDetectCamera(MainWindow *w) : QThread() { window = w; }
    protected:
        void run() {
            window->startCameraDetection(system);

        }
    private:
        MainWindow *window;
        SystemManager *system = new SystemManager;

    };
    ThreadDetectCamera tdc;

    QTimer *timer = new QTimer();

    //classe a remettre en commentaire si bug
    bool detectCameras;

    //Internal class to update properties from cameras
    class ThreadUpdateProperties : public QThread {
    public:
        ThreadUpdateProperties(MainWindow *w) : QThread() { window = w; }
    protected:
        void run() {
            window->startUpdateProperties();
        }
    private:
        MainWindow *window;
    };
    ThreadUpdateProperties tup;
    QString getLastProject();
    void saveLastProject(QString projectPath);
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOW_H
