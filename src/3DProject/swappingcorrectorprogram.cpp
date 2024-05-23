#include "swappingcorrectorprogram.h"

SwappingCorrectorProgram::SwappingCorrectorProgram(QWidget *parent) : ProgramWindow(parent) {}

SwappingCorrectorProgram::SwappingCorrectorProgram(QString& fileName, QWidget *parent) : ProgramWindow(fileName, parent) {}

void SwappingCorrectorProgram::loadFile(QString& fileName) {
    setData(fileName);
}
