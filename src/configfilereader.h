#ifndef CONFIGFILEREADER_H
#define CONFIGFILEREADER_H

#include <QString>
#include <QFile>
#include <QTextStream>

/**Specific class for reading config files

*/
class ConfigFileReader {
public:
	//constructor
	ConfigFileReader(QString path);
	//Read one Integer parameter at the n position. not efficient method. Use preferably the other methods to avoid performance issues.
	int readInt(int n);
	//read one string parameter at the n position. not efficient method. Use preferably the other methods to avoid performance issues. 
	QString readString(int n);
	//read the next parameter in the file. It returns NULL if there is a problem (end of file, can't open the file). Automatically open the file if it isn't opened.
	QString readNextParameter();
	//Read the couple comment/parameter value. It returns a list with its first element the comment and the second the parameter value. It is mainly useful to create list of parameters while you don't know the name it has to be given.
	QStringList *readNextParameterAndComment();

	~ConfigFileReader(void);

private:
	//jump n lines in the file. the file MUST be opened before. Return true if success, false if failing or reaching the end of file prematurely.
	bool jumpLines(int n);

	QString path;
	QFile *file;
	QTextStream * Stream;
};

#endif