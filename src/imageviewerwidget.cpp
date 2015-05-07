#include "imageviewerwidget.h"

using namespace std;

ImageViewerWidget::ImageViewerWidget(QString path, QString filename) : path(path), filename(filename) {
    fullPath = path + "/" + filename;
    setImageFromFile(fullPath);
    setWidget(&imageWidget);
    setWindowTitle(filename);
}

ImageViewerWidget::~ImageViewerWidget() {

}

void ImageViewerWidget::setImageFromFile(const QString& filepath) {
    QFile file(filepath);
    if (!file.exists()) {
        qDebug("File does not exist: %s\n", filepath);
        return;
    }

    QPixmap image;
    bool loaded = image.load(filepath);
    if (loaded) {
        imageWidget.getImageWidget()->setImage(image); 
    } else {
        qDebug("Image could not be loaded...\n");
    }
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* event) {
}

void ImageViewerWidget::wheelEvent(QWheelEvent* event) {
}