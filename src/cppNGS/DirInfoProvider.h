#ifndef DIRINFOPROVIDER_H
#define DIRINFOPROVIDER_H

#include <QDir>
#include "Exceptions.h"
#include "cppNGS_global.h"

class CPPNGSSHARED_EXPORT DirInfoProvider
{
public:
	virtual ~DirInfoProvider(){}

	virtual QString absolutePath() = 0;
	virtual QString absoluteFilePath(QString file) = 0;
	virtual QString parentAbsolutePath() = 0;
	virtual bool exists() = 0;

	virtual QStringList entryList(const QStringList &nameFilters, QDir::Filters filters = QDir::NoFilter,
						  QDir::SortFlags sort = QDir::NoSort) const = 0;
};

#endif // DIRINFOPROVIDER_H
