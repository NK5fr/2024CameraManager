#include "imageviewerwidget.h"

using namespace std;

ImageViewerWidget::ImageViewerWidget(QString path, QString filename) : path(path), filename(filename) {
    fullPath = path + "/" + filename;
    setImageFromFile(fullPath);
    setWidget(&imageWidget);
    setWindowTitle(filename);

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

    QPixmap image;
    bool loaded = image.load(filepath);
    if (loaded) {
        imageWidget.getImageWidget()->setImage(image);
        imageWidget.update();
    } else {
        qDebug("Image could not be loaded...\n");
    }
}

/*
void ImageViewerWidget::mousePressEvent(QMouseEvent* event) {
}*/

void ImageViewerWidget::wheelEvent(QWheelEvent* event) {
    int tempSelectedImageInFolder = max(0, min((event->delta() / 120) + selectedImageInFolder, imageFiles.size() - 1));
    if (tempSelectedImageInFolder != selectedImageInFolder) {
        selectedImageInFolder = tempSelectedImageInFolder;
        setImageFromFile(imageFiles[selectedImageInFolder].absoluteFilePath());
        setWindowTitle(imageFiles[selectedImageInFolder].fileName());
    }
}
