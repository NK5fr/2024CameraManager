/**
 * \file mainwindow.h
 * \author Virgile Wozny
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>  // gs
#include <QTimer> // gs
#include <QProcess> // gs

#include "menubar.h"
#include "abstractcameramanager.h"

namespace Ui {
    class MainWindow;

    enum PropertiesWidgetPosition {
        PropertyName = 0, PropertyAuto = 1, PropertyValue = 2, PropertySlider = 3
    };

    extern bool crosshair, crosshairReal, forceHighQuality;
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

    /** ACCESSORS **/
    void modifySubWindow(QMdiSubWindow* in, bool add);
    Ui::MainWindow *getUi();
    std::vector<AbstractCameraManager*> getCameraManagers();
    int getSelectedCameraManager();

    /** THREAD METHODS **/
    void startCameraDetection();
    void startUpdateProperties();

    // gs test
    void execute(QString command, QProcess &p);

    public slots:
    /* All of them could probably be private */
    /** TOOL BAR **/
    void on_actionLiveView_toggled(bool arg1);
    void on_actionUpdateImages_triggered();
    void on_actionCrosshair_toggled(bool arg1);
    void on_actionCrosshairReal_toggled(bool arg1);
    void on_actionMosaic_triggered();
    void on_actionHighQuality_toggled(bool arg1);

    /** MENU BAR **/
    void menuBarClicked(QAction*);

    /** CAMERA TREE **/
    void on_CameraTree_customContextMenuRequested(const QPoint &);
    void menuCameraAction_triggered(QAction *);
    void on_CameraTree_itemClicked(const QModelIndex);
    void on_SelectCameras_currentIndexChanged(int index);

    /** PROJECT TREE **/
    void on_ProjectTree_customContextMenuRequested(const QPoint &);
    void menuProjectAction_triggered(QAction *);
    void on_ProjectTree_doubleClicked(const QModelIndex &index);

    /** TO BE DELETED **/
    /*void on_Detect_clicked();*/
    /*void on_updatePropertiesButton_clicked();*/

    /* grethe - output fra trackpoint */
    void executeFinished(QProcess &p);
    void executeError(QProcess::ProcessError);
    void appendOutput();
    void readStdOutput(QProcess &p);
    void printOutput();

signals:
    void activateCrosshair(bool);

protected:
    /** CAMERA TREE **/
    /* Could probably be private */
    void on_addGroup();
    void on_deleteGroup();
    void on_editItem();
    void on_resetItem();

private:
    // gs TEST for å real time output fra trackpoint
    QTextEdit* text_edit; //gs
    QStringList options;  // gs

    QProcess process;
    QTimer process_timer;
    QString process_file;
    qint64 process_file_pos;

    /** CREATING FOLDER AND FILE ITEMS **/
    void createTreeItem(QTreeWidgetItem *parent, QString name);
    void createTreeFolder(QTreeWidgetItem *parent, QString path, QString name);

    MenuBar *bar;
    Ui::MainWindow *ui;
    std::vector<AbstractCameraManager*> cameraManagers;
    int selectedCameraManager;
    QPixmap propertiesIcons[3];


    QString projectsPath;
    QString calibrationPath;

    QLabel *label;
    std::vector<QString> ConfigFilesPaths;
    /* Internal class to detect cameras */
    class ThreadDetectCamera : public QThread {
    public:
        ThreadDetectCamera(MainWindow *w) : QThread() { window = w; }
    protected:
        void run() {
            window->startCameraDetection();
        }
    private:
        MainWindow *window;
    };
    ThreadDetectCamera tdc;
    bool detectCameras;

    /* Internal class to update properties from cameras */
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
};

#endif // MAINWINDOW_H


