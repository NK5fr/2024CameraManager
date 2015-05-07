#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QtCore>
#include <QWidget>
#include <QLabel>

class ImageLabel : public QWidget {
    Q_OBJECT

public:
    explicit ImageLabel(QWidget *parent = 0);
    const QPixmap* pixmap() const;

    public slots:
    void setPixmap(const QPixmap&);

protected:
    void resizeEvent(QResizeEvent *);

    private slots:
    void resizeImage();

private:
    QLabel *label;
};


#endif