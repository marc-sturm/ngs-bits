#ifndef FILEINFOPROVIDERREMOTE_H
#define FILEINFOPROVIDERREMOTE_H

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include "cppNGS_global.h"
#include "FileInfoProvider.h"
#include "ApiRequestHandler.h"
#include "Settings.h"

class CPPNGSSHARED_EXPORT FileInfoProviderRemote : virtual public FileInfoProvider
{
public:
	FileInfoProviderRemote(QString file);
	virtual ~FileInfoProviderRemote() {}

	QString absolutePath() override;
	QString absoluteFilePath() override;
	QString dirAbsolutePath() override;
	QString parentDirAbsolutePath() override;
	QString parentSubDirAbsolutePath(QString subdir) override;
	QString baseName() override;
	QString fileName() override;
	QString suffix() override;
	QDateTime lastModified() override;
	bool exists() override;

private:
	QJsonObject getFileInfo(QString file);

protected:
	QString file_;
	QString server_host_;
	int server_port_;
};

#endif // FILEINFOPROVIDERSERVER_H
