#include "imageviewerwidget.h"

using namespace std;

ImageViewerWidget::ImageViewerWidget(QString path, QString filename, TrackPointProperty* trackPoint) : path(path), filename(filename) {
    fullPath = path + "/" + filename;
    setImageFromFile(fullPath);
    setWidget(&imageWidget);
    setWindowTitle(filename);
    imageWidget.getImageWidget()->setTrackPointProperty(trackPoint);

    imageFiles = QDir(path).entryInfoList(QStringList() << "*.pgm", QDir::Filter::Files);
    for (int i = 0; i < imageFiles.size(); i++) {
        if (imageFiles[i].fileName() == filename) {
            selectedImageInFolder = i;
            break;
        }
    }
}

ImageViewerWidget::~ImageViewerWidget() {

}

void ImageViewerWidget::setImageFromFile(const QString& filepath) {
    QFile file(filepath);
    if (!file.exists()) {
        qDebug() << "File does not exist: " << filepath;
        return;
    }
    //QPixmap image;
    if (!file.open(QIODevice::ReadOnly)) return;
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
        if (data[i] == '\n') {
            QString label(tempBuffer);
            if (element == 0) {
                if (!label.contains("P5")) return;
            } else if (element == 1) {
                width = label.toUInt();
            } else if (element == 2) {
                height = label.toUInt();
            }
            element++;
            startIndex = i + 1;
        } else {
            if (element == 4) {
                startIndex = i;
                break;
            }
            if (i - startIndex >= tempBufferSize - 1) break;
            tempBuffer[i - startIndex] = data[i];
            tempBuffer[(i - startIndex) + 1] = '\0';
        }
    }
    if (element < 4) {
        width = imageWidget.getImageWidget()->getTexture()->getTextureWidth();
        height = imageWidget.getImageWidget()->getTexture()->getTextureHeight();
        imageSize = imageWidget.getImageWidget()->getTexture()->getTextureSize();
        imageData = new char[imageSize];
        memset(imageData, 0, imageSize);
        imageWidget.getImageWidget()->updateImage((unsigned char*) imageData, imageSize, width, height);
        imageWidget.getImageWidget()->update();
        delete[] imageData;
        return;
    }
    imageSize = blob.size() - startIndex;
    imageData = new char[imageSize];
    memcpy(imageData, data + startIndex, imageSize);
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
    }
}
