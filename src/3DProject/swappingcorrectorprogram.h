#ifndef SWAPPINGCORRECTORPROGRAM_H
#define SWAPPINGCORRECTORPROGRAM_H

#include "programwindow.h"
#include <QWidget>

/**
 * @brief The SwappingCorrectorProgram class
 * SwappingCorrectorProgram is a class which is a wrapper for the main class ProgramWindow. It allows the user to have a simple class
 * that acts as an interface to create and use ProgramWindow.
 */

class SwappingCorrectorProgram : public ProgramWindow
{
public:

    /**
     * @brief SwappingCorrectorProgram
     * Constructs a SwappingCorrectorProgram object with an empty Data object.
     * @param parent
     *          the parent widget of the SwappingCorrectorProgram widget.
     */
    SwappingCorrectorProgram(QWidget *parent = Q_NULLPTR);

    /**
     * @brief SwappingCorrectorProgram
     * Constructs a SwappingCorrectorProgram object whose data is taken from the file given in parameter.
     * @param fileName
     *          the pathname of the file that contains the data of the program.
     * @param parent
     *          the parent widget of the SwappingCorrectorProgram widget.
     */
    SwappingCorrectorProgram(QString& fileName, QWidget *parent = Q_NULLPTR);

    /**
     * @brief loadFile
     * Calls the setData() ProgramWindow method to load data in the program
     * @param fileName
     *          the pathname of the file containing the data to be loaded.
     */
    void loadFile(QString& fileName);
};

#endif // SWAPPINGCORRECTORPROGRAM_H
