
#include "calibrationfilewidget.h"

CalibrationFileWidget::CalibrationFileWidget(CalibrationFile* calibFile) : QWidget() {
    this->calibFile = calibFile;
    initUI();
}

void CalibrationFileWidget::initUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout();
    QVBoxLayout* leftLayout = new QVBoxLayout();
    QGridLayout* gridLayout = new QGridLayout();

    combinationPreviewWidget = new CalibrationFileOpenGLWidget(calibFile);
    leftLayout->addWidget(combinationPreviewWidget);

    filterList = new QTreeWidget();
    filterList->setHeaderLabels(QStringList() << "" << "Warning value" << "Max value");
    QTreeWidgetItem* s0Filter = new QTreeWidgetItem();
    s0Filter->setText(0, "S0");
    s0Filter->setText(1, "0.13");
    s0Filter->setText(2, "10");
    filterList->addTopLevelItem(s0Filter);
    QTreeWidgetItem* cStdDevFilter = new QTreeWidgetItem();
    cStdDevFilter->setText(0, "C std. dev.");
    cStdDevFilter->setText(1, "2");
    cStdDevFilter->setText(2, "5");
    filterList->addTopLevelItem(cStdDevFilter);
    filterList->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    leftLayout->addWidget(filterList);

    cameraTable = new QTreeWidget(this);
    cameraTable->setHeaderLabels(QStringList() << "" << "Cam 0" << "Cam 1" << "Cam 2");
    //cameraTable->setContentsMargins(0, 0, 20, 0);
    leftLayout->addWidget(cameraTable);


    //QStandardItemModel* combinationListModel = new QStandardItemModel(2, 3, this);
    combinationList = new QTreeWidget(this);
    combinationList->setHeaderLabels(QStringList() << "Combination" << "S0" << "#. of frames");
    combinationList->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Expanding);

    //combinationListModel->setHorizontalHeaderLabels(QStringList() << "Combination" << "S0" << "#. of frames");
    vector<TrackPoint::CameraCombination*> camComb = calibFile->getCameraCombinations();

    for (int i = 0; i < camComb.size(); i++) {
        QString combination = QString::number(camComb[i]->camNumbers[0]) + "_" + QString::number(camComb[i]->camNumbers[1]) + "_" + QString::number(camComb[i]->camNumbers[2]);
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, combination);
        item->setText(1, QString::number(camComb[i]->s0));
        item->setText(2, QString::number(camComb[i]->numFrames));
        item->setData(0, Qt::UserRole, QVariant::fromValue((quintptr) camComb[i]));
        if (camComb[i]->numFrames < 55) {
            item->setBackgroundColor(0, QColor(255, 0, 0, 150));
            item->setBackgroundColor(1, QColor(255, 0, 0, 150));
            item->setBackgroundColor(2, QColor(255, 0, 0, 150));
        } else if (camComb[i]->s0 > 0.13) {
            item->setBackgroundColor(0, QColor(255, 255, 0, 150));
            item->setBackgroundColor(1, QColor(255, 255, 0, 150));
            item->setBackgroundColor(2, QColor(255, 255, 0, 150));
        }
        //if (camComb[i]->numFrames < 55) item->setDisabled(true);
        combinationList->addTopLevelItem(item);
    }
    combinationList->resizeColumnToContents(0);
    combinationList->resizeColumnToContents(1);
    combinationList->resizeColumnToContents(2);

    leftLayout->setStretchFactor(combinationPreviewWidget, 2);
    leftLayout->setStretchFactor(filterList, 1);
    leftLayout->setStretchFactor(cameraTable, 1);

    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(combinationList);
    setLayout(mainLayout);

    combinationPreviewWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);


    connect(combinationList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(combinationClicked(QTreeWidgetItem*, int)));
    connect(combinationList, SIGNAL(itemSelectionChanged()), this, SLOT(combinationSelectionChanged()));
}

void CalibrationFileWidget::combinationClicked(QTreeWidgetItem* item, int column) {
    if (item->isDisabled()) {
        cameraTable->clear();
        combinationPreviewWidget->setSelectedCameraCombination(nullptr);
        combinationPreviewWidget->update();
        return;
    }
    TrackPoint::CameraCombination* camComb = (TrackPoint::CameraCombination*) item->data(0, Qt::UserRole).value<quintptr>();
    if (camComb != nullptr) {
        updateCameraTable(camComb);
    }
    combinationPreviewWidget->setSelectedCameraCombination(camComb);
    combinationPreviewWidget->update();
}

void CalibrationFileWidget::combinationSelectionChanged() {
    if (combinationList->selectedItems().size() == 0) return;
    QTreeWidgetItem* item = combinationList->selectedItems()[0];
    combinationClicked(item, 0);
}

void CalibrationFileWidget::updateCameraTable(TrackPoint::CameraCombination* camComb) {
    cameraTable->clear();

    for (int i = 0; i < 3; i++) {
        if (camComb->cameras[i] == nullptr) return;
    }

    QTreeWidgetItem* cameraNoItem = new QTreeWidgetItem();
    cameraNoItem->setText(0, "Camera No");
    cameraNoItem->setText(1, QString::number(camComb->cameras[0]->camNo));
    cameraNoItem->setText(2, QString::number(camComb->cameras[1]->camNo));
    cameraNoItem->setText(3, QString::number(camComb->cameras[2]->camNo));
    cameraTable->addTopLevelItem(cameraNoItem);

    QTreeWidgetItem* cameraConstItem = new QTreeWidgetItem();
    cameraConstItem->setText(0, "Camera Constant");
    cameraConstItem->setText(1, QString::number(camComb->cameras[0]->cameraConstant));
    cameraConstItem->setText(2, QString::number(camComb->cameras[1]->cameraConstant));
    cameraConstItem->setText(3, QString::number(camComb->cameras[2]->cameraConstant));
    cameraTable->addTopLevelItem(cameraConstItem);

    QTreeWidgetItem* cameraStdDevItem = new QTreeWidgetItem();
    cameraStdDevItem->setText(0, "C Std. Dev.");
    cameraStdDevItem->setText(1, QString::number(camComb->cameras[0]->cameraConstantStdDev));
    cameraStdDevItem->setText(2, QString::number(camComb->cameras[1]->cameraConstantStdDev));
    cameraStdDevItem->setText(3, QString::number(camComb->cameras[2]->cameraConstantStdDev));
    cameraTable->addTopLevelItem(cameraStdDevItem);

    QTreeWidgetItem* xhStdDevItem = new QTreeWidgetItem();
    xhStdDevItem->setText(0, "XH Std. Dev.");
    xhStdDevItem->setText(1, QString::number(camComb->cameras[0]->XHStdDev));
    xhStdDevItem->setText(2, QString::number(camComb->cameras[1]->XHStdDev));
    xhStdDevItem->setText(3, QString::number(camComb->cameras[2]->XHStdDev));
    cameraTable->addTopLevelItem(xhStdDevItem);

    QTreeWidgetItem* yhStdDevItem = new QTreeWidgetItem();
    yhStdDevItem->setText(0, "YH Std. Dev.");
    yhStdDevItem->setText(1, QString::number(camComb->cameras[0]->YHStdDev));
    yhStdDevItem->setText(2, QString::number(camComb->cameras[1]->YHStdDev));
    yhStdDevItem->setText(3, QString::number(camComb->cameras[2]->YHStdDev));
    cameraTable->addTopLevelItem(yhStdDevItem);

    QTreeWidgetItem* f1StdDevItem = new QTreeWidgetItem();
    f1StdDevItem->setText(0, "F1 Std. Dev.");
    f1StdDevItem->setText(1, QString::number(camComb->cameras[0]->F1StdDev));
    f1StdDevItem->setText(2, QString::number(camComb->cameras[1]->F1StdDev));
    f1StdDevItem->setText(3, QString::number(camComb->cameras[2]->F1StdDev));
    cameraTable->addTopLevelItem(f1StdDevItem);

    QTreeWidgetItem* f2StdDevItem = new QTreeWidgetItem();
    f2StdDevItem->setText(0, "F2 Std. Dev.");
    f2StdDevItem->setText(1, QString::number(camComb->cameras[0]->F2StdDev));
    f2StdDevItem->setText(2, QString::number(camComb->cameras[1]->F2StdDev));
    f2StdDevItem->setText(3, QString::number(camComb->cameras[2]->F2StdDev));
    cameraTable->addTopLevelItem(f2StdDevItem);

    QTreeWidgetItem* p1StdDevItem = new QTreeWidgetItem();
    p1StdDevItem->setText(0, "P1 Std. Dev.");
    p1StdDevItem->setText(1, QString::number(camComb->cameras[0]->P1StdDev));
    p1StdDevItem->setText(2, QString::number(camComb->cameras[1]->P1StdDev));
    p1StdDevItem->setText(3, QString::number(camComb->cameras[2]->P1StdDev));
    cameraTable->addTopLevelItem(p1StdDevItem);

    QTreeWidgetItem* p2StdDevItem = new QTreeWidgetItem();
    p2StdDevItem->setText(0, "P2 Std. Dev.");
    p2StdDevItem->setText(1, QString::number(camComb->cameras[0]->P2StdDev));
    p2StdDevItem->setText(2, QString::number(camComb->cameras[1]->P2StdDev));
    p2StdDevItem->setText(3, QString::number(camComb->cameras[2]->P2StdDev));
    cameraTable->addTopLevelItem(p2StdDevItem);

    cameraTable->resizeColumnToContents(0);
    cameraTable->resizeColumnToContents(1);
    cameraTable->resizeColumnToContents(2);
    cameraTable->resizeColumnToContents(3);
}