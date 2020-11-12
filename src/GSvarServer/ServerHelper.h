#ifndef SERVERHELPER_H
#define SERVERHELPER_H

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QUuid>
#include "Exceptions.h"
#include "Settings.h"

class ServerHelper
{
public:
	static QString getAppName();
	static QString getAppBaseName();
	static int strToInt(QString in);
	static bool canConvertToInt(QString in);
	static QString generateUniqueStr();
	static int getNumSettingsValue(QString key);
	static QString getStringSettingsValue(QString key);

protected:
	ServerHelper();
	~ServerHelper();

private:	
	static ServerHelper& instance();

};

#endif // SERVERHELPER_H
