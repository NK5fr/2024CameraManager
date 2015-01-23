#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QtWidgets/QMainWindow>
#include "ui_cameramanager.h"

class CameraManager : public QMainWindow
{
	Q_OBJECT

public:
	CameraManager(QWidget *parent = 0);
	~CameraManager();

private:
	Ui::CameraManagerClass ui;
};

#endif // CAMERAMANAGER_H
