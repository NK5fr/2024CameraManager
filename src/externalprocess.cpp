

#include "externalprocess.h"

ExternalProcess::ExternalProcess() : QProcess() {
    edit = new QTextEdit();
    edit->setFont(QFont("Courier", 9));
    edit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOutput()));
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(readErrOutput()));
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readError()));
    connect(edit, SIGNAL(destroyed(QObject*)), this, SLOT(terminateRun(QObject*)) );
}

ExternalProcess::~ExternalProcess() {
    this->terminate();
    delete edit;
}

void ExternalProcess::readStdOutput() {
    edit->moveCursor(QTextCursor::End);
    edit->insertPlainText(QString::fromLatin1(readAllStandardOutput()));
    edit->moveCursor(QTextCursor::End);
};

void ExternalProcess::readErrOutput() {
    edit->moveCursor(QTextCursor::End);
    edit->setTextColor(QColor(255, 0, 0));
    edit->insertPlainText(QString::fromLatin1(readAllStandardError()));
    edit->setTextColor(QColor(0, 0, 0));
    edit->moveCursor(QTextCursor::End);
};

void ExternalProcess::readError() {
    edit->setTextColor(QColor(255, 0, 0));
    edit->append("An Error Occured! Error Code is: " + QString::number(error()));
    edit->setTextColor(QColor(0, 0, 0));
};


void ExternalProcess::terminateRun(QObject* obj) {
    kill();
    waitForFinished();
}
