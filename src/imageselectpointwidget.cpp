
#include "imageselectpointwidget.h"

ImageSelectPointWidget::ImageSelectPointWidget() {
    pointNr = 0;
    pointMax = 0;
    backButton.setText("Back");
    backButton.setMinimumSize(10, 40);
    backButton.setEnabled(false);
    nextButton.setText("Next");
    nextButton.setMinimumSize(10, 40);
    //subImageResolutionX.setText(QString::number(imageWidget.getNumImagesWidth()));
    //subImageResolutionX.setMaximumWidth(100);
    //subImageResolutionY.setText(QString::number(imageWidget.getNumImagesHeight()));
    //subImageResolutionY.setMaximumWidth(100);
    acceptSubImageResolution.setText("Accept");
    //imageWidget.hint
    imageWidget.setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    //imageWidget.setAlignment(Qt::AlignCenter);
    
    QHBoxLayout* widget = new QHBoxLayout();
    
    //widget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    widget->addWidget(&imageWidget);
    widget->setAlignment(Qt::AlignCenter);
    layout.addLayout(widget);
    /*
    display.setReadOnly(true);
    display.setAlignment(Qt::AlignCenter);
    //display.setFixedWidth(50);
    updateDisplay();
    layout.addWidget(&display);
    QHBoxLayout* buttonPanel = new QHBoxLayout();
    buttonPanel->addWidget(&backButton);
    buttonPanel->addWidget(&nextButton);
    layout.addLayout(buttonPanel);
    layout.addSpacing(0);
    QGridLayout* gridLayout = new QGridLayout();
    QLabel* subImageWidthLabel = new QLabel("No. images in width:");
    gridLayout->addWidget(subImageWidthLabel, 0, 0);
    QLabel* subImageHeightLabel = new QLabel("No. images in height:");
    gridLayout->addWidget(subImageHeightLabel, 1, 0);
    gridLayout->addWidget(&subImageResolutionX, 0, 1);
    gridLayout->addWidget(&subImageResolutionY, 1, 1);
    //layout.addLayout(gridLayout);
    layout.addWidget(&acceptSubImageResolution);
    */
    setLayout(&layout);

    connect(&nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
    connect(&backButton, SIGNAL(clicked()), this, SLOT(backClicked()));
    connect(&acceptSubImageResolution, SIGNAL(clicked()), this, SLOT(acceptClicked()));
}

void ImageSelectPointWidget::nextClicked() {
    pointNr++;
    if (pointNr > pointMax) pointMax++;
    backButton.setEnabled(true);
    //imageWidget.nextPointSeries();
    updateDisplay();
}

void ImageSelectPointWidget::backClicked() {
    pointNr--;
    if (pointNr == 0) backButton.setEnabled(false);
    //imageWidget.backPointSeries();
    updateDisplay();
}

void ImageSelectPointWidget::acceptClicked() {
    QString text = getPointsInFormattedString();
    QTextEdit* edit = new QTextEdit();
    edit->setPlainText(text);
    edit->show();
}

void ImageSelectPointWidget::updateDisplay() {
    display.setText(QString::number(pointNr + 1) + " / " + QString::number(pointMax + 1));
}

QString ImageSelectPointWidget::getPointsInFormattedString() {
    /*
    std::vector<std::vector<TrackPoint::PointInCamera*>> points = imageWidget.getPoints();
    QString res;
    int pMax = 0;
    int pMaxGlobal = 0;
    bool failed = false;
    if (points.size() == 0) return QString();
    for (int pointGroupNr = 0; pointGroupNr < points.size() && !failed; pointGroupNr++) {
        for (int pNr = 0; pNr < points[pointGroupNr].size() && !failed; pNr++) {
            if (points[pointGroupNr][pNr] != nullptr) {
                if (!points[pointGroupNr][pNr]->isUsed) break;
                if (pointGroupNr > 0 && pNr > pMax) failed = true;
                pMax++;
            }
        }
        if (pointGroupNr == 0) pMaxGlobal = pMax;
        if (pMax < pMaxGlobal) failed = true;
    }

    for (int pNr = 0; pNr < points[0].size() && !failed; pNr++) {
        for (int pointGroupNr = 0; pointGroupNr < points.size() && !failed; pointGroupNr++) {
            if (points[pointGroupNr][pNr] == nullptr) continue;
            if (!points[pointGroupNr][pNr]->isUsed) continue;
            res.append(QString::number(points[pointGroupNr][pNr]->pointX) + " " + QString::number(points[pointGroupNr][pNr]->pointY) + "\n");
        }
    }*/
    //return res;
    return QString();
}
