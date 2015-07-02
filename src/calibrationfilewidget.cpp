
#include "calibrationfilewidget.h"

CalibrationFileWidget::CalibrationFileWidget(QWidget* parent, CalibrationFile* calibFile) : QWidget(parent) {
    this->calibFile = calibFile;
    failedColor = QColor(255, 0, 0, 150);
    warningColor = QColor(255, 255, 0, 150);
    okColor = QColor(0, 255, 0, 150);
    initUI();
}

void CalibrationFileWidget::initUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout();
    QVBoxLayout* leftLayout = new QVBoxLayout();
    QGridLayout* gridLayout = new QGridLayout();

    combinationPreviewWidget = new CalibrationFileOpenGLWidget(calibFile);
    leftLayout->addWidget(combinationPreviewWidget);

    const bool filterOnTextChanged = false;
    QVBoxLayout* subLeftLayout = new QVBoxLayout();
    filterList = new QTreeWidget(this);
    filterList->setHeaderLabels(QStringList() << "" << "Warning value" << "Failed value");
    QTreeWidgetItem* s0Filter = new QTreeWidgetItem(filterList);
    s0Filter->setText(0, "S0");
    filterList->addTopLevelItem(s0Filter);
    QLineEdit* s0WarningEdit = new QLineEdit("0.13", this);
    s0WarningEdit->setFrame(false);
    if (filterOnTextChanged) connect(s0WarningEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(s0WarningEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    QLineEdit* s0FailedEdit = new QLineEdit("10", this);
    s0FailedEdit->setFrame(false);
    if (filterOnTextChanged) connect(s0FailedEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(s0FailedEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    filterList->setItemWidget(s0Filter, 1, s0WarningEdit);
    filterList->setItemWidget(s0Filter, 2, s0FailedEdit);
    QTreeWidgetItem* cStdDevFilter = new QTreeWidgetItem(filterList);
    cStdDevFilter->setText(0, "C std. dev.");
    filterList->addTopLevelItem(cStdDevFilter);
    QLineEdit* cStdDevWarningEdit = new QLineEdit("2", this);
    cStdDevWarningEdit->setFrame(false);
    if (filterOnTextChanged) connect(cStdDevWarningEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(cStdDevWarningEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    QLineEdit* cStdDevFailedEdit = new QLineEdit("5", this);
    cStdDevFailedEdit->setFrame(false);
    if (filterOnTextChanged) connect(cStdDevFailedEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(cStdDevFailedEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    filterList->setItemWidget(cStdDevFilter, 1, cStdDevWarningEdit);
    filterList->setItemWidget(cStdDevFilter, 2, cStdDevFailedEdit);
    QTreeWidgetItem* XHStdDevFilter = new QTreeWidgetItem(filterList);
    XHStdDevFilter->setText(0, "XH std. dev.");
    filterList->addTopLevelItem(XHStdDevFilter);
    QLineEdit* XHStdDevWarningEdit = new QLineEdit("2", this);
    XHStdDevWarningEdit->setFrame(false);
    if (filterOnTextChanged) connect(XHStdDevWarningEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(XHStdDevWarningEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    QLineEdit* XHStdDevFailedEdit = new QLineEdit("5", this);
    XHStdDevFailedEdit->setFrame(false);
    if (filterOnTextChanged) connect(XHStdDevFailedEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(XHStdDevFailedEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    filterList->setItemWidget(XHStdDevFilter, 1, XHStdDevWarningEdit);
    filterList->setItemWidget(XHStdDevFilter, 2, XHStdDevFailedEdit);
    QTreeWidgetItem* YHStdDevFilter = new QTreeWidgetItem(filterList);
    YHStdDevFilter->setText(0, "YH std. dev.");
    filterList->addTopLevelItem(YHStdDevFilter);
    QLineEdit* YHStdDevWarningEdit = new QLineEdit("2", this);
    YHStdDevWarningEdit->setFrame(false);
    if (filterOnTextChanged) connect(YHStdDevWarningEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(YHStdDevWarningEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    QLineEdit* YHStdDevFailedEdit = new QLineEdit("5", this);
    YHStdDevFailedEdit->setFrame(false);
    if (filterOnTextChanged) connect(YHStdDevFailedEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(YHStdDevFailedEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    filterList->setItemWidget(YHStdDevFilter, 1, YHStdDevWarningEdit);
    filterList->setItemWidget(YHStdDevFilter, 2, YHStdDevFailedEdit);
    QTreeWidgetItem* noFramesFilter = new QTreeWidgetItem(filterList);
    noFramesFilter->setText(0, "# of frames");
    filterList->addTopLevelItem(noFramesFilter);
    QLineEdit* noFramesWarningEdit = new QLineEdit("55", this);
    noFramesWarningEdit->setFrame(false);
    if (filterOnTextChanged) connect(noFramesWarningEdit, SIGNAL(textChanged(const QString&)), this, SLOT(filtersChanged()));
    else connect(noFramesWarningEdit, SIGNAL(returnPressed()), this, SLOT(filtersChanged()));
    filterList->setItemWidget(noFramesFilter, 1, noFramesWarningEdit);
    filterList->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
    subLeftLayout->addWidget(filterList);

    for (int i = 0; i < calibFile->getCameraCombinations().size(); i++) {
        updateFiltersCamerasOnly(calibFile->getCameraCombinations()[i]);
    }

    selectedCombinationTable = new QTreeWidget(this);
    selectedCombinationTable->setHeaderLabels(QStringList() << "" << "Cam 0" << "Cam 1" << "Cam 2" << "");
    subLeftLayout->addWidget(selectedCombinationTable);

    QHBoxLayout* subPreviewWindowLayout = new QHBoxLayout();
    subPreviewWindowLayout->addLayout(subLeftLayout);

    cameraTable = new QTreeWidget(this);
    cameraTable->setHeaderLabels(QStringList() << "Cam No." << "OK Combs." << "Warning Combs." << "Failed Combs.");
    subPreviewWindowLayout->addWidget(cameraTable);
    leftLayout->addLayout(subPreviewWindowLayout);

    updateCameraTable();

    combinationList = new QTreeWidget(this);
    combinationList->setHeaderLabels(QStringList() << "Combination" << "S0" << "# frames");
    combinationList->setColumnCount(3);
    combinationList->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Expanding);

    updateCombinationTable();

    leftLayout->setStretchFactor(combinationPreviewWidget, 2);
    leftLayout->setStretchFactor(filterList, 1);
    leftLayout->setStretchFactor(cameraTable, 1);

    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(combinationList);
    setLayout(mainLayout);

    combinationPreviewWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    connect(combinationList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(combinationClicked(QTreeWidgetItem*, int)));
    connect(cameraTable, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(cameraClicked(QTreeWidgetItem*, int)));
    connect(combinationList, SIGNAL(itemSelectionChanged()), this, SLOT(combinationSelectionChanged()));
    connect(cameraTable, SIGNAL(itemSelectionChanged()), this, SLOT(cameraSelectionChanged()));
}

void CalibrationFileWidget::updateCombinationTable() {
    combinationList->clear();
    vector<TrackPoint::CameraCombination*> camComb = calibFile->getCameraCombinations();
    for (int i = 0; i < camComb.size(); i++) {
        QString combination = QString::number(camComb[i]->camNumbers[0]) + "_" + QString::number(camComb[i]->camNumbers[1]) + "_" + QString::number(camComb[i]->camNumbers[2]);
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, combination);
        item->setText(1, QString::number(camComb[i]->s0));
        item->setText(2, QString::number(camComb[i]->numFrames));
        item->setData(0, Qt::UserRole, QVariant::fromValue((quintptr) camComb[i]));
        updateFiltersCamerasOnly(camComb[i]);
        if (camComb[i]->status == TrackPoint::CalibrationStatus::Warning) {
            item->setBackgroundColor(0, warningColor);
        } else if (camComb[i]->status == TrackPoint::CalibrationStatus::Failed) {
            item->setBackgroundColor(0, failedColor);
        } else {
            item->setBackgroundColor(0, okColor);
        }
        bool warningS0 = (camComb[i]->s0 > ((QLineEdit*) filterList->itemWidget(filterList->invisibleRootItem()->child(0), 1))->text().toDouble());
        bool failedS0 = (camComb[i]->s0 > ((QLineEdit*) filterList->itemWidget(filterList->invisibleRootItem()->child(0), 2))->text().toDouble());
        if (warningS0) item->setBackgroundColor(1, warningColor);
        if (failedS0) item->setBackgroundColor(1, failedColor);
        if (!warningS0 && !failedS0) item->setBackgroundColor(1, okColor);
        if (camComb[i]->numFrames < ((QLineEdit*) filterList->itemWidget(filterList->invisibleRootItem()->child(4), 1))->text().toDouble()) {
            item->setBackgroundColor(2, failedColor);
        } else {
            item->setBackgroundColor(2, okColor);
        }
        combinationList->addTopLevelItem(item);
    }
    combinationList->resizeColumnToContents(0);
    combinationList->resizeColumnToContents(1);
    combinationList->resizeColumnToContents(2);
}

void CalibrationFileWidget::filtersChanged() {
    //updateFilters();
    updateCombinationTable();
    updateCameraTable();
    if (lastSelectedCombination == nullptr) return;
    updateCameraCombinationTable(lastSelectedCombination);
}

void CalibrationFileWidget::updateCameraTable() {
    cameraTable->clear();
    vector<TrackPoint::CameraCombination*> camComb = calibFile->getCameraCombinations();
    vector<TrackPoint::Camera*> cams = calibFile->getCameras();
    for (int i = 0; i < cams.size(); i++) {
        int ok = 0;
        int warning = 0;
        int failed = 0;
        for (int j = 0; j < camComb.size(); j++) {
            if (cams[i]->camNo != camComb[j]->camNumbers[0] && cams[i]->camNo != camComb[j]->camNumbers[1] && cams[i]->camNo != camComb[j]->camNumbers[2]) {
                failed++;
                continue;
            }
            if (camComb[j]->status == TrackPoint::OK) {
                ok++;
            } else if (camComb[j]->status == TrackPoint::Warning) {
                warning++;
            } else if (camComb[j]->status == TrackPoint::Failed) {
                failed++;
            }
        }
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, QString::number(cams[i]->camNo));
        item->setText(1, QString::number(ok));
        item->setText(2, QString::number(warning));
        item->setText(3, QString::number(failed) + "/" + QString::number(camComb.size()));
        cameraTable->addTopLevelItem(item);
    }
}

void CalibrationFileWidget::updateFiltersCamerasOnly(TrackPoint::CameraCombination* comb) {
    QTreeWidgetItem* invisItem = filterList->invisibleRootItem();
    comb->status = TrackPoint::CalibrationStatus::OK;
    for (int j = 0; j < 3; j++) {
        if (comb->cameras[j] != nullptr) {
            double warningCStd = ((QLineEdit*) filterList->itemWidget(invisItem->child(2), 1))->text().toDouble();
            double maxCStd = ((QLineEdit*) filterList->itemWidget(invisItem->child(2), 2))->text().toDouble();
            if (comb->cameras[j]->cameraConstantStdDev >((QLineEdit*) filterList->itemWidget(invisItem->child(1), 1))->text().toDouble()) {
                comb->status = TrackPoint::CalibrationStatus::Warning;
            }
            if (comb->cameras[j]->cameraConstantStdDev >((QLineEdit*) filterList->itemWidget(invisItem->child(1), 2))->text().toDouble()) {
                comb->status = TrackPoint::CalibrationStatus::Failed;
                break;
            }
            if (comb->cameras[j]->XHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(2), 1))->text().toDouble()) {
                comb->status = TrackPoint::CalibrationStatus::Warning;
            }
            if (comb->cameras[j]->XHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(2), 2))->text().toDouble()) {
                comb->status = TrackPoint::CalibrationStatus::Failed;
                break;
            }
            if (comb->cameras[j]->YHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(3), 1))->text().toDouble()) {
                comb->status = TrackPoint::CalibrationStatus::Warning;
            }
            if (comb->cameras[j]->YHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(3), 2))->text().toDouble()) {
                comb->status = TrackPoint::CalibrationStatus::Failed;
                break;
            }
        } else {
            comb->status = TrackPoint::CalibrationStatus::Failed;
        }
    }
}

void CalibrationFileWidget::cameraClicked(QTreeWidgetItem* item, int column) {
    combinationPreviewWidget->setSelectedCamera(item->text(0).toInt());
    combinationPreviewWidget->setSelectedCameraCombination(nullptr);
    combinationPreviewWidget->update();
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
        updateCameraCombinationTable(camComb);
    }
    lastSelectedCombination = camComb;
    combinationPreviewWidget->setSelectedCamera(-1);
    combinationPreviewWidget->setSelectedCameraCombination(camComb);
    combinationPreviewWidget->update();
}

void CalibrationFileWidget::cameraSelectionChanged() {
    if (cameraTable->selectedItems().size() == 0) return;
    QTreeWidgetItem* item = cameraTable->selectedItems()[0];
    cameraClicked(item, 0);
}

void CalibrationFileWidget::combinationSelectionChanged() {
    if (combinationList->selectedItems().size() == 0) return;
    QTreeWidgetItem* item = combinationList->selectedItems()[0];
    combinationClicked(item, 0);
}

void CalibrationFileWidget::updateCameraCombinationTable(TrackPoint::CameraCombination* camComb) {
    selectedCombinationTable->clear();
    if (camComb == nullptr) return;
    QTreeWidgetItem* invisItem = filterList->invisibleRootItem();

    for (int i = 0; i < 3; i++) {
        if (camComb->cameras[i] == nullptr) return;
    }

    QTreeWidgetItem* cameraNoItem = new QTreeWidgetItem();
    cameraNoItem->setText(0, "Camera No");
    cameraNoItem->setText(1, QString::number(camComb->cameras[0]->camNo));
    cameraNoItem->setText(2, QString::number(camComb->cameras[1]->camNo));
    cameraNoItem->setText(3, QString::number(camComb->cameras[2]->camNo));
    selectedCombinationTable->addTopLevelItem(cameraNoItem);

    QTreeWidgetItem* cameraConstItem = new QTreeWidgetItem();
    cameraConstItem->setText(0, "Camera Constant");
    for (int i = 0; i < 3; i++) {
        cameraConstItem->setText(i + 1, QString::number(camComb->cameras[i]->cameraConstant));
    }
    selectedCombinationTable->addTopLevelItem(cameraConstItem);

    QTreeWidgetItem* cameraStdDevItem = new QTreeWidgetItem();
    cameraStdDevItem->setText(0, "C Std. Dev.");
    for (int i = 0; i < 3; i++) {
        cameraStdDevItem->setText(i + 1, QString::number(camComb->cameras[i]->cameraConstantStdDev));
        if (camComb->cameras[i]->cameraConstantStdDev >((QLineEdit*) filterList->itemWidget(invisItem->child(1), 1))->text().toDouble()) {
            cameraStdDevItem->setBackgroundColor(i + 1, warningColor);
        }
        if (camComb->cameras[i]->cameraConstantStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(1), 2))->text().toDouble()) {
            cameraStdDevItem->setBackgroundColor(i + 1, failedColor);
        }
    }
    selectedCombinationTable->addTopLevelItem(cameraStdDevItem);

    QTreeWidgetItem* xhStdDevItem = new QTreeWidgetItem();
    xhStdDevItem->setText(0, "XH Std. Dev.");
    for (int i = 0; i < 3; i++) {
        xhStdDevItem->setText(i + 1, QString::number(camComb->cameras[i]->XHStdDev));
        if (camComb->cameras[i]->XHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(2), 1))->text().toDouble()) {
            xhStdDevItem->setBackgroundColor(i + 1, warningColor);
        }
        if (camComb->cameras[i]->XHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(2), 2))->text().toDouble()) {
            xhStdDevItem->setBackgroundColor(i + 1, failedColor);
        }
    }
    selectedCombinationTable->addTopLevelItem(xhStdDevItem);

    QTreeWidgetItem* yhStdDevItem = new QTreeWidgetItem();
    yhStdDevItem->setText(0, "YH Std. Dev.");
    for (int i = 0; i < 3; i++) {
        yhStdDevItem->setText(i + 1, QString::number(camComb->cameras[i]->YHStdDev));
        if (camComb->cameras[i]->YHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(3), 1))->text().toDouble()) {
            yhStdDevItem->setBackgroundColor(i + 1, warningColor);
        }
        if (camComb->cameras[i]->YHStdDev > ((QLineEdit*) filterList->itemWidget(invisItem->child(3), 2))->text().toDouble()) {
            yhStdDevItem->setBackgroundColor(i + 1, failedColor);
        }
    }
    selectedCombinationTable->addTopLevelItem(yhStdDevItem);

    QTreeWidgetItem* f1StdDevItem = new QTreeWidgetItem();
    f1StdDevItem->setText(0, "F1 Std. Dev.");
    for (int i = 0; i < 3; i++) {
        f1StdDevItem->setText(i + 1, QString::number(camComb->cameras[i]->F1StdDev));
    }
    selectedCombinationTable->addTopLevelItem(f1StdDevItem);

    QTreeWidgetItem* f2StdDevItem = new QTreeWidgetItem();
    f2StdDevItem->setText(0, "F2 Std. Dev.");
    for (int i = 0; i < 3; i++) {
        f2StdDevItem->setText(i + 1, QString::number(camComb->cameras[i]->F2StdDev));
    }
    selectedCombinationTable->addTopLevelItem(f2StdDevItem);

    QTreeWidgetItem* p1StdDevItem = new QTreeWidgetItem();
    p1StdDevItem->setText(0, "P1 Std. Dev.");
    for (int i = 0; i < 3; i++) {
        p1StdDevItem->setText(i + 1, QString::number(camComb->cameras[i]->P1StdDev));
    }
    selectedCombinationTable->addTopLevelItem(p1StdDevItem);

    QTreeWidgetItem* p2StdDevItem = new QTreeWidgetItem();
    p2StdDevItem->setText(0, "P2 Std. Dev.");
    for (int i = 0; i < 3; i++) {
        p2StdDevItem->setText(i + 1, QString::number(camComb->cameras[i]->P2StdDev));
    }
    selectedCombinationTable->addTopLevelItem(p2StdDevItem);

    selectedCombinationTable->resizeColumnToContents(0);
    selectedCombinationTable->resizeColumnToContents(1);
    selectedCombinationTable->resizeColumnToContents(2);
    selectedCombinationTable->resizeColumnToContents(3);
}