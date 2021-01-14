#ifndef ENDPOINTHELPER_H
#define ENDPOINTHELPER_H

#include "cppREST_global.h"
#include "WebEntity.h"
#include "SessionManager.h"
#include "FileCache.h"
#include "EndpointManager.h"


class CPPRESTSHARED_EXPORT EndpointHelper
{
public:
	static bool isEligibileToAccess(Request request);
	static QString getFileNameWithExtension(QString filename_with_path);
	static StaticFile readFileContent(QString filename, ByteRange byte_range);
	static QString addFileToCache(QString filename);
	static Response serveStaticFile(QString filename, ByteRange byte_range, ContentType type, bool is_downloadable);
	static Response serveStaticFileFromCache(QString id, ByteRange byte_range, ContentType type, bool is_downloadable);
	static Response serveFolderContent(QString folder);
	static QByteArray generateHeaders(QString filename, int length, ByteRange byte_range, qint64 file_size, ContentType type, bool is_downloadable);
	static QByteArray generateHeaders(int length, ContentType type);
	static Response listFolderContent(Request request);
	static Response serveEndpointHelp(Request request);
	static Response serveStaticFile(Request request);
	static Response serveStaticFileFromCache(Request request);
	static Response serveProtectedStaticFile(Request request);

protected:
	EndpointHelper();
	~EndpointHelper();

private:
	static EndpointHelper& instance();
};

#endif // ENDPOINTHELPER_H
