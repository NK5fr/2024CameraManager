

#ifndef IMAGESELECTPOINTWIDGET_H
#define IMAGESELECTPOINTWIDGET_H

#include <qwidget.h>
#include <qgridlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qformlayout.h>
#include <qtextedit.h>

#include "imagewidget.h"

class ImageSelectPointWidget : public QWidget {
    Q_OBJECT
public:
    ImageSelectPointWidget();

    inline ImageWidget* getImageWidget() { return &imageWidget; }

private slots:
    void nextClicked();
    void backClicked();
    void acceptClicked();

private:
    QVBoxLayout layout;
    QLineEdit display;
    QLineEdit subImageResolutionX;
    QLineEdit subImageResolutionY;
    QPushButton nextButton;
    QPushButton backButton;
    QPushButton acceptSubImageResolution;
    ImageWidget imageWidget;
    int pointNr;
    int pointMax;

    void updateDisplay();
    QString getPointsInFormattedString();
};

#endif