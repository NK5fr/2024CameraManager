#include "configfilereader.h"
#include <QStringList>

ConfigFileReader::ConfigFileReader(QString path) {
	file = new QFile(path);
	Stream = new QTextStream(file);
}

ConfigFileReader::~ConfigFileReader(void) {
	delete file;
	delete Stream;
}

//read one int parameter at the n position. not efficient method. if reading fails, return -1
int ConfigFileReader::readInt(int n) {
	QString line;
	int nlinestopass = 0;
	int r= -1;
	int i;
	bool b = false;
	//if the file is already opened, we set the position to the start of the file (it should be recalled it's not an efficient function) and start reading from there.
	if(file->isOpen()) {
		if(!Stream->seek(0)) return -1;
		//else the file is Not opened, we open it and start reading it.
	} else {
		if(!file->open(QIODevice::ReadOnly | QIODevice::Text)) return -1;
	}
	//read 1st parameter which correspond to the number of lines to pass
	*Stream >> nlinestopass;
	if (!jumpLines(nlinestopass)) return -1;

		
	//Start reading parameters one by one.
	i = 0;
	while (i < n && !(Stream->atEnd())) {
		line = Stream->readLine();
		//jumps all comment lines.
		if(!line.contains("!")) {
			i++;
		}
	}
	r = line.toInt(&b, 10);
	if(b) return r;
	return -1;
}

//read one string parameter at the n position. not efficient method. if reading fails, return NULL
QString ConfigFileReader::readString(int n) {
	QString line;
	int i;
	int nlinestopass = 0;
	//if the file is already opened, we set the position to the start of the file (it should be recalled it's not an efficient function) and start reading from there.
	if(file->isOpen()) {
		if(!Stream->seek(0)) return NULL;
	//else the file is Not opened, we open it and start reading it.
	} else {
		if(!file->open(QIODevice::ReadOnly | QIODevice::Text)) return NULL;
	}
	//read 1st parameter which correspond to the number of lines to pass
	*Stream >> nlinestopass;
	if (!jumpLines(nlinestopass)) return NULL;

		
	//Start reading parameters one by one.
	i = 0;
	while (i < n && !(Stream->atEnd())) {
		line = Stream->readLine();
		if(!line.contains("!")) {
			i++;
		}
	}
	if(!line.contains("!")) return line;
	return NULL;
}


//jump multiple lines in the file
bool ConfigFileReader::jumpLines(int n) {
	for (int i = 0; i<n; i++) {
		if(Stream->atEnd()) return false;
		Stream->readLine();
	}
	return true;
}



//read the next string parameter in the file, or null if there is a problem or the end of file is reached.
QString ConfigFileReader::readNextParameter() {
	if(!file->isOpen()) {
		if(!file->open(QIODevice::ReadOnly | QIODevice::Text)) return NULL;
	}
	QString line;
	while(!Stream->atEnd()) {
		line = Stream->readLine();
		if(!line.contains("!")) return line;
	}
	//If we reach the end of the file, we return NULL
	return NULL;
}

QStringList *ConfigFileReader::readNextParameterAndComment() {
	QStringList *r = new QStringList();
	if(!file->isOpen()) {
		if(!file->open(QIODevice::ReadOnly | QIODevice::Text)) return NULL;
	}
	if(!Stream->atEnd()) {
		r->append(Stream->readLine());
	} else {
		return NULL;
	}
	if(!Stream->atEnd()) {
		r->append(Stream->readLine());
	} else {
		return NULL;
	}
	//if the two lines are not comment, it means that we reached the end of the file, with all the numbers for the approximate location.
	if(!r->at(0).contains("!") && !r->at(1).contains("!")) return NULL;
	//if the second line contains a "!" (that shouldn't happen since it's the parameter), we swap with the first line and we continue the reading.
	while(r->at(1).contains("!")) {
		if(!Stream->atEnd()) {
            r->swapItemsAt(0,1);
			r->replace(1, Stream->readLine());
		} else {
			return NULL;
		}
	}
	return r;
}
