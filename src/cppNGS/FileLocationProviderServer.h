#ifndef FILELOCATIONPROVIDERSERVER_H
#define FILELOCATIONPROVIDERSERVER_H

#include "cppNGS_global.h"
#include "FileLocationProvider.h"
#include "HttpRequestHandler.h"
#include "Settings.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


class CPPNGSSHARED_EXPORT FileLocationProviderServer : virtual public FileLocationProvider
{
public:
	FileLocationProviderServer(const QString sample_id, const QString server_host, const int server_port);
	virtual ~FileLocationProviderServer() {}

	QList<FileLocation> getBamFiles() override;
	QList<FileLocation> getSegFilesCnv() override;
	QList<FileLocation> getIgvFilesBaf() override;
	QList<FileLocation> getMantaEvidenceFiles() override;

private:
	void setIsFoundFlag(FileLocation& file);
	QString sendGetRequest(QString path);
	QList<FileLocation> requestFileInfoByType(PathType type);
	FileLocation mapJsonObjectToFileLocation(QJsonObject obj);
	QList<FileLocation> mapJsonArrayToFileLocationList(QJsonArray array);

protected:
	QString sample_id_;
	QString server_host_;
	int server_port_;
};

#endif // FILELOCATIONPROVIDERSERVER_H
