#include "imageviewerwidget.h"

ImageViewerWidget::ImageViewerWidget(QString path, QString filename, TrackPointProperty* trackPoint) : path(path), filename(filename) {
    fullPath = path + "/" + filename;
    setImageFromFile(fullPath);
    setWindowTitle(filename);
    imageWidget.setTrackPointProperty(trackPoint);
    imageFiles = QDir(path).entryInfoList(QStringList() << "*.pgm", QDir::Filter::Files);
    for (int i = 0; i < imageFiles.size(); i++) {
        if (imageFiles[i].fileName() == filename) {
            selectedImageInFolder = i;
            break;
        }
    }
    initGUI();

    imageFileSlider.setSliderPosition(selectedImageInFolder);
}

ImageViewerWidget::~ImageViewerWidget() {

}

void ImageViewerWidget::setImageFromFile(const QString& filepath) {
    QFile file(filepath);
    if (!file.exists()) {
        qDebug() << "File does not exist: " << filepath;
        return;
    }
    int64_t width = 0;
    int64_t height = 0;
    int64_t imageSize = 0;
    char* imageData = unpackPGMFile(filepath, &imageSize, &width, &height);
    
    imageWidget.updateImage((unsigned char*) imageData, imageSize, width, height);
    imageWidget.update();
    delete[] imageData;
}

void ImageViewerWidget::wheelEvent(QWheelEvent* event) {
    int tempSelectedImageInFolder = std::max(0, std::min((event->angleDelta().y() / 120) + selectedImageInFolder, (int) imageFiles.size() - 1));
    if (tempSelectedImageInFolder != selectedImageInFolder) {
        selectedImageInFolder = tempSelectedImageInFolder;
        setImageFromFile(imageFiles[selectedImageInFolder].absoluteFilePath());
        //imageWidget.update();
        setWindowTitle(imageFiles[selectedImageInFolder].fileName());
        imageFileSlider.setSliderPosition(selectedImageInFolder);
    }
}

void ImageViewerWidget::imageFileSliderChanged(int value) {
    selectedImageInFolder = value;
    setImageFromFile(imageFiles[selectedImageInFolder].absoluteFilePath());
    setWindowTitle(imageFiles[selectedImageInFolder].fileName());
}

void ImageViewerWidget::enableSubRegionsStateChanged(Qt::CheckState state) {
    imageWidget.setEnableSubImage(state == Qt::CheckState::Checked);
    imageWidget.update();
}

void ImageViewerWidget::numSubRegionsXChanged(int val) {
    imageWidget.setNumSubImagesX(val);
    updateSubRegionSize();
    imageWidget.update();
}

void ImageViewerWidget::numSubRegionsYChanged(int val) {
    imageWidget.setNumSubImagesY(val);
    updateSubRegionSize();
    imageWidget.update();
}

void ImageViewerWidget::updateSubRegionSize() {
    subRegionSizeX.setText(QString::number((double) imageWidget.getImageWidth() / imageWidget.getNumSubImagesX(), 'f', 2));
    subRegionSizeY.setText(QString::number((double) imageWidget.getImageHeight() / imageWidget.getNumSubImagesY(), 'f', 2));
}

void ImageViewerWidget::zoomFactorChanged(const QString& text) {
    imageWidget.setZoomFactor(text.toDouble());
}

void ImageViewerWidget::zoomAreaSizeChanged(const QString& text) {
    imageWidget.setZoomAreaSize(text.toDouble());
}

void ImageViewerWidget::enablePointStateChanged(Qt::CheckState val) {
    imageWidget.setShowPointSeries(val == Qt::CheckState::Checked);
    update();
}

void ImageViewerWidget::customPointStringStateChanged(Qt::CheckState val) {
    imageWidget.setShowPointSeriesLabel(val == Qt::CheckState::Checked);
    update();
}

void ImageViewerWidget::singlePointClicked() {
    imageWidget.setSinglePointSeries(true);
    update();
}

void ImageViewerWidget::multiplePointClicked() {
    imageWidget.setSinglePointSeries(false);
    update();
}

void ImageViewerWidget::createPointFileClicked() {
    PointFileCreatorDialog* pfcd = new PointFileCreatorDialog(this);
    pfcd->exec(); // blocking function
    PointFileInfo fileInfo = pfcd->getFileInfo();
    QString fileContain;
    if (fileInfo.filePath.isEmpty()) return;
    double subWidth = (imageWidget.getImageWidth() / ((imageWidget.isEnableSubImage()) ? imageWidget.getNumSubImagesX() : 1));
    double subHeight = (imageWidget.getImageHeight() / ((imageWidget.isEnableSubImage()) ? imageWidget.getNumSubImagesY() : 1));
    if (fileInfo.allPointsInOneImageFirst) {
        std::vector<TrackPoint::PointInCamera*> newP;
        const std::vector<TrackPoint::PointInCamera*>& p = imageWidget.getPointSeries();
        for (int i = 0; i < ((imageWidget.isEnableSubImage()) ? (imageWidget.getNumSubImagesX() * imageWidget.getNumSubImagesY()) : 1); i++) {
            for (int j = 0; j < p.size(); j++) {
                int pointSubRegionX = (int) p[j]->pointX / (imageWidget.getImageWidth() / ((imageWidget.isEnableSubImage()) ? imageWidget.getNumSubImagesX() : 1));
                int pointSubRegionY = (int) p[j]->pointY / (imageWidget.getImageHeight() / ((imageWidget.isEnableSubImage()) ? imageWidget.getNumSubImagesY() : 1));
                int camIndex = (pointSubRegionY * imageWidget.getNumSubImagesX()) + pointSubRegionX;
                if (camIndex == i) {
                    p[j]->camNo = camIndex;
                    newP.push_back(p[j]);
                }
            }
        }
        for (int i = 0; i < newP.size(); i++) {
            double xPos = ((imageWidget.isEnableSubImage()) ? std::fmod(newP[i]->pointX, (double) imageWidget.getImageWidth() / imageWidget.getNumSubImagesX()) : newP[i]->pointX);
            double yPos = ((imageWidget.isEnableSubImage()) ? std::fmod(newP[i]->pointY, (double) imageWidget.getImageHeight() / imageWidget.getNumSubImagesY()) : newP[i]->pointY);
            if (imageWidget.isEnableShowPointSeriesLabel()) {
                fileContain.append(newP[i]->string + " ");
            }
            fileContain.append(QString::number(xPos, 'f', 0) + " " + QString::number(yPos, 'f', 0) + "\r\n");
        }
    } else {
        std::vector<TrackPoint::PointInCamera*> newP;
        const std::vector<TrackPoint::PointInCamera*>& p = imageWidget.getPointSeries();
        bool* taken = new bool[p.size()];
        for (int i = 0; i < p.size(); i++) {
            taken[i] = false;
        }
        for (int i = 0; i < p.size(); i++) {
            for (int j = 0; j < ((imageWidget.isEnableSubImage()) ? (imageWidget.getNumSubImagesX() * imageWidget.getNumSubImagesY()) : 1); j++) {
                for (int k = 0; k < p.size(); k++) {
                    if (taken[k]) continue;
                    int pointSubRegionX = (int) p[k]->pointX / (imageWidget.getImageWidth() / ((imageWidget.isEnableSubImage()) ? imageWidget.getNumSubImagesX() : 1));
                    int pointSubRegionY = (int) p[k]->pointY / (imageWidget.getImageHeight() / ((imageWidget.isEnableSubImage()) ? imageWidget.getNumSubImagesY() : 1));
                    int camIndex = (pointSubRegionY * imageWidget.getNumSubImagesX()) + pointSubRegionX;
                    if (camIndex == j) {
                        p[k]->camNo = camIndex;
                        newP.push_back(p[k]);
                        taken[k] = true;
                        //i++;
                        break;
                    }
                }
            }
        }
        delete[] taken;

        for (int i = 0; i < newP.size(); i++) {
            double xPos = ((imageWidget.isEnableSubImage()) ? std::fmod(newP[i]->pointX, (double) imageWidget.getImageWidth() / imageWidget.getNumSubImagesX()) : newP[i]->pointX);
            double yPos = ((imageWidget.isEnableSubImage()) ? std::fmod(newP[i]->pointY, (double) imageWidget.getImageHeight() / imageWidget.getNumSubImagesY()) : newP[i]->pointY);
            if (imageWidget.isEnableShowPointSeriesLabel()) {
                fileContain.append(newP[i]->string + " ");
            }
            fileContain.append(QString::number(xPos, 'f', 0) + " " + QString::number(yPos, 'f', 0) + "\r\n");
        }
    }
    QFile file(fileInfo.filePath);
    file.open(QIODevice::WriteOnly);
    QTextStream outstream(&file);
    outstream << fileContain;
    file.close();
}

char* ImageViewerWidget::unpackPGMFile(const QString& filepath, int64_t* const bufferSize, int64_t* const imageWidth, int64_t* const imageHeight) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) return nullptr;
    QByteArray blob = file.readAll();
    const char* data = blob.constData();
    file.close();
    const int tempBufferSize = 128;
    char tempBuffer[tempBufferSize];
    int startIndex = 0;
    int element = 0;
    unsigned int width = 0;
    unsigned int height = 0;
    char* imageData = nullptr;
    unsigned int imageSize = 0;
    for (int i = 0; i < blob.size(); i++) {
        char c = data[i];
        if (element == 4) {
            startIndex = i;
            break;
        }
        if (c == '\0') break;
        if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9') {
            if (i - startIndex >= tempBufferSize - 1) break;
            tempBuffer[i - startIndex] = c;
            tempBuffer[(i - startIndex) + 1] = '\0';
        } else {
            //if (startIndex - i <= 0) continue;
            QString label(tempBuffer);
            if (element == 0) {
                if (!label.contains("P5")) return nullptr;
            } else if (element == 1) {
                width = label.toUInt();
            } else if (element == 2) {
                height = label.toUInt();
            }
            element++;
            startIndex = i + 1;
        }
    }
    if (element < 4) {
        *imageWidth = imageWidget.getTexture()->getTextureWidth();
        *imageHeight = imageWidget.getTexture()->getTextureHeight();
        imageSize = imageWidget.getTexture()->getTextureSize();
        *bufferSize = imageSize;
        imageData = new char[imageSize];
        memset(imageData, 0, imageSize);
        return imageData;
    }
    imageSize = blob.size() - startIndex;
    *bufferSize = imageSize;
    *imageWidth = width;
    *imageHeight = height;
    imageData = new char[imageSize];
    memcpy(imageData, data + startIndex, imageSize);
    return imageData;
}

void ImageViewerWidget::initGUI() {
    QGridLayout* imageSubRegionsLayout = new QGridLayout();
    imageSubRegionsLayout->addWidget(new QLabel("Enable Sub-Regions:"), 0, 0);
    imageSubRegionsLayout->addWidget(&enableSubRegions, 0, 1);
    imageSubRegionsLayout->addWidget(new QLabel("Num. Sub-Region X:"), 1, 0);
    imageSubRegionsLayout->addWidget(&numSubRegionsX, 1, 1);
    imageSubRegionsLayout->addWidget(new QLabel("Num. Sub-Region Y:"), 2, 0);
    imageSubRegionsLayout->addWidget(&numSubRegionsY, 2, 1);
    imageSubRegionsLayout->addWidget(new QLabel("Sub-Region Width:"), 3, 0);
    imageSubRegionsLayout->addWidget(&subRegionSizeX, 3, 1);
    imageSubRegionsLayout->addWidget(new QLabel("Sub-Region Height:"), 4, 0);
    imageSubRegionsLayout->addWidget(&subRegionSizeY, 4, 1);
    imageSubRegionsLayout->setAlignment(Qt::AlignTop);
    imageSubRegionsLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

    enableSubRegions.setChecked(imageWidget.isEnableSubImage());
    numSubRegionsX.setMinimum(1);
    numSubRegionsX.setValue(imageWidget.getNumSubImagesX());
    numSubRegionsY.setMinimum(1);
    numSubRegionsY.setValue(imageWidget.getNumSubImagesY());
    subRegionSizeX.setEnabled(false);
    subRegionSizeY.setEnabled(false);
    updateSubRegionSize();

    QGridLayout* zoomingOptionsLayout = new QGridLayout();
    zoomingOptionsLayout->addWidget(new QLabel("Zoom Factor:"), 0, 0);
    zoomingOptionsLayout->addWidget(&zoomFactorComboBox, 0, 1);
    zoomingOptionsLayout->addWidget(new QLabel("Zoom Area Size:"), 1, 0);
    zoomingOptionsLayout->addWidget(&zoomAreaSizeComboBox, 1, 1);
    zoomingOptionsLayout->setAlignment(Qt::AlignTop);
    zoomingOptionsLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

    zoomFactorComboBox.addItems(QStringList() << "1" << "2" << "4" << "8" << "16" << "32" << "64" << "128");
    zoomFactorComboBox.setCurrentIndex(zoomFactorComboBox.findData(QString::number(imageWidget.getZoomFactor(), 'f', 0), Qt::DisplayRole));
    zoomAreaSizeComboBox.addItems(QStringList() << "100" << "200" << "250" << "300" << "400");
    zoomAreaSizeComboBox.setCurrentIndex(zoomAreaSizeComboBox.findData(QString::number(imageWidget.getZoomAreaSize(), 'f', 0), Qt::DisplayRole));

    QGridLayout* pointSeriesLayout = new QGridLayout();
    pointSeriesLayout->addWidget(new QLabel("Enable Points"), 0, 0);
    pointSeriesLayout->addWidget(&enablePointSeries, 0, 1);
    pointSeriesLayout->addWidget(new QLabel("Custom Point-String"), 1, 0);
    pointSeriesLayout->addWidget(&enablePointStringLabel, 1, 1);
    pointSeriesLayout->addWidget(new QLabel("Single Point"), 2, 0);
    pointSeriesLayout->addWidget(&singlePointSeries, 2, 1);
    pointSeriesLayout->addWidget(new QLabel("Multiple Points"), 3, 0);
    pointSeriesLayout->addWidget(&multiplePointSeries, 3, 1);
    pointSeriesLayout->addWidget(&createPointFile, 4, 0, 1, 2);
    pointSeriesLayout->setAlignment(Qt::AlignTop);
    pointSeriesLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

    enablePointSeries.setChecked(imageWidget.isEnableShowPointSeries());
    enablePointStringLabel.setChecked(imageWidget.isEnableShowPointSeriesLabel());
    singlePointSeries.setChecked(imageWidget.isSinglePointSeries());
    multiplePointSeries.setChecked(!imageWidget.isSinglePointSeries());
    createPointFile.setText("Create Point-file");

    QHBoxLayout* optionsLayout = new QHBoxLayout();
    optionsLayout->addLayout(imageSubRegionsLayout);
    optionsLayout->addLayout(pointSeriesLayout);
    optionsLayout->addLayout(zoomingOptionsLayout);

    imageWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(&imageWidget);
    mainLayout->addWidget(&imageFileSlider);
    mainLayout->addLayout(optionsLayout);

    QWidget* widget = new QWidget();
    widget->setLayout(mainLayout);
    setWidget(widget);

    imageFileSlider.setMaximum(imageFiles.size() - 1);
    imageFileSlider.setOrientation(Qt::Orientation::Horizontal);
    imageFileSlider.setTickInterval(1);
    imageFileSlider.setTickPosition(QSlider::TickPosition::TicksBelow);

    connect(&enableSubRegions, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(enableSubRegionsStateChanged(Qt::CheckState)));
    connect(&numSubRegionsX, SIGNAL(valueChanged(int)), this, SLOT(numSubRegionsXChanged(int)));
    connect(&numSubRegionsY, SIGNAL(valueChanged(int)), this, SLOT(numSubRegionsYChanged(int)));
    connect(&imageFileSlider, SIGNAL(valueChanged(int)), this, SLOT(imageFileSliderChanged(int)));
    connect(&zoomFactorComboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(zoomFactorChanged(const QString&)));
    connect(&zoomAreaSizeComboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(zoomAreaSizeChanged(const QString&)));
    connect(&enablePointSeries, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(enablePointStateChanged(Qt::CheckState)));
    connect(&enablePointStringLabel, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(customPointStringStateChanged(Qt::CheckState)));
    connect(&singlePointSeries, SIGNAL(clicked()), this, SLOT(singlePointClicked()));
    connect(&multiplePointSeries, SIGNAL(clicked()), this, SLOT(multiplePointClicked()));
    connect(&createPointFile, SIGNAL(clicked()), this, SLOT(createPointFileClicked()));
}

PointFileCreatorDialog::PointFileCreatorDialog(QWidget* parent) : QDialog(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->addWidget(new QLabel("Filepath:"));
    fileLayout->addWidget(&filePath);
    fileLayout->addWidget(&filePathUpdate);
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addWidget(&allPointsPerImageFirst, 0, 0);
    gridLayout->addWidget(new QLabel("All points in one image first."), 0, 1);
    gridLayout->addWidget(&onePointPerImageFirst, 1, 0);
    gridLayout->addWidget(new QLabel("One point in all images first."), 1, 1);
    gridLayout->addWidget(&sortByString, 2, 0);
    gridLayout->addWidget(new QLabel("Sort by String. (Not implemented!)"), 2, 1);
    gridLayout->setAlignment(Qt::AlignLeft);
    gridLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(&okButton);
    buttonLayout->addWidget(&closeButton);
    mainLayout->addLayout(fileLayout);
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    filePathUpdate.setText("...");
    okButton.setText("OK");
    closeButton.setText("Close");
    allPointsPerImageFirst.setChecked(fileInfo.allPointsInOneImageFirst);
    onePointPerImageFirst.setChecked(!fileInfo.allPointsInOneImageFirst);
    sortByString.setChecked(fileInfo.sortByString);

    connect(&filePathUpdate, SIGNAL(clicked()), this, SLOT(filePathUpdateClicked()));
    connect(&okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(&closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(&allPointsPerImageFirst, SIGNAL(clicked), this, SLOT(allPointsPerImageChecked()));
    connect(&onePointPerImageFirst, SIGNAL(clicked()), this, SLOT(onePointForAllImagesChecked()));
    connect(&filePath, SIGNAL(textChanged(const QString&)), this, SLOT(filePathChanged(const QString&)));
}

void PointFileCreatorDialog::allPointsPerImageChecked() {
    fileInfo.allPointsInOneImageFirst = true;
}

void PointFileCreatorDialog::onePointForAllImagesChecked() {
    fileInfo.allPointsInOneImageFirst = false;
}

void PointFileCreatorDialog::okButtonClicked() {
    close();
}

void PointFileCreatorDialog::filePathUpdateClicked() {
    fileInfo.filePath = QFileDialog::getSaveFileName(this);
    filePath.setText(fileInfo.filePath);
}

void PointFileCreatorDialog::filePathChanged(const QString& text) {
    fileInfo.filePath = text;
}
