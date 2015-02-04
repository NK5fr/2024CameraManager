/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow {
public:

    QAction *actionMosaic;
    QAction *actionUpdateImages;
    QAction *actionLiveView;
    QAction *actionCrosshair;
    QAction *actionQuitter;
    QAction *actionCrosshairReal;
    QAction *actionHighQuality;
    QMdiArea *centralwidget;
    QToolBar *toolBar;
    QStatusBar *statusbar;
    QDockWidget *CamerasWidget_2;
    QWidget *dockWidgetContents_4;
    QTabWidget *tabWidget;
    QWidget *projects;
    QLabel *label_2;
    QTreeWidget *ProjectTree;
    QWidget *camera;
    QComboBox *SelectCameras;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *propertiesContainer;
    QLabel *propertiesIcon;
    QLabel *label;
    QTreeView *CameraTree;

    void setupUi(QMainWindow *MainWindow) {
        if (MainWindow->objectName().isEmpty()) MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1185, 664);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        MainWindow->setDockOptions(QMainWindow::AnimatedDocks);
        actionMosaic = new QAction(MainWindow);
        actionMosaic->setObjectName(QStringLiteral("actionMosaic"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/mosaic"), QSize(), QIcon::Normal, QIcon::Off);
        actionMosaic->setIcon(icon);
        actionUpdateImages = new QAction(MainWindow);
        actionUpdateImages->setObjectName(QStringLiteral("actionUpdateImages"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/one"), QSize(), QIcon::Normal, QIcon::Off);
        actionUpdateImages->setIcon(icon1);
        actionLiveView = new QAction(MainWindow);
        actionLiveView->setObjectName(QStringLiteral("actionLiveView"));
        actionLiveView->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/liveview"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QStringLiteral(":/icons/liveview_on"), QSize(), QIcon::Normal, QIcon::On);
        actionLiveView->setIcon(icon2);
        actionCrosshair = new QAction(MainWindow);
        actionCrosshair->setObjectName(QStringLiteral("actionCrosshair"));
        actionCrosshair->setCheckable(true);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/crosshair"), QSize(), QIcon::Normal, QIcon::Off);
        actionCrosshair->setIcon(icon3);
        actionQuitter = new QAction(MainWindow);
        actionQuitter->setObjectName(QStringLiteral("actionQuitter"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/quit"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuitter->setIcon(icon4);
        actionCrosshairReal = new QAction(MainWindow);
        actionCrosshairReal->setObjectName(QStringLiteral("actionCrosshairReal"));
        actionCrosshairReal->setCheckable(true);
        actionCrosshairReal->setEnabled(false);
        actionHighQuality = new QAction(MainWindow);
        actionHighQuality->setObjectName(QStringLiteral("actionHighQuality"));
        actionHighQuality->setCheckable(true);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/quality_low"), QSize(), QIcon::Normal, QIcon::Off);
        icon5.addFile(QStringLiteral(":/icons/quality_high"), QSize(), QIcon::Normal, QIcon::On);
        actionHighQuality->setIcon(icon5);
        actionHighQuality->setVisible(true);
        actionHighQuality->setIconVisibleInMenu(true);
        centralwidget = new QMdiArea(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setContextMenuPolicy(Qt::NoContextMenu);
#ifndef QT_NO_TOOLTIP
        toolBar->setToolTip(QStringLiteral(""));
#endif // QT_NO_TOOLTIP
        toolBar->setLayoutDirection(Qt::LeftToRight);
        toolBar->setMovable(true);
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        toolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);
        CamerasWidget_2 = new QDockWidget(MainWindow);
        CamerasWidget_2->setObjectName(QStringLiteral("CamerasWidget_2"));
        CamerasWidget_2->setMinimumSize(QSize(374, 512));
        CamerasWidget_2->setMaximumSize(QSize(374, 602));
        CamerasWidget_2->setContextMenuPolicy(Qt::DefaultContextMenu);
        CamerasWidget_2->setAcceptDrops(false);
        CamerasWidget_2->setLayoutDirection(Qt::LeftToRight);
        CamerasWidget_2->setAutoFillBackground(false);
        CamerasWidget_2->setInputMethodHints(Qt::ImhNone);
        CamerasWidget_2->setFloating(false);
        CamerasWidget_2->setAllowedAreas(Qt::AllDockWidgetAreas);
        dockWidgetContents_4 = new QWidget();
        dockWidgetContents_4->setObjectName(QStringLiteral("dockWidgetContents_4"));
        tabWidget = new QTabWidget(dockWidgetContents_4);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 20, 371, 531));
        tabWidget->setMinimumSize(QSize(371, 501));
        projects = new QWidget();
        projects->setObjectName(QStringLiteral("projects"));
        label_2 = new QLabel(projects);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 20, 161, 21));
        ProjectTree = new QTreeWidget(projects);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        ProjectTree->setHeaderItem(__qtreewidgetitem);
        ProjectTree->setObjectName(QStringLiteral("ProjectTree"));
        ProjectTree->setGeometry(QRect(5, 51, 361, 441));
        ProjectTree->setContextMenuPolicy(Qt::CustomContextMenu);
        ProjectTree->setHeaderHidden(true);
        tabWidget->addTab(projects, QString());
        camera = new QWidget();
        camera->setObjectName(QStringLiteral("camera"));
        SelectCameras = new QComboBox(camera);
        SelectCameras->setObjectName(QStringLiteral("SelectCameras"));
        SelectCameras->setGeometry(QRect(10, 10, 311, 27));
        SelectCameras->setMinimumSize(QSize(311, 27));
        verticalLayoutWidget = new QWidget(camera);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 230, 351, 261));
        propertiesContainer = new QVBoxLayout(verticalLayoutWidget);
        propertiesContainer->setSpacing(2);
        propertiesContainer->setObjectName(QStringLiteral("propertiesContainer"));
        propertiesContainer->setContentsMargins(0, 0, 0, 0);
        propertiesIcon = new QLabel(camera);
        propertiesIcon->setObjectName(QStringLiteral("propertiesIcon"));
        propertiesIcon->setGeometry(QRect(20, 210, 31, 21));
        label = new QLabel(camera);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(90, 210, 241, 17));
        CameraTree = new QTreeView(camera);
        CameraTree->setObjectName(QStringLiteral("CameraTree"));
        CameraTree->setGeometry(QRect(10, 40, 351, 141));
        CameraTree->setMinimumSize(QSize(351, 141));
        CameraTree->setContextMenuPolicy(Qt::CustomContextMenu);
        CameraTree->header()->setVisible(false);
        tabWidget->addTab(camera, QString());
        CamerasWidget_2->setWidget(dockWidgetContents_4);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), CamerasWidget_2);

        toolBar->addAction(actionLiveView);
        toolBar->addAction(actionUpdateImages);
        toolBar->addSeparator();
        toolBar->addAction(actionCrosshair);
        toolBar->addAction(actionCrosshairReal);
        toolBar->addSeparator();
        toolBar->addAction(actionMosaic);
        toolBar->addAction(actionHighQuality);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow) {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionMosaic->setText(QApplication::translate("MainWindow", "Mosaic", 0));
#ifndef QT_NO_TOOLTIP
        actionMosaic->setToolTip(QApplication::translate("MainWindow", "Make a mosaic with camera views", 0));
#endif // QT_NO_TOOLTIP
        actionUpdateImages->setText(QApplication::translate("MainWindow", "UpdateImages (Space)", 0));
        actionUpdateImages->setShortcut(QApplication::translate("MainWindow", "Space", 0));
        actionLiveView->setText(QApplication::translate("MainWindow", "LiveView", 0));
        actionCrosshair->setText(QApplication::translate("MainWindow", "crosshair", 0));
#ifndef QT_NO_TOOLTIP
        actionCrosshair->setToolTip(QApplication::translate("MainWindow", "activate crosshair", 0));
#endif // QT_NO_TOOLTIP
        actionQuitter->setText(QApplication::translate("MainWindow", "quitter", 0));
        actionCrosshairReal->setText(QApplication::translate("MainWindow", "Real", 0));
#ifndef QT_NO_TOOLTIP
        actionCrosshairReal->setToolTip(QApplication::translate("MainWindow", "real coordonates instead of precise ones", 0));
#endif // QT_NO_TOOLTIP
        actionHighQuality->setText(QApplication::translate("MainWindow", "highQuality", 0));
#ifndef QT_NO_TOOLTIP
        actionHighQuality->setToolTip(QApplication::translate("MainWindow", "force high quality resizing, CPU intensive", 0));
#endif // QT_NO_TOOLTIP
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
#ifndef QT_NO_TOOLTIP
        CamerasWidget_2->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "Projects and files List", 0));
        tabWidget->setTabText(tabWidget->indexOf(projects), QApplication::translate("MainWindow", "Projects", 0));
        propertiesIcon->setText(QString());
        label->setText(QApplication::translate("MainWindow", "No Selection", 0));
        tabWidget->setTabText(tabWidget->indexOf(camera), QApplication::translate("MainWindow", "Cameras", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
