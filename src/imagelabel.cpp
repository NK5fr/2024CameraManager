
#include "imagelabel.h"


ImageLabel::ImageLabel(QWidget* parent):QWidget(parent) {}

void ImageLabel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    resizeImage();
}

const QPixmap ImageLabel::pixmap() const {
    return label->pixmap();
}

void ImageLabel::setPixmap(const QPixmap &pixmap) {
    label->setPixmap(pixmap);
    resizeImage();
}

void ImageLabel::resizeImage() {
    QSize pixSize = label->pixmap().size();
    pixSize.scale(size(), Qt::KeepAspectRatio);
    label->setFixedSize(pixSize);
}
