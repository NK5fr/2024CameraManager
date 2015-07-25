#include "imageviewerwidget.h"

using namespace std;

ImageViewerWidget::ImageViewerWidget(QString path, QString filename, TrackPointProperty* trackPoint) : path(path), filename(filename) {
    fullPath = path + "/" + filename;
    setImageFromFile(fullPath);
    //setWidget(&imageWidget);
    setWindowTitle(filename);
    imageWidget.getImageWidget()->setTrackPointProperty(trackPoint);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(&imageWidget);
    layout->addWidget(&imageFileSlider);
    QWidget* widget = new QWidget();
    widget->setLayout(layout);
    setWidget(widget);

    imageFiles = QDir(path).entryInfoList(QStringList() << "*.pgm", QDir::Filter::Files);
    imageFileSlider.setMaximum(imageFiles.size() - 1);
    imageFileSlider.setOrientation(Qt::Orientation::Horizontal);
    imageFileSlider.setTickInterval(1);
    imageFileSlider.setTickPosition(QSlider::TickPosition::TicksBelow);
    for (int i = 0; i < imageFiles.size(); i++) {
        if (imageFiles[i].fileName() == filename) {
            selectedImageInFolder = i;
            break;
        }
    }

    connect(&imageFileSlider, SIGNAL(valueChanged(int)), this, SLOT(imageFileSliderChanged(int)));
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
    
    imageWidget.getImageWidget()->updateImage((unsigned char*) imageData, imageSize, width, height);
    imageWidget.getImageWidget()->update();
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

char* ImageViewerWidget::unpackPGMFile(const QString& filepath, int64_t* bufferSize, int64_t* imageWidth, int64_t* imageHeight) {
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
        *imageWidth = imageWidget.getImageWidget()->getTexture()->getTextureWidth();
        *imageHeight = imageWidget.getImageWidget()->getTexture()->getTextureHeight();
        imageSize = imageWidget.getImageWidget()->getTexture()->getTextureSize();
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
