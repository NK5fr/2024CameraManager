
#include "qopenglvideowidget.h"

QOpenglVideoWidget::QOpenglVideoWidget() {
    setMouseTracking(true);
    imageDetect = nullptr;
    mouseIn = false;
    trackPointProperty = nullptr;
    imgBuffer = nullptr;
    bufferSize = 0;
    imageWidth = 0;
    imageHeight = 0;
    initializeGL();
    update();
    connect(this, SIGNAL(forceUpdate()), this, SLOT(receiveUpdate()));
}

QOpenglVideoWidget::~QOpenglVideoWidget() {

}

void QOpenglVideoWidget::setImage(unsigned char* imgBuffer, unsigned int bufferSize, unsigned int imageWidth, unsigned int imageHeight) {

    if (imgBuffer == nullptr) return;
    if (this->imageWidth != imageWidth || this->imageHeight != imageHeight) {
        texture.createEmptyTexture(imageWidth, imageHeight);
    }
    texture.updateTexture(imgBuffer, bufferSize);
    this->imgBuffer = imgBuffer;
    this->bufferSize = bufferSize;
    this->imageWidth = imageWidth;
    this->imageHeight = imageHeight;
    resizeEvent();
    emit forceUpdate();
}

void QOpenglVideoWidget::receiveUpdate() {
    //trick to pass the update to the main thread...
    update();
}

void QOpenglVideoWidget::initializeGL() {
    glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, width() * this->devicePixelRatio(), height() * this->devicePixelRatio());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width(), 0, height(), -1, 1);
}

void QOpenglVideoWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    texture.bind();
    glBegin(GL_POLYGON);
    glVertex2d(0, 0);
    glTexCoord2d(0, 0);
    glVertex2d(imageWidth, 0);
    glTexCoord2d(1, 0);
    glVertex2d(imageWidth, imageHeight);
    glTexCoord2d(1, 1);
    glVertex2d(0, imageHeight);
    glTexCoord2d(0, 1);
    glEnd();
}

void QOpenglVideoWidget::resizeGL(int width, int height) {
    initializeGL();
}


void QOpenglVideoWidget::resizeEvent(QResizeEvent *) {
    //initializeGL();
}

void QOpenglVideoWidget::enterEvent(QEvent *) {
    mouseIn = true;
}

void QOpenglVideoWidget::leaveEvent(QEvent *) {
    //if (!Ui::crosshair) return;
    mouseIn = false;
    update();
}

void QOpenglVideoWidget::mouseMoveEvent(QMouseEvent * event) {
    //if (!Ui::crosshair) return;
    //mouse = event->pos();
    update();
}

void QOpenglVideoWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (isMaximized()) showNormal();
    else showMaximized();
}

void QOpenglVideoWidget::changedState(Qt::WindowStates, Qt::WindowStates newState) {
    //active = newState & Qt::WindowActive;
}

void QOpenglVideoWidget::activateCrosshair(bool state) {
    //setMouseTracking(state);
}
