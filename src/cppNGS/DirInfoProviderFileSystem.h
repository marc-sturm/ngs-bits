#ifndef DIRINFOPROVIDERFILESYSTEM_H
#define DIRINFOPROVIDERFILESYSTEM_H

#include "DirInfoProvider.h"

class CPPNGSSHARED_EXPORT DirInfoProviderFileSystem : virtual public DirInfoProvider
{
public:
	DirInfoProviderFileSystem(QString directory);
	virtual ~DirInfoProviderFileSystem() {}

	QString absolutePath() override;
	QString absoluteFilePath(QString file) override;
	QString parentAbsolutePath() override;
	bool exists() override;

	QStringList entryList(const QStringList &nameFilters, QDir::Filters filters = QDir::NoFilter,
						  QDir::SortFlags sort = QDir::NoSort) const override;
private:
	QString directory_;
};

#endif // DIRINFOPROVIDERFILESYSTEM_H
