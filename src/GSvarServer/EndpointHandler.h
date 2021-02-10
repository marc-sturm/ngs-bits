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
#include "EndpointHelper.h"
#include "UrlManager.h"

#include "FileLocationProviderLocal.h"
#include "VariantList.h"


class EndpointHandler
{


public:
    EndpointHandler();
    ~EndpointHandler();

	static Response serveIndexPage(Request request);
	static Response serveApiInfo(Request request);
	static Response serveTempUrl(Request request);
	static Response locateFileByType(Request request);
	static Response locateProjectFile(Request request);
	static Response performLogin(Request request);
	static Response performLogout(Request request);

private:
	static bool isValidUser(QString name, QString password);
	static QString getGSvarFile(QString sample_name, bool search_multi);
	static QString createTempUrl(QString filename);
};

#endif // ENDPOINTHANDLER_H
