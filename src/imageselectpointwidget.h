

#ifndef IMAGESELECTPOINTWIDGET_H
#define IMAGESELECTPOINTWIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qtextedit.h>

#include "imageopenglwidget.h"
#include "imagewidget.h"

class ImageSelectPointWidget : public QWidget {
    Q_OBJECT
public:
    ImageSelectPointWidget();

    inline ImageOpenGLWidget* getImageWidget() { return &imageWidget; }

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
    ImageOpenGLWidget imageWidget;
    //ImageWidget imageWidget;
    int pointNr;
    int pointMax;

    void updateDisplay();
    QString getPointsInFormattedString();
};

#endif
