
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef EXTERNAL_PROCESS_H
#define EXTERNAL_PROCESS_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets/qtextedit.h>

/*
    ExternalProcess-class
    Creates QProcess with logging of output to QTextEdit.. 
*/
class ExternalProcess : public QProcess {
    Q_OBJECT
public:
    ExternalProcess();
    ~ExternalProcess();

    inline QTextEdit* getTextEdit() { return edit; }

private slots:
    void readStdOutput();
    void readErrOutput();
    void readError();
    void terminateRun(QObject*);


private:
    QTextEdit* edit;
};

#endif
