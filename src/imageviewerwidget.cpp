#include "imageviewerwidget.h"

using namespace std;

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
    int tempSelectedImageInFolder = max(0, min((event->delta() / 120) + selectedImageInFolder, imageFiles.size() - 1));
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

void ImageViewerWidget::enableSubRegionsStateChanged(int state) {
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

    zoomFactorComboBox.addItems(QStringList() << "1" << "2" << "4" << "8" << "16" << "32" << "64");
    zoomFactorComboBox.setCurrentIndex(zoomFactorComboBox.findData(QString::number(imageWidget.getZoomFactor(), 'f', 0), Qt::DisplayRole));
    zoomAreaSizeComboBox.addItems(QStringList() << "100" << "200" << "250" << "300");
    zoomAreaSizeComboBox.setCurrentIndex(zoomAreaSizeComboBox.findData(QString::number(imageWidget.getZoomAreaSize(), 'f', 0), Qt::DisplayRole));

    QHBoxLayout* optionsLayout = new QHBoxLayout();
    optionsLayout->addLayout(imageSubRegionsLayout);
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

    connect(&enableSubRegions, SIGNAL(stateChanged(int)), this, SLOT(enableSubRegionsStateChanged(int)));
    connect(&numSubRegionsX, SIGNAL(valueChanged(int)), this, SLOT(numSubRegionsXChanged(int)));
    connect(&numSubRegionsY, SIGNAL(valueChanged(int)), this, SLOT(numSubRegionsYChanged(int)));
    connect(&imageFileSlider, SIGNAL(valueChanged(int)), this, SLOT(imageFileSliderChanged(int)));
    connect(&zoomFactorComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(zoomFactorChanged(const QString&)));
    connect(&zoomAreaSizeComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(zoomAreaSizeChanged(const QString&)));
}
