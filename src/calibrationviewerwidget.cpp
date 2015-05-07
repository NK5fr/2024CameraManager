#include "calibrationviewerwidget.h"

#include <QFile>
#include <QPushButton>
#include <QTextStream>
#include <QMenu>
#include <QScrollBar>
#include <QLineEdit>

#include <iostream>
#include <string>

using namespace std;

/** CONSTRUCTOR AND ASSIMILATED **/
/* Constructor */
CalibrationViewerWidget::CalibrationViewerWidget(QString path, QString name) {
    fullPath = path + "/" + name;
    calibrationFile = new CalibrationFile(path);
    // Creating QTextEdit, which need to be known to save file latter if asked
    fileContain = new CalibrationEdit(this);
    format = fileContain->textCursor().charFormat();

    // Initializing the QHash for the display combination
    initializingCombinaisons();

    showTextView();
    // Modify the combination lines according to the Combination field
    displayCombinations();

    connect(fileContain, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(onRightClic()));
    setWindowTitle(name);
}

// Initializing combinations
void CalibrationViewerWidget::initializingCombinaisons(){
    // Bool to continue reading when started
    bool reading=false;
    // Opening file 
    QFile myFile(fullPath);
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)){ return; }
    // Reading the first line to know how cameras the are
    QString line = myFile.readLine();
    camerasNumber = (line.split(" ").size()-1)*3;
    while(!myFile.atEnd()){
        line = myFile.readLine();
        // Trying to find a group camera
        if(!combinationsInitialised || reading){

            QStringList headers = line.split(":");
            // If header size == 5 and header has 3 cells separated with '_'
            // Way of founding cameras combinations
            if(headers[0].size()==5 && headers[0].split("_").size()==3){
                Calibration cal;
                // Testing if the combination is 'NO CONVERGENCE...'
                if(headers[1].contains("NO CONVERGENCE"))
                    cal=Failed;
                else
                    cal=Normal;
                combinations.insert(headers[0], cal);
                // States changed 
                combinationsInitialised=true;
                reading=true;
            } else {
                reading=false;
            }
        }
    }
    // Setting the reversed 'Failed' lines as 'Failed' too 
    QList<QString> keys = combinations.keys();
    for(int i=0;i<combinations.size();i++){
        if(combinations.value(keys.at(i))==Failed)
            combinations.insert(getReversedKey(keys.at(i)), Failed);
    }
    myFile.close();
}

///** VIEWS AND ASSIMILATED **/
///* Show file as text view */
void CalibrationViewerWidget::showTextView(){

    /* Opening file */
    QFile myFile(fullPath);
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)){ return; }

    /* Adding lines to the QTextEdit component */
    fileContain->setCurrentCharFormat(format);
    fileContain->setText(myFile.readAll());

    myFile.close();

    setWidget(fileContain);
}


/* Show file as table view */
void CalibrationViewerWidget::showTableView(int lig){
    /* Spliting the QTextEdit contain with the arg '\n' */
    QStringList line = fileContain->toPlainText().split("\n");

    QGridLayout *layout = new QGridLayout();
    QGridLayout *leftLayout = new QGridLayout();

    int camerasNumber=0;
    int columnNumber;
    int lineNumber=0;
    bool begin=false;
    int i=lig;
    /* While reading file */
    while(i<line.size() && !line.at(i).contains("========")){
        /* If line contains Camno, which mean it's the
         * beginning of the part which must be read and analysed */
        if(line.at(i).contains("Camno")){
            /* Split line to add QLabel whith the reference camera n° */
            QStringList list = line.at(i).split(".");
            leftLayout->addWidget(new QLabel(list.at(0)), camerasNumber*4, 0, 1, 1);
            leftLayout->addWidget(new QLabel(QString::fromUtf8("Serial n\u00B0 ")+list.at(2)), camerasNumber*4+1, 0, 1, 1);
            /* Adding 3 \n for the camera label to be aligned with the datas on the right */
            leftLayout->addWidget(new QLabel("\n\n\n"), camerasNumber*4+2, 0, 1, 1);
            /* One camera more, begin to start read options, and go (back) to column 0 */
            camerasNumber++;
            begin=true;
            columnNumber=0;
        } else if(begin==true){
            /* Reading options. Spliting and removing spaces and tabs */
            QStringList list = line[i].split(" ");
            /* Cleaning the strings */
            for(int i=0;i<list.size();i++){
                QString tmp = list.at(i);
                tmp.replace(" ","");
                tmp.replace("\n","");
                list.replace(i, tmp);
            }
            int i=0;
            /* Reading all parameters from the line */
            while(i<list.size()){
                QString tmp = list.at(i);
                /* There are many empty cells, because there were spaces which were removed */
                if(!tmp.isEmpty()){
                    /* Have to watch the next cell, because there is a space between an
                                                   * option name and the 'std.dev.' string */
                    if(list.at(i+1).compare("std.dev.:")==0){
                        tmp = tmp + " " + list.at(i+1);
                        i++;
                    }
                    /* Adding QLabel for the option name at the right position */
                    layout->addWidget(new QLabel(tmp), camerasNumber*4+lineNumber, columnNumber, 1, 1);
                    /* Adding QLineEdit to see the option value (no editable) */
                    QLineEdit *lineEdit = new QLineEdit(list.at(i+1));
                    lineEdit->setReadOnly(true);
                    layout->addWidget(lineEdit, camerasNumber*4+lineNumber, columnNumber+1, 1, 1);
                    /* Update line number for the graphic position of options */
                    lineNumber++;
                    i++;

                }
                /* i : if empty cell, i=i+1,
                 *     if normal option, i=i+2,
                 *     if option with std.dev., i=i+3 */
                i++;
            }
            /* Updating line number and column umber for the graphic placement */
            lineNumber=0;
            columnNumber=columnNumber+2;
        }
        i++;
    }

    /* Left widget and scroll area */
    leftLayout->setVerticalSpacing(12);
    QWidget *wi = new QWidget();
    wi->setLayout(leftLayout);

    QScrollArea *leftArea = new QScrollArea();
    leftArea->setWidget(wi);
    leftArea->setMaximumWidth(150);

    /* Right widget and scroll area */
    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(layout);

    QScrollArea *rightArea = new QScrollArea();
    rightArea->setWidget(rightWidget);

    /* Main layout and main widget */
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(leftArea);
    mainLayout->addWidget(rightArea);

    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(mainLayout);
    mainWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    /* Setting main widget, and connecting signal/slot */
    setWidget(mainWidget);

    connect(mainWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(onRightClic()));

    cout << "End of function" << endl;
}

/** RIGHT CLIC AND ASSIMILATED **/
/* Right clic on the main widget of the viewer */
void CalibrationViewerWidget::onRightClic(){
    /* Creating a menu with allowed actions */
    QMenu *menu = new QMenu();

    cout << "View : " << view << endl;
    if(view==0){
        menu->addAction("Go top");
        QAction *fileEdition = new QAction(tr("File edition"), menu);
        fileEdition->setCheckable(true);
        fileEdition->setChecked(textEditable);

        QAction *save = new QAction(tr("Save"), menu);
        save->setEnabled(textEditable);

        menu->addSeparator();
        menu->addAction(fileEdition);
        menu->addSeparator();
        menu->addAction(save);
    }
    if(view==1){
        menu->addAction("Change view");
    }

    menu->popup(cursor().pos());

    connect(menu, SIGNAL(triggered(QAction*)),
            this, SLOT(menuProjectAction_triggered(QAction*)));
}

/* Clic on a menu item */
void CalibrationViewerWidget::menuProjectAction_triggered(QAction* action){
    if(action->text()=="Go top"){
        fileContain->moveCursor(QTextCursor::Start);
    } else if(action->text()=="File edition"){
        textEditable=!textEditable;
        /* If textEditable, then NOT read only, and reverse... */
        fileContain->setReadOnly(!textEditable);

    } else if(action->text()=="Save"){
        /* Changing all state to show all */
        showUselessBool=!showUselessBool;
        showFailedBool=!showFailedBool;
        calculateShowUseless();
        calculateShowFailed();
        /* Show from file again */
        showTextView();
        /* Modify the combination lines according to the Combination field */
        displayCombinations();
        /* Change the first line, if needed */
        changeFirstLine();
        /* Saving the QTextEdit contain into the file at the fullPath path */
        QFile file(fullPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){ cout << "Open problems" << endl; }
        QTextStream out(&file);
        out << fileContain->toPlainText().toUtf8().constData();
    } else if(action->text()=="Change view"){
        cout << "Test" << endl;
        showTextView();
        displayCombinations();
        view=0;
        cout << "View inside : " << view << endl;
    }
}

/** LEFT CLIC AND ASSIMILATED **/
/* Left clic performed on CalibrationEdit field */
void CalibrationViewerWidget::clicOnCalibrationEdit(int lig){
    QMenu *menu = new QMenu();
    QStringList lines = fileContain->toPlainText().split("\n");
    QString line = lines.at(lig);

    /* If it is a path line containing a combination path
     * Then popup the menu with 'Change view' */
    if(line.contains("C:/") && line.contains("calibration") && line.contains(".dat")){
        menu->addAction("Change view");
        menu->popup(cursor().pos());

        connect(menu, SIGNAL(triggered(QAction*)),
                this, SLOT(menuCalibration(QAction*)));
        return;
    }

    QString key = fileContain->getKeyByLine(lig);

    /* If it is not a combination line */
    if(key.size()!=5 || key.split("_").size()!=3)
        return;

    QAction *showFailed = new QAction(tr("Show failed"), menu);
    showFailed->setEnabled(textEditable);
    showFailed->setCheckable(true);
    showFailed->setChecked(showFailedBool);
    QAction *showUseless = new QAction(tr("Show useless"), menu);
    showUseless->setEnabled(textEditable);
    showUseless->setCheckable(true);
    showUseless->setChecked(showUselessBool);

    menu->addAction("Go to file");
    /* If it is not a failed line */
    if(!lines[lig].contains("NO CONVERGENCE")){
        QAction *select = new QAction("Select", menu);
        select->setEnabled(!showFailedBool && textEditable);
        select->setCheckable(true);
        select->setChecked(combinations.value(key)==Selected);
        menu->addAction(select);
        menu->addSeparator();
    }
    menu->addAction(showFailed);
    menu->addAction(showUseless);
    menu->addSeparator();
    /* Creating the 4 actions to sort the lines */
    QAction *so = new QAction(tr("Sort by SO"), menu);
    so->setEnabled(!showFailedBool && textEditable);
    so->setCheckable(true);
    so->setChecked(columnSort==1);
    menu->addAction(so);

    QAction *mean = new QAction(tr("Sort by mean"), menu);
    mean->setEnabled(!showFailedBool && textEditable);
    mean->setCheckable(true);
    mean->setChecked(columnSort==3);
    menu->addAction(mean);

    QAction *max = new QAction(tr("Sort by max"), menu);
    max->setEnabled(!showFailedBool && textEditable);
    max->setCheckable(true);
    max->setChecked(columnSort==5);
    menu->addAction(max);

    QAction *noOfFrames = new QAction(tr("Sort by No of frames"), menu);
    noOfFrames->setEnabled(!showFailedBool && textEditable);
    noOfFrames->setCheckable(true);
    noOfFrames->setChecked(columnSort==10);
    menu->addAction(noOfFrames);

    if(camerasNumber==6){
        /* Creating the 5th action to sort. This one is for combo sorts
         * Only available with 6 cameras (2 combinations) */
        QAction *comboSortAction = new QAction(tr("Combo sort"), menu);
        comboSortAction->setEnabled(!showFailedBool && textEditable);
        comboSortAction->setCheckable(true);
        comboSortAction->setChecked(comboSort);
        menu->addSeparator();
        menu->addAction(comboSortAction);
    }


    menu->popup(cursor().pos());

    connect(menu, SIGNAL(triggered(QAction*)),
            this, SLOT(menuCalibration(QAction*)));
}
/* clic on a left menu item */
void CalibrationViewerWidget::menuCalibration(QAction *action){
    if(action->text()=="Go to file"){
        /* Getting the key to the appropriate line */
        QString key = fileContain->getKeyByLine(fileContain->textCursor().blockNumber());
        /* Setting  cursor at the beginning of the file */
        fileContain->moveCursor(QTextCursor::Start);

        int lig;
        QStringList lines = fileContain->toPlainText().split("\n");
        /* While it is not the right line, move 'Down' */
        do{
            fileContain->moveCursor(QTextCursor::Down);
            lig = fileContain->textCursor().blockNumber();

        } while(!lines.at(lig).contains("calibration_comb_"+key+".dat"));
        /* Moving the scroll bar for the cursor to be at the top of the widget */
        fileContain->verticalScrollBar()->setValue(fileContain->verticalScrollBar()->value()
                                                   + fileContain->cursorRect().top()
                                                   - fileContain->rect().top());
        /* Selecting the file line */
        fileContain->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    } else if(action->text()=="Select"){
        /* Perform the 'select' function */
        select();
        /* Calcul to see which are Useless now */
        calculUselessCombinations();
        /* Show from file again */
        showTextView();
        /* Modify the combination lines according to the Combination field */
        displayCombinations();
        sortByColumnId();
        /* Change the first line, if needed */
        changeFirstLine();

    } else if(action->text()=="Show failed"){
        showFailedBool=!showFailedBool;
        calculateShowFailed();
        /* Show from file */
        showTextView();
        /* Modify the combination lines according to the Combination field */
        displayCombinations();
        sortByColumnId();

    } else if(action->text()=="Show useless"){
        showUselessBool=!showUselessBool;
        calculateShowUseless();
        /* Show from file */
        showTextView();
        /* Modify the combination lines according to the Combination field */
        displayCombinations();
        sortByColumnId();
    } else if(action->text()=="Sort by SO"){
        columnSort=1;
        sortByColumnId();
        comboSort=false;
    } else if(action->text()=="Sort by mean"){
        columnSort=3;
        sortByColumnId();
        comboSort=false;
    } else if(action->text()=="Sort by max"){
        columnSort=5;
        sortByColumnId();
        comboSort=false;
    } else if(action->text()=="Sort by No of frames"){
        columnSort=10;
        sortByColumnId();
        comboSort=false;
    } else if (action->text()=="Combo sort"){
        comboSort=!comboSort;
        if(comboSort)
            sortCombo();
        else
            sortByColumnId();
    } else if(action->text()=="Change view"){
        /* With a left clic, it's always from text view to table view
         * with a right clic, it's always from table view to text view
         * So useless to keep in mind in which view we are */
        showTableView(fileContain->textCursor().blockNumber());
        view++;

    }
}
/* Set as 'Selected' the current line, and so the current combination */
void CalibrationViewerWidget::select(){
    /* If action is select, then mark the line as selected */
    /* Getting the line cursor, which is the line where was the clic */
    int lig = fileContain->textCursor().blockNumber();
    QString key=fileContain->getKeyByLine(lig);
    if(combinations.value(key)==Selected)
        combinations.insert(key, Normal);
    else
        combinations.insert(key, Selected);
}

/* Show the failed combinations according to the bool 'show' */
void CalibrationViewerWidget::calculateShowFailed(){
    QList<QString> keys = combinations.keys();
    for(int i=0;i<combinations.size();i++){
        /* If the value with the keys[i] is concerned */
        if(combinations.value(keys[i])==Failed || combinations.value(keys[i])==HideByFail){
            /* If the combination has to be displayed, or not */
            if(showFailedBool)
                combinations.insert(keys[i], Failed);
            else
                combinations.insert(keys[i], HideByFail);
        }
    }
}

/* Show the useless combinations according to the bool 'show' */
void CalibrationViewerWidget::calculateShowUseless(){
    QList<QString> keys = combinations.keys();
    for(int i=0;i<combinations.size();i++){
        /* If the value with the keys[i] is concerned */
        if(combinations.value(keys[i])==Useless || combinations.value(keys[i])==HideByCalcul){
            /* If the combination has to be displayed, or not */
            if(showUselessBool)
                combinations.insert(keys[i], Useless);
            else
                combinations.insert(keys[i], HideByCalcul);
        }
    }
}

/* Calcul the useless combinations according to the 'Selected' lines */
void CalibrationViewerWidget::calculUselessCombinations(){
    /* Checking if the useless combinations are hidden or not */
    Calibration newValue;
    if(showUselessBool)
        newValue = Useless;
    else
        newValue = HideByCalcul;
    QList<QString> allKeys = combinations.keys();
    /* Getting all the keys with the 'Selected' value */
    QList<QString> combinationsSelected = combinations.keys(Selected);

    /* If not keys selected, then put all 'HideByCalcul' or 'Useless' into 'Normal' */
    if(combinationsSelected.size()==0){
        for(int i=0;i<allKeys.size();i++){
            if(combinations.value(allKeys.at(i))==Useless
                    || combinations.value(allKeys.at(i))==HideByCalcul){
                combinations.insert(allKeys.at(i), Normal);
            }
        }
        /* End of function does not have to be performed */
        return;
    }
    /* Checking all the selected keys */
    for(int i=0;i<combinationsSelected.size();i++){
        /* Getting all the numbers for the selected key at the int 'i'*/
        QStringList numbersString = combinationsSelected[i].split("_");

        /* Checking all combinations for each key
         * This loop is to put Normal value to the combinations which are no longuer useless
         * Must do it BEFORE the other loop */
        for(int j=0;j<allKeys.size();j++){
            /* Checking each number */
            bool inside=false;
            for(int k=0;k<numbersString.size();k++){
                /* Checking if the number is into the key and if the value of the selected key is 'Normal' */
                if(allKeys.at(j).contains(numbersString.at(k)))
                    inside=true;
            }
            if(!inside && (combinations.value(allKeys.at(j))==Useless ||
                           combinations.value(allKeys.at(j))==HideByCalcul))
                combinations.insert(allKeys.at(j), Normal);
            inside=false;
        }

        /* Checking all combinations for each key
         * This one is to put Useless/HideByCalcul to the combinations which are useless
         * Must do it AFTER the other loop */
        for(int j=0;j<allKeys.size();j++){
            /* Checking each number */
            for(int k=0;k<numbersString.size();k++){
                /* Checking if the number is into the key and if the value of the selected key is 'Normal' */
                if(allKeys.at(j).contains(numbersString.at(k)) &&
                        combinations.value(allKeys.at(j))==Normal){
                    combinations.insert(allKeys.at(j), newValue);
                }
            }
        }
    }
}

/* Display combinations according to the 'Calibration' field */
void CalibrationViewerWidget::displayCombinations(){
    /* Saving cursor and file to put them again at the end */
    QTextCursor saveCursor = fileContain->textCursor();
    int saveScrollBarValue = fileContain->verticalScrollBar()->value();

    /* Moving the cursor at the beginning of the combination lines */
    moveCursorTopCombinationsLines();

    int line = fileContain->textCursor().blockNumber();
    QString key;
    while(isCombinationCamera(fileContain->getKeyByLine(line))){
        /* Getting the current key */
        key=fileContain->getKeyByLine(line);

        /* Format of the current text in the QTextEdit */
        QTextCharFormat formatTmp=format;
        QTextCursor cursor;
        /* If the line must not be shown */
        if(combinations.value(key)==HideByCalcul || combinations.value(key)==HideByFail){
            fileContain->moveCursor(QTextCursor::Down, QTextCursor::KeepAnchor);
            fileContain->textCursor().removeSelectedText();

        } else if(combinations.value(key)==Selected){
            /* Selecting text */
            fileContain->moveCursor(QTextCursor::Down, QTextCursor::KeepAnchor);
            /* Changing format of the selected text */
            formatTmp.setFontWeight(QFont::Bold);
            cursor = fileContain->textCursor();
            cursor.setCharFormat(formatTmp);
            fileContain->setTextCursor(cursor);
            cursor.clearSelection();
            fileContain->setTextCursor(cursor);

        } else if(combinations.value(key)==Failed){
            /* Selecting text */
            fileContain->moveCursor(QTextCursor::Down, QTextCursor::KeepAnchor);
            /* Changing format of the selected text */
            formatTmp.setFontUnderline(true);
            cursor = fileContain->textCursor();
            cursor.setCharFormat(formatTmp);
            fileContain->setTextCursor(cursor);
            cursor.clearSelection();
            fileContain->setTextCursor(cursor);

        } else if(combinations.value(key)==Useless){
            /* Selecting text */
            fileContain->moveCursor(QTextCursor::Down, QTextCursor::KeepAnchor);
            /* Changing format of the selected text */
            formatTmp.setFontWeight(QFont::Light);
            cursor = fileContain->textCursor();
            cursor.setCharFormat(formatTmp);
            fileContain->setTextCursor(cursor);
            cursor.clearSelection();
            fileContain->setTextCursor(cursor);

        } else {
            /* Moving down */
            fileContain->moveCursor(QTextCursor::Down);
        }
        /* Getting the new line */
        line = fileContain->textCursor().blockNumber();
    }
    /* Put the previous value */
    fileContain->setTextCursor(saveCursor);
    fileContain->verticalScrollBar()->setValue(saveScrollBarValue);
}
/* Testing and returning if the QString key is a right combination camera */
bool CalibrationViewerWidget::isCombinationCamera(QString key){
    return key.size()==5 && key.split("_").size()==3;
}
bool CalibrationViewerWidget::isLineCombinationCamera(int line){
    QStringList lines = fileContain->toPlainText().split("\n");
    return isCombinationCamera(lines.at(line).split(":").at(0));
}

/* Change the first line of the file, if needed */
bool CalibrationViewerWidget::changeFirstLine(){
    QList<QString> allKeys = combinations.keys();
    /* Taking all the 'Selected' keys */
    QList<QString> selectedCombinations;
    for(int i=0;i<allKeys.size();i++){
        if(combinations.value(allKeys.at(i))==Selected)
            selectedCombinations.insert(selectedCombinations.size(), allKeys.at(i));
    }
    /* Watching if the keys number is the number requested */
    if(selectedCombinations.size()==camerasNumber/3){
        /* Remove the first line */
        fileContain->moveCursor(QTextCursor::Start);
        fileContain->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        fileContain->textCursor().insertText("Groups:");
        for(int i=0;i<selectedCombinations.size();i++){
            fileContain->textCursor().insertText(" "+selectedCombinations.at(i));
        }
        return true;
    }
    return false;
}


void CalibrationViewerWidget::sortByColumnId(){
    if(columnSort!=1 && columnSort!=3 && columnSort!=5 && columnSort!=10)
        return;
    /* Saving cursor and file to put them again at the end */
    QTextCursor saveCursor = fileContain->textCursor();
    int saveScrollBarValue = fileContain->verticalScrollBar()->value();

    int lig;
    /* Moving the cursor at the beginning of the combination lines */
    moveCursorTopCombinationsLines();
    for(int i=0;i<combinations.size();i++){
        QStringList lines = fileContain->toPlainText().split("\n");

        /*for(int i=0;i<combinations.size();i++){*/
        for(int j=0;j<combinations.size()-1;j++){
            lig = fileContain->textCursor().blockNumber();
            /* Getting the StringList of the two lines which will be compared */
            QStringList top = lines.at(lig).split(" ");
            QStringList bottom = lines.at(lig+1).split(" ");
            /* If it is comparable lines (not 'Failed') */
            if(combinations.value(top.at(0).split(":")[0])!=Failed &&
                    combinations.value(bottom.at(0).split(":")[0])!=Failed){
                /* Getting and comparing the field at columnSort
                 * Removing the last char to avoid ',' '.' ' ' at the end... */
                QString first = top.at(columnSort).mid(0, top.at(columnSort).size()-1);
                QString second = bottom.at(columnSort).mid(0, bottom.at(columnSort).size()-1);
                if(first.toDouble()>second.toDouble()){
                    inverseFieldLines();
                }
            }
            /* Moving down to check next lines */
            fileContain->moveCursor(QTextCursor::Down);
        }
        /* Moving at the top to do it again */
        moveCursorTopCombinationsLines();
    }

    /* Put the previous value */
    fileContain->setTextCursor(saveCursor);
    fileContain->verticalScrollBar()->setValue(saveScrollBarValue);
}

void CalibrationViewerWidget::sortCombo(){
    QList<QString> keys = combinations.keys();
    QHash< QString, double > comboCalibration = QHash< QString, double >();
    bool insert=false;
    /* Inserting combo summ values into a QHash */
    for(int i=0;i<combinations.size();i++){
        if(combinations.value(keys.at(i))!=Failed && combinations.value(keys.at(i))!=HideByFail){
            QString key = keys.at(i);
            QList<QString> newKeys = comboCalibration.keys();
            QString reversedKey = getReversedKey(key);
            for(int j=0;j<comboCalibration.size();j++){
                /* If the reverser is already in the QHash */
                if(reversedKey==newKeys.at(j)){
                    /* Adding the value to the previous one */
                    comboCalibration.insert(reversedKey,comboCalibration.value(reversedKey)+getParameterByKey(key));
                    insert=true;
                    break;
                }
            }
            /* Insert the new value */
            if(!insert){

                comboCalibration.insert(key, getParameterByKey(key));
            }
        }
        insert=false;
    }

    QList<QString> comboKeys = comboCalibration.keys();
    QString keyAssociated;
    double value;
    QHash<QString, int> orderedHash;
    int i=0;
    while(comboCalibration.size()!=0){
        comboKeys = comboCalibration.keys();
        keyAssociated=comboKeys.at(0);
        value=comboCalibration.value(keyAssociated);
        for(int j=0;j<comboCalibration.size();j++){
            if(value>comboCalibration.value(comboKeys.at(j))){
                value=comboCalibration.value(comboKeys.at(j));
                keyAssociated=comboKeys.at(j);
            }
        }
        orderedHash.insert(keyAssociated, i);
        orderedHash.insert(getReversedKey(keyAssociated), i);
        comboCalibration.remove(keyAssociated);
        comboCalibration.remove(getReversedKey(keyAssociated));
        i++;
    }
    executeSortChanges(orderedHash);

}

void CalibrationViewerWidget::executeSortChanges(QHash<QString, int> order){
    if(columnSort!=1 && columnSort!=3 && columnSort!=5 && columnSort!=10)
        return;
    /* Saving cursor and file to put them again at the end */
    QTextCursor saveCursor = fileContain->textCursor();
    int saveScrollBarValue = fileContain->verticalScrollBar()->value();

    int lig;
    /* Moving the cursor at the beginning of the combination lines */
    moveCursorTopCombinationsLines();
    for(int i=0;i<combinations.size();i++){
        QStringList lines = fileContain->toPlainText().split("\n");

        /*for(int i=0;i<combinations.size();i++){*/
        for(int j=0;j<combinations.size()-1;j++){
            lig = fileContain->textCursor().blockNumber();
            /* Getting the StringList of the two lines which will be compared */
            QStringList top = lines.at(lig).split(":");
            QStringList bottom = lines.at(lig+1).split(":");
            /* If it is comparable lines (not 'Failed') */
            if(combinations.value(top[0])!=Failed &&
                    combinations.value(bottom[0])!=Failed){
                if(order.value(top[0])>order.value(bottom[0])){
                    inverseFieldLines();
                }
            }
            /* Moving down to check next lines */
            fileContain->moveCursor(QTextCursor::Down);
        }
        /* Moving at the top to do it again */
        moveCursorTopCombinationsLines();
    }

    /* Put the previous value */
    fileContain->setTextCursor(saveCursor);
    fileContain->verticalScrollBar()->setValue(saveScrollBarValue);
}

QString CalibrationViewerWidget::getReversedKey(QString key){
    vector<bool> present;
    present.resize(6);
    for(int i=0;i<present.size();i++)
        present[i]=false;

    QStringList numbers = key.split("_");
    for(int i=0;i<numbers.size();i++)
        present[numbers.at(i).toInt()]=true;
    QString reversedKey = "";
    for(int i=0;i<present.size();i++){
        if(!present[i]){
            reversedKey=reversedKey+QString("%1").arg(i);
            if(reversedKey.size()<5){
                reversedKey=reversedKey+"_";
            }
        }
    }
    return reversedKey;
}

double CalibrationViewerWidget::getParameterByKey(QString key){
    QStringList lines = fileContain->toPlainText().split("\n");
    moveCursorTopCombinationsLines();
    QStringList currentLine;
    int line=fileContain->textCursor().blockNumber()-1;
    do{
        line++;
        currentLine = lines.at(line).split(":");
    } while(currentLine[0]!=key);
    QString argument = lines.at(line).split(" ").at(columnSort);
    return argument.mid(0, argument.size()-1).toDouble();
}

void CalibrationViewerWidget::inverseFieldLines(){
    /* Saving and removing the first line */
    fileContain->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString topToBottom = fileContain->textCursor().selectedText();
    fileContain->textCursor().removeSelectedText();

    fileContain->moveCursor(QTextCursor::StartOfLine);
    fileContain->moveCursor(QTextCursor::Down);

    /* Saving, removing the second line, and inserting the first line */
    fileContain->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString bottomToTop = fileContain->textCursor().selectedText();
    fileContain->textCursor().removeSelectedText();
    fileContain->textCursor().insertText(topToBottom);

    fileContain->moveCursor(QTextCursor::StartOfLine);
    fileContain->moveCursor(QTextCursor::Up);

    /* Inserting the second line */
    fileContain->textCursor().insertText(bottomToTop);
    fileContain->moveCursor(QTextCursor::StartOfLine);
}

void CalibrationViewerWidget::moveCursorTopCombinationsLines(){
    QStringList lines = fileContain->toPlainText().split("\n");
    fileContain->moveCursor(QTextCursor::Start);
    int lig;
    /* While it is not the right line, move 'Down' */
    do{
        fileContain->moveCursor(QTextCursor::Down);
        lig = fileContain->textCursor().blockNumber();
    } while(!lines.at(lig).contains("="));
    /* Move one line done to be at the beginning of the combination lines */
    fileContain->moveCursor(QTextCursor::Down);
}


