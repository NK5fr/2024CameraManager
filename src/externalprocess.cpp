

#include "externalprocess.h"

ExternalProcess::ExternalProcess() : QProcess() {
    edit = new QTextEdit();
    edit->setFont(QFont("Courier", 9));
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOutput()));
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readError()));
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

void ExternalProcess::readError() {
    edit->append("An Error Occured! Error Code is: " + QString::number(error()));
};

