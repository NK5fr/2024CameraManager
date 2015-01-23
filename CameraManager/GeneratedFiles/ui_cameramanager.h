/********************************************************************************
** Form generated from reading UI file 'cameramanager.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERAMANAGER_H
#define UI_CAMERAMANAGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CameraManagerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CameraManagerClass)
    {
        if (CameraManagerClass->objectName().isEmpty())
            CameraManagerClass->setObjectName(QStringLiteral("CameraManagerClass"));
        CameraManagerClass->resize(600, 400);
        menuBar = new QMenuBar(CameraManagerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        CameraManagerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CameraManagerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        CameraManagerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(CameraManagerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        CameraManagerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(CameraManagerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        CameraManagerClass->setStatusBar(statusBar);

        retranslateUi(CameraManagerClass);

        QMetaObject::connectSlotsByName(CameraManagerClass);
    } // setupUi

    void retranslateUi(QMainWindow *CameraManagerClass)
    {
        CameraManagerClass->setWindowTitle(QApplication::translate("CameraManagerClass", "CameraManager", 0));
    } // retranslateUi

};

namespace Ui {
    class CameraManagerClass: public Ui_CameraManagerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERAMANAGER_H
