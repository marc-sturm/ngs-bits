#include "EndpointHelper.h"

bool EndpointHelper::isEligibileToAccess(Request request)
{
	if ((!request.form_urlencoded.contains("token")) && (!request.url_params.contains("token")))
	{
		qDebug() << "Not present";
		return false;
	}
	if ((!SessionManager::isTokenValid(request.form_urlencoded["token"])) && (!SessionManager::isTokenValid(request.url_params["token"])))
	{
		qDebug() << "Not valid";
		return false;
	}
	return true;
}

QString EndpointHelper::getFileNameWithExtension(QString filename_with_path)
{
	QList<QString> path_items = filename_with_path.split('/');
	return path_items.takeLast();
}

QByteArray EndpointHelper::readFileContent(QString filename)
{
	qDebug() << "Reading file:" << filename;
	QByteArray content {};

	QString found_id = FileCache::getFileIdIfInCache(filename);
	if (found_id.length() > 0)
	{
		qDebug() << "File has been found in the cache:" << found_id;
		return FileCache::getFileById(found_id).content;
	}


	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		THROW(FileAccessException, "File could not be found: " + filename);
	}

	if (!file.atEnd())
	{
		content = file.readAll();
	}

	qDebug() << "Adding file to the cache:" << filename;
	FileCache::addFileToCache(ServerHelper::generateUniqueStr(), filename, content);
	return content;
}

Response EndpointHelper::serveStaticFile(QString filename, ContentType type, bool is_downloadable)
{
	QByteArray body {};
	try
	{
		body = readFileContent(filename);
	}
	catch(Exception& e)
	{
		return WebEntity::createError(ErrorType::INTERNAL_SERVER_ERROR, ContentType::TEXT_HTML, e.message());
	}

	return (Response{generateHeaders(getFileNameWithExtension(filename), body.length(), type, is_downloadable), body});
}

Response EndpointHelper::serveFolderContent(QString folder)
{
	QDir dir(folder);
	if (!dir.exists())
	{
		return WebEntity::createError(ErrorType::INTERNAL_SERVER_ERROR, ContentType::TEXT_HTML, "Requested folder does not exist");
	}

	dir.setFilter(QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoSymLinks);

	QFileInfoList list = dir.entryInfoList();
	QList<FolderItem> files {};
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		if ((fileInfo.fileName() == ".") || (fileInfo.fileName() == "..")) continue;

		FolderItem current_item;
		current_item.name = fileInfo.fileName();
		current_item.size = fileInfo.size();
		current_item.modified = fileInfo.lastModified();
		current_item.is_folder = fileInfo.isDir() ? true : false;
		files.append(current_item);
		qDebug() << "File:" << fileInfo.fileName() << ", " << fileInfo.size() << fileInfo.isDir();

	}
	return WebEntity::cretateFolderListing(files);
}

QByteArray EndpointHelper::generateHeaders(QString filename, int length, ContentType type, bool is_downloadable)
{
	QByteArray headers {};
	headers.append("HTTP/1.1 200 OK\n");
	headers.append("Connection: Keep-Alive\n");
	headers.append("Keep-Alive: timeout=5, max=1000\n");
	headers.append("Content-Length: " + QString::number(length) + "\n");
	headers.append("Content-Type: " + WebEntity::convertContentTypeToString(type) + "\n");
	if (is_downloadable)
	{
		headers.append("Content-Disposition: form-data; name=file_download; filename=" + filename + "\n");
	}

	headers.append("\n");

	return headers;
}

QByteArray EndpointHelper::generateHeaders(int length, ContentType type)
{
	return generateHeaders("", length, type, false);
}

Response EndpointHelper::listFolderContent(Request request)
{
	qInfo() << "List folder content: " << request.remote_address;
	return serveFolderContent("./");
}

Response EndpointHelper::serveEndpointHelp(Request request)
{
	qInfo() << "API help page has been requested: " << request.remote_address;
	QByteArray body {};
	if (request.path_params.count() == 0)
	{
		body = EndpointManager::generateGlobalHelp().toLocal8Bit();
	}
	else
	{
		body = EndpointManager::generateEntityHelp(request.path_params[0], request.method).toLocal8Bit();
	}
	return Response{generateHeaders(body.length(), ContentType::TEXT_HTML), body};
}

Response EndpointHelper::serveStaticFile(Request request)
{
	qDebug() << "Accessing static content";
	QString path = ServerHelper::getStringSettingsValue("server_root");
	path = WebEntity::getUrlWithoutParams(path.trimmed() + request.path_params[0]);
	return serveStaticFile(path, WebEntity::getContentTypeByFilename(path), false);
}

Response EndpointHelper::serveProtectedStaticFile(Request request)
{
	if (!isEligibileToAccess(request)) return WebEntity::createError(ErrorType::FORBIDDEN, request.return_type, "Secure token has not been provided");

	return serveStaticFile(":/assets/client/example.png", ContentType::APPLICATION_OCTET_STREAM, true);
}

EndpointHelper::EndpointHelper()
{
}

EndpointHelper::~EndpointHelper()
{
}

EndpointHelper& EndpointHelper::instance()
{
	static EndpointHelper endpoint_helper;
	return endpoint_helper;
}
