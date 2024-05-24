#include "configfileviewerwidget.h"
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMenu>
#include <QtGui/QAction>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

#include <string.h>
#include "configfilereader.h"

#include <QtWidgets/QFileDialog>


#include <iostream>
#include <qmessagebox.h>
#include <qprocess.h>

/* Constructor */
ConfigFileViewerWidget::ConfigFileViewerWidget(QString filePath) : textEditable(true), view(0){
  setWindowTitle("Configuration File");

  loadAllParameters = false;
  this->path = filePath;
  if (path.contains("option")) isTextViewOnly = false; //use wizard view also
  else isTextViewOnly = true;

  //tabs initialization
  tabs = new QTabWidget();
  /* Creating QTextEdit, which need to be known to save file latter if asked */
  createTextEditor();
  /* And creating the wizard widget, that will allow to change principal values*/
  if (!isTextViewOnly) createWizard();//only for option files

  setWidget(tabs);

  showWizardView();
  //TTODO: change this to something more relative, like the size of all widgets or else. It's for compability with smaller screens.
  this->resize(575, 375);
  fileContain->setFont(QFont("Courier", 9));
  fileContain->setReadOnly(!textEditable);

  //TTODO: Check if this really works correctly.
  //Explanation: By calling changeView, it may change twice the current tab, reverting to the old state. It didn't do in the test, but for code safety, it would be best to change this
   connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeView()));
}


/* Right click on the main widget of the viewer */
void ConfigFileViewerWidget::onRightClic(){
  /* Creating a menu with allowed actions */
  QMenu *menu = new QMenu();

  //TTODO: QTextEdit specific actions (undo/redo)

  //General actions
  fileEdition = new QAction(tr("File edition"), menu);
  fileEdition->setCheckable(true);
  fileEdition->setChecked(textEditable);

  save = new QAction(tr("Save"), menu);
  save->setEnabled(textEditable);

  launchTrackPoint = new QAction(tr("Launch TrackPoint"), menu);


  if (!isTextViewOnly){
    menu->addAction("Change view");
    menu->addSeparator();
  }
  menu->addAction(fileEdition);
  if (!isTextViewOnly){
    if (view == 0) {
        if (loadAllParameters == false) {
            menu->addAction("Display all parameters");
        }
        else {
            menu->addAction("Display important parameters only");
        }
    }
    menu->addSeparator();
  }
  menu->addAction(save);

  menu->addAction(launchTrackPoint);

  menu->popup(cursor().pos());

  connect(menu, SIGNAL(triggered(QAction*)),
    this, SLOT(menuProjectAction_triggered(QAction*)));
}


/* Click on a menu item */
void ConfigFileViewerWidget::menuProjectAction_triggered(QAction* action){
  if (action->text() == "Change view"){
    changeView();
  }
  else if (action->text() == "File edition"){
    /* If edition was allowed, remove the authorisation */
    if (!action->isChecked()){
      preventFileEditing();
      return;
    }
    /* Displaying a menu to ask confirmation about allowing files edition */
    action->setChecked(false);
    frame = new QFrame();
    QGridLayout *layout = new QGridLayout();
    QPushButton *yes = new QPushButton("Yes");
    QPushButton *no = new QPushButton("No");
    layout->addWidget(new QLabel("You will allow files editing. Are you sure ?"), 0, 0, 1, 2);
    layout->addWidget(yes, 1, 1, 1, 1);
    layout->addWidget(no, 1, 0, 1, 1);

    frame->setLayout(layout);
    frame->setWindowTitle("Files edition ?");
    frame->setGeometry(300, 300, frame->minimumWidth(), frame->minimumHeight());
    frame->show();

    connect(yes, SIGNAL(clicked()),
      this, SLOT(allowFileEditing()));
    connect(no, SIGNAL(clicked()),
      frame, SLOT(close()));

  }
  else if (action->text() == "Display all parameters") {
    loadAllParameters = true;
    resetWizard();
    changeView();
  }
  else if (action->text() == "Display important parameters only") {
    loadAllParameters = false;
    resetWizard();
    changeView();
  }
  else if (action->text() == "Save"){
    /* Saving the QTextEdit contain into the file at the fullPath path */
    saveFile();
  }

  else if (action->text() == "Launch TrackPoint"){
      /* Launching the TrackPoint.exe file */
      launchTrackPointFunc();
  }
}

/*Save the file with the string in the QTextEdit*/
void ConfigFileViewerWidget::saveFile() {
    qInfo() << "saved";
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){ return; }
    QTextStream out(&file);
    out << fileContain->toPlainText().toUtf8().constData();
}

/* Clic on the button asking confirmation about editing.
 * Editing is now allowed */
void ConfigFileViewerWidget::allowFileEditing(){
  textEditable = true;
  fileContain->setReadOnly(false);
  frame->close();

  for (int i = 0; i < wizardComboBoxes->size(); i++) {
    wizardComboBoxes->at(i)->setEnabled(true);
  }
  for (int i = 0; i < wizardSpinBoxes->size(); i++) {
    wizardSpinBoxes->at(i)->setEnabled(true);
  }
  for (int i = 0; i < wizardPathEditBoxes->size(); i++) {
    wizardPathEditBoxes->at(i)->setEnabled(true);
  }
}

void ConfigFileViewerWidget::preventFileEditing() {
  textEditable = false;
  fileContain->setReadOnly(true);
  for (int i = 0; i < wizardComboBoxes->size(); i++) {
    wizardComboBoxes->at(i)->setEnabled(false);
  }
  for (int i = 0; i < wizardSpinBoxes->size(); i++) {
    wizardSpinBoxes->at(i)->setEnabled(false);
  }
  for (int i = 0; i < wizardPathEditBoxes->size(); i++) {
    wizardPathEditBoxes->at(i)->setEnabled(false);
  }
}

/*Switch view between text view and wizard view*/
void ConfigFileViewerWidget::changeView() {
  /* Checking view ID and file type to display the right view of the file */
  //view = 1 -> text view
  if (view == 1) {
    showWizardView();
    view = 0;
    //view = 0 -> wizard view
  }
  else {
    showTextView();
    view = 1;
  }
}

void ConfigFileViewerWidget::launchTrackPointFunc()
{
    QFile file = QFile(execFilePath);
    if (file.exists()) {
        QProcess::startDetached(execFilePath);
    } else {
        QMessageBox::information(this, "Not a valid exe file!", execFilePath.append("\ndoes not exist!"));
    }
}

void ConfigFileViewerWidget::changeExecFunc()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Executable"), executablePath, tr("Executable (*.exe)"));
    if (!fileName.isNull() && !fileName.isEmpty()) {
        this->execFilePath = fileName;
        this->currentExecPathLabel->setText(fileName);
    }
}

/* Show file as a text view */
void ConfigFileViewerWidget::showTextView(){
  tabs->setCurrentWidget(fileContain);

}

/* Show file as a wizard view*/
void ConfigFileViewerWidget::showWizardView() {
  //TTODO add a popup window when you switch from raw text view to the wizard view, asking to confirm to save the file or not.
  tabs->setCurrentWidget(wizardScrollArea);
}

/*Reset the wizard. The wizard needs to be created first!*/
void ConfigFileViewerWidget::resetWizard() {
  delete wizard;
  delete wizardWrapper;
  delete wizardScrollArea;
  delete wizardComboBoxes;
  delete wizardComboBoxesRealValues;
  delete wizardComboBoxesParameterPosition;
  delete wizardSpinBoxes;
  delete wizardSpinBoxesParameterPosition;
  delete wizardPathEditBoxes;
  delete wizardPathEditBoxesParameterPosition;
  delete wizardCategories;
  delete wizardCategoriesPosition;
  delete wizardLineEdit;
  delete wizardLineEditParameterPosition;
  createWizard();
}

void ConfigFileViewerWidget::   createWizard() {
  //From the smallest widget to the biggest one: wizard; wizardWrapper; WizardScrollArea; WizardScrollAreaWrapper;
  wizard = new QFormLayout();
  wizardWrapper = new QWidget();
  wizardWrapper->setContextMenuPolicy(Qt::CustomContextMenu);
  wizardWrapper->setLayout(wizard);
  //The size constraints are here to give space to the wizardWrapper, otherwise it gets "crushed" by the scrollArea.
  wizard->setSizeConstraint(QLayout::SetMinAndMaxSize);
  wizardScrollArea = new QScrollArea();
  wizardScrollArea->setWidget(wizardWrapper);
  //Initialization of all lists
  wizardComboBoxes = new QList<QComboBox*>();
  wizardComboBoxesRealValues = new QList<QList<QString>*>();
  wizardComboBoxesParameterPosition = new QList<int>();

  wizardSpinBoxes = new QList<QSpinBox*>();
  wizardSpinBoxesParameterPosition = new QList<int>();

  wizardPathEditBoxes = new QList<pathEditBox*>();
  wizardPathEditBoxesParameterPosition = new QList<int>();

  wizardCategories = new QList<QLabel*>();
  wizardCategoriesPosition = new QList<int>();

  wizardLineEdit = new QList<QLineEdit*>();
  wizardLineEditParameterPosition = new QList<int>();

  QString strParam;
  /*Opening the files: the config file, and the list of parameters "parameterList.txt"*/
  ConfigFileReader *cfgreader = new ConfigFileReader(path);
  QFile parameterList(QDir::currentPath() + "/" + CALIBRATION_PARAMETERLIST_FILE);
  if (!parameterList.open(QIODevice::ReadOnly | QIODevice::Text)) return;
  /*Catching parameters and putting them into the lists*/
  QString line;
  QStringList splitLine;

  //The parameter number in parameterList.txt
  int parameterNumber;

  //Used when we load all parameters
  int currentParameterNumber = 1;
  //Used to check if operations are done correctly.
  bool ok = true;
  QTextStream pListReader(&parameterList);
  //if we add all parameters, we will need to have the cfgreader set at the first parameter
  cfgreader->readString(1);
  //Here is the main algorithm to add automatically all the parameters. We read all the parameters in parameterList and add them one by one, depending on their type.
  //Each type of parameter has it's own mini-algorithm in a if condition.
  while (!pListReader.atEnd()) {
    line = pListReader.readLine();
    //if the line is not a comment or empty, we split the line with the delimiter "|" and determine what type it is
    if (!line.contains("!") && !line.isEmpty()) {
      splitLine = line.split("|", Qt::SkipEmptyParts);
      //read the first element, which shall correspond to the number of the parameter
      parameterNumber = splitLine.at(0).toInt(&ok);
      //if we successfully read the parameter, we search the type of parameter
      if (ok) {
        //if we decide to load all parameters, we continue to add parameters until parameterNumber is at the right parameter. Check the end of the algorithm to see the rest of the loading after we read all parameterList.txt
        if (loadAllParameters == true) {
          QStringList *parameterAndName;
          //We load the parameters until we reach the correct parameter number, so that every parameters are in the correct order.
          while (currentParameterNumber < parameterNumber) {
            parameterAndName = cfgreader->readNextParameterAndComment();
            if (parameterAndName != NULL) {
              //we remove the "!" from the comment, and here is our name!
              QString name = parameterAndName->at(0);
              name.remove(0, 1);
              QLineEdit *lineEdit = wizardAddLineEdit(name, parameterAndName->at(1));
              wizardLineEdit->append(lineEdit);
              wizardLineEditParameterPosition->append(currentParameterNumber);
              currentParameterNumber++;
            }
          }
        }

        //if the parameter is a spinbox//
        if (splitLine.at(1).compare("int") == 0) {
          currentParameterNumber++;
          QSpinBox *spinBox;
          spinBox = wizardAddSpinBox(splitLine.at(2), cfgreader->readInt(parameterNumber), -2500, 2500);
          if (splitLine.size() >= 4) {
            int min = splitLine.at(3).toInt(&ok);
            if (ok) spinBox->setMinimum(min);
            if (splitLine.size() >= 5) {
              int max = splitLine.at(4).toInt(&ok);
              if (ok) spinBox->setMaximum(max);
            }
          }
          wizardSpinBoxes->append(spinBox);
          wizardSpinBoxesParameterPosition->append(parameterNumber);

          //if the parameter is a combobox with strings//
        }
        else if (splitLine.at(1).compare("cmbBox") == 0) {
          currentParameterNumber++;
          QList<QString> *possibleValues = new QList<QString>();
          QList<QString> *realValues = new QList<QString>();
          for (int i = 3; i < splitLine.size(); i = i + 2) {
            possibleValues->append(splitLine.at(i));
            realValues->append(splitLine.at(i + 1));
          }
          QComboBox *cmbBox = wizardAddComboBox(splitLine.at(2), possibleValues);
          QString pickedValue = cfgreader->readString(parameterNumber);
          for (int i = 0; i < realValues->size(); i++) {
            if (pickedValue.compare(realValues->at(i)) == 0) cmbBox->setCurrentIndex(i);
          }
          wizardComboBoxesRealValues->append(realValues);
          wizardComboBoxes->append(cmbBox);
          wizardComboBoxesParameterPosition->append(parameterNumber);
          //if the parameter is a path//
        }
        else if (splitLine.at(1).compare("path") == 0) {
          currentParameterNumber++;
          pathEditBox *pEditBox;
          if (splitLine.at(3).compare("folder") == 0) {
            pEditBox = wizardAddPathEditBox(splitLine.at(2), cfgreader->readString(parameterNumber), true);
          }
          else {
            pEditBox = wizardAddPathEditBox(splitLine.at(2), cfgreader->readString(parameterNumber), false);
          }
          wizardPathEditBoxes->append(pEditBox);
          wizardPathEditBoxesParameterPosition->append(parameterNumber);
          //if the "parameter" is a category//
        }
        else if (splitLine.at(1).compare("category") == 0) {
          QLabel *categoryLabel = wizardAddCategory(splitLine.at(2));
          wizardCategories->append(categoryLabel);
          //The position of the category is not important, but I wrote it to keep it similar compared to the other parameters.
          wizardCategoriesPosition->append(parameterNumber);
        }
      }
    }
  }
  //When we reached the end of file, there may be still parameters to load when "all parameters" is ticked. This loop solves the problem.
  if (loadAllParameters == true) {
    cfgreader->readString(parameterNumber - 1);
    //Until parameterAndName isn't null, meaning that the end of the file has been reached.
    for (QStringList *parameterAndName = cfgreader->readNextParameterAndComment(); parameterAndName != NULL; parameterAndName = cfgreader->readNextParameterAndComment())  {
      parameterAndName = cfgreader->readNextParameterAndComment();
      if (parameterAndName != NULL && !parameterAndName->at(0).contains("Approximate location for the frame markers")) {
        //we remove the "!" from the comment, and here is our name!
        QString name = parameterAndName->at(0);
        name.remove(0, 1);
        QLineEdit *lineEdit = wizardAddLineEdit(name, parameterAndName->at(1));
        wizardLineEdit->append(lineEdit);
        wizardLineEditParameterPosition->append(currentParameterNumber);
      }
    }
  }
  /*Finalizing the wizard options, like the save button*/
  QLabel *emptyLabel = new QLabel("");
  QPushButton *saveButton = new QPushButton("save");
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveFile()));
  wizard->addRow(saveButton, emptyLabel);

  /*TrackPoint exec button*/
  QPushButton *launchTrackPointButton = new QPushButton("  launch trackpoint  ");
  connect(launchTrackPointButton, SIGNAL(clicked()), this, SLOT(launchTrackPointFunc()));
  QPushButton *changeExecButton = new QPushButton("change path to executable");
  connect(changeExecButton, SIGNAL(clicked()), this, SLOT(changeExecFunc()));
  wizard->addRow(launchTrackPointButton, changeExecButton);
  QLabel *test = new QLabel("current path to executable");
  wizard->addRow(test, this->currentExecPathLabel);


  connect(wizardWrapper, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(onRightClic()));
  tabs->addTab(wizardScrollArea, "wizard");
  delete cfgreader;
}

void ConfigFileViewerWidget::createTextEditor() {
  fileContain = new QTextEdit();
  fileContain->setContextMenuPolicy(Qt::CustomContextMenu);
  /* Opening file */
  QFile myFile(path);
  //QFile myFile("/home/tomash/kode/fou/_TrackPoint_SEPT_2015/TrackPoint_SEPT_2015/input/options.txt");
  if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text))	{ return; }
  QString line;
  /* Adding lines to the QTextEdit component */
  while (!myFile.atEnd()){
    line = myFile.readLine();
    fileContain->setText(fileContain->toPlainText() + line);
  }
  myFile.close();

  tabs->addTab(fileContain, "raw text");
  connect(fileContain, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(onRightClic()));
}

//Save the wizard parameters
void ConfigFileViewerWidget::WizardSave() {
  QString FileContent = fileContain->toPlainText();
  QStringList SplitFileContent = FileContent.split("\n");
  int n = 1;
  QString line;
  //TTODO: clean the code to make it more flexible, with a list of all parameters
  //Read and replace
  for (int i = 1; i < SplitFileContent.size(); i++) {
    line = SplitFileContent.at(i);
    //if content[i] is not a comment
    if (!line.contains("!")) {
      //Check the parameter for each position
      //TTODO increase the performance by reducing the number of imbricated "for".
      for (int j = 0; j < wizardSpinBoxesParameterPosition->size(); j++) {
        if (wizardSpinBoxesParameterPosition->at(j) == n) line = QString::number(wizardSpinBoxes->at(j)->value());
      }
      for (int j = 0; j < wizardComboBoxesParameterPosition->size(); j++) {
        if (wizardComboBoxesParameterPosition->at(j) == n) line = wizardComboBoxesRealValues->at(j)->at(wizardComboBoxes->at(j)->currentIndex());
      }
      for (int j = 0; j < wizardPathEditBoxesParameterPosition->size(); j++) {
        if (wizardPathEditBoxesParameterPosition->at(j) == n) line = wizardPathEditBoxes->at(j)->getValue();
      }
      for (int j = 0; j < wizardLineEdit->size(); j++) {
        if (wizardLineEditParameterPosition->at(j) == n) line = wizardLineEdit->at(j)->text();
      }
      SplitFileContent.replace(i, line);
      n++;
    }
  }
  fileContain->setText(SplitFileContent.join("\n"));
}

//Append at the end of the wizard a new category with the given name. It is used to determine in what category you are.
QLabel * ConfigFileViewerWidget::wizardAddCategory(QString name) {
  QLabel *label = new QLabel(name);
  label->setAlignment(Qt::AlignCenter);
  QFont Font = label->font();
  Font.setBold(true);
  Font.setPointSize(Font.pointSize() + 1);
  label->setStyleSheet("QLabel { color : blue; }");
  label->setFont(Font);
  label->setMargin(5);
  wizard->addRow(label);
  return label;
}

//Append at the end of the wizard an Integer spinbox with the given name, starting value, min value and max value. It automatically connect it to wizardSave().
QSpinBox * ConfigFileViewerWidget::wizardAddSpinBox(QString name, int initialValue, int minvalue, int maxvalue) {
  QSpinBox *spinBox = new QSpinBox();
  spinBox->setValue(initialValue);
  spinBox->setMinimum(minvalue); spinBox->setMaximum(maxvalue);
  wizard->addRow(name, spinBox);
  connect(spinBox, SIGNAL(valueChanged(int)),
    this, SLOT(WizardSave()));
  return spinBox;
}


//Append at the end of the wizard a combobox with the given name, the list of choices
QComboBox * ConfigFileViewerWidget::wizardAddComboBox(QString name, QList<QString> *possibleValues) {
  QComboBox * comboBox = new QComboBox();
  for (int i = 0; i < possibleValues->size(); i++) {
    comboBox->addItem(possibleValues->at(i));
  }
  connect(comboBox, SIGNAL(currentIndexChanged(int)),
    this, SLOT(WizardSave()));
  wizard->addRow(name, comboBox);
  return comboBox;
}

//Append at the end of the wizard a LineEdit, whose purpose is to select a path with a file chooser. 
pathEditBox * ConfigFileViewerWidget::wizardAddPathEditBox(QString name, QString value, bool chooseDirectory) {
  pathEditBox *PathEditBox = new pathEditBox(value, path, chooseDirectory);
  connect(PathEditBox->getLineEdit(), SIGNAL(textChanged(QString)), this, SLOT(WizardSave()));
  wizard->addRow(name, PathEditBox);
  return PathEditBox;
}

//Append at the end of the wizard a line edit with a given name and value.
QLineEdit *ConfigFileViewerWidget::wizardAddLineEdit(QString name, QString value) {
  QLineEdit *lineEdit = new QLineEdit(value);
  wizard->addRow(name, lineEdit);
  connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(WizardSave()));
  return lineEdit;
}


/////////////////////////
////pathEditBox class////
/////////////////////////

//Constructor of pathEditBox
pathEditBox::pathEditBox(QString value, QString startPathToSearch, bool chooseDirectory) {
  this->chooseDirectory = chooseDirectory;
  this->startPlaceToSearch = startPathToSearch;
  layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  text = new QLineEdit(value);
  FileChooserOpener = new QPushButton("...");
  FileChooserOpener->setMaximumWidth(50);
  layout->addWidget(text, 2);
  layout->addWidget(FileChooserOpener, 1);
  connect(FileChooserOpener, SIGNAL(clicked()),
    this, SLOT(selectFile()));
  this->setLayout(layout);
  this->setContentsMargins(0, 0, 0, 0);
}

//Return the value contained in the QLineEdit
QString pathEditBox::getValue() {
  return text->text();
}

//return the lineEdit contained in the PathEditBox
QLineEdit * pathEditBox::getLineEdit() {
  return text;
}

//Select a filepath using the QFileDialog.
void pathEditBox::selectFile() {
  QString path = NULL;
  if (chooseDirectory) {
    path = QFileDialog::getExistingDirectory(this, this->startPlaceToSearch);
  }
  else {
    path = QFileDialog::getOpenFileName(this, "Choose a file", this->startPlaceToSearch, "All files (*)");
  }

  if (path.isNull()) {
    text->setText(path);
  }

}
