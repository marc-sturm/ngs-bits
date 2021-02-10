#ifndef DIRINFOPROVIDERSERVER_H
#define DIRINFOPROVIDERSERVER_H

#include <QJsonDocument>
#include <QJsonObject>
#include "cppNGS_global.h"
#include "DirInfoProvider.h"
#include "ApiRequestHandler.h"
#include "Settings.h"

class CPPNGSSHARED_EXPORT DirInfoProviderServer : virtual public DirInfoProvider
{
public:
	DirInfoProviderServer(QString directory);
	virtual ~DirInfoProviderServer() {}

	QString absolutePath() override;
	QString absoluteFilePath(QString file) override;
	QString parentAbsolutePath() override;
	bool exists() override;

	QStringList entryList(const QStringList &nameFilters, QDir::Filters filters = QDir::NoFilter,
						  QDir::SortFlags sort = QDir::NoSort) const override;
private:
	QJsonObject getDirInfo(QString file);

protected:
	QString directory_;
	QString server_host_;
	int server_port_;
};


#endif // DIRINFOPROVIDERSERVER_H
