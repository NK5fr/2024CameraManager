#ifndef CONFIGFILEVIEWERWIDGET_H
#define CONFIGFILEVIEWERWIDGET_H

#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QScrollArea>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include "constants.h"


//In the wizard, it's a field that allows to change paths via a file chooser dialog.
class pathEditBox : public QWidget {
	Q_OBJECT
public:
	pathEditBox(QString value, QString startPlaceToSearch, bool chooseDirectory);
	QString getValue();
	QLineEdit *getLineEdit();
private slots:
	//linked to FileChooserOpener. Open the file chooser dialog and change the text in text variable accordingly.
	void selectFile();
private:
	//The place where the file chooser dialog will start in
	QString startPlaceToSearch;
	//The layout of the pathEditBox widget
	QHBoxLayout *layout;
	//The text line
	QLineEdit *text;
	//The button that allows to open a file chooser dialog
	QPushButton *FileChooserOpener;
	//Define if a directory or a file must be selected
	bool chooseDirectory;
};


class ConfigFileViewerWidget : public QMdiSubWindow{
    Q_OBJECT
public:
    ConfigFileViewerWidget(QString filePath);

private slots:
    void onRightClic();
	//Menu
    void menuProjectAction_triggered(QAction*);
    //Allow file editing
	void allowFileEditing();
	//Prevent file editing
	void preventFileEditing();
	//Save all the parameters in the Wizard into the QString managed by the QTextEdit.!! It doesn't save into the file!! To save into the file, use the saveFile method
	void WizardSave();
	//Save the QString of the QTextEdit into the hard drive.
	void saveFile();
	//Switch the view between the text view and wizard view.
	void changeView();

private:
	//Show the text view
    void showTextView();
	//Show the wizard/utilitary view
	void showWizardView();
	//create the wizard and all its components. Load also the file into wizard
	void createWizard();
	//Reset the wizard. The wizard need to be created first before resetting!
	void resetWizard();
	//Create the wizard and all its components. Load also the file into the QTextEdit
	void createTextEditor();
	//Append at the end of the wizard a new category with the given name. It is used to determine in what category you are. Return the QLabel of the category.
	QLabel * wizardAddCategory(QString name);
	//Append at the end of the wizard an Integer spinbox with the given name, min value, max value. Return the newly created spinbox.
	QSpinBox * wizardAddSpinBox(QString name, int initialValue, int minvalue, int maxvalue);
	//Append at the end of the wizard a combobox with the given name, the list of choices. Return the newly created comboBox
	QComboBox * wizardAddComboBox(QString name, QList<QString> *possiblevalues);
	//Append at the end of the wizard a blank line to edit strings, with the given name and the given value
	QLineEdit *wizardAddLineEdit(QString name, QString value);
	//Append at the end of the wizard a pathEditBox, whose purpose is to select a path with a file chooser. chooseDirectory allows to select a directory or not.
	pathEditBox * wizardAddPathEditBox(QString name, QString value, bool chooseDirecory);


	QScrollArea *wizardScrollArea; 
	QWidget *wizardWrapper;
	QFormLayout *wizard;

	//QTextEdit
    QTextEdit *fileContain;

	//QTabWidget
	QTabWidget *tabs;

	//Others
	QString path;
    bool textEditable;
	int view;
	QFrame *frame;

	QAction *fileEdition;
    QAction *save;

	char debugstr;

	//Determine if all parameters in the config file have to be loaded in the wizard
	bool loadAllParameters;
	//List of all widgets in the wizard by types
	QList<QComboBox*> *wizardComboBoxes;
	QList<QList<QString>*> *wizardComboBoxesRealValues;
	QList<int> *wizardComboBoxesParameterPosition;
	
	QList<QSpinBox*> *wizardSpinBoxes;
	QList<int> *wizardSpinBoxesParameterPosition;

	QList<pathEditBox*> *wizardPathEditBoxes;
	QList<int> *wizardPathEditBoxesParameterPosition;

	QList<QLabel*> *wizardCategories;
	QList<int> *wizardCategoriesPosition;

	QList<QLineEdit *> *wizardLineEdit;
	QList<int> *wizardLineEditParameterPosition;
};

#endif // CONFIGFILEVIEWERWIDGET_H
