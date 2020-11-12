#ifndef ENDPOINTHANDLER_H
#define ENDPOINTHANDLER_H

#include <QFile>
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "Exceptions.h"
#include "WebEntity.h"
#include "FileCache.h"
#include "NGSD.h"
#include "SessionManager.h"
#include "EndpointManager.h"

#include "FileLocationProviderFileSystem.h"
#include "VariantList.h"


class EndpointHandler : public QObject
{
    Q_OBJECT

public:
    EndpointHandler();
    ~EndpointHandler();

	static bool isEligibileToAccess(Request request);
	static QString getFileNameWithExtension(QString filename_with_path);
	static QByteArray readFileContent(QString filename);
	static Response serveStaticFile(QString filename, ContentType type, bool is_downloadable);
	static Response serveFolderContent(QString folder);
	static QByteArray generateHeaders(QString filename, int length, ContentType type, bool is_downloadable);
	static QByteArray generateHeaders(int length, ContentType type);
	static bool isValidUser(QString name, QString password);
	static QString getGSvarFile(QString sample_name, bool search_multi);

	static Response serveIndexPage(Request request);
	static Response serveApiInfo(Request request);
	static Response listFolderContent(Request request);
	static Response locateFileByType(Request request);
	static Response serveEndpointHelp(Request request);
	static Response serveStaticFile(Request request);
	static Response serveProtectedStaticFile(Request request);
	static Response performLogin(Request request);
	static Response performLogout(Request request);
};

#endif // ENDPOINTHANDLER_H
