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

StaticFile EndpointHelper::readFileContent(QString filename, ByteRange byte_range)
{
	qDebug() << "Reading file:" << filename;
	StaticFile static_file {};
	static_file.filename_with_path = filename;
	static_file.modified = QFileInfo(filename).lastModified();

	QString found_id = FileCache::getFileIdIfInCache(filename);
	if (found_id.length() > 0)
	{
		qDebug() << "File has been found in the cache:" << found_id;
		return FileCache::getFileById(found_id);
	}


	QFile file(filename);
	static_file.size = file.size();
	if (!file.open(QIODevice::ReadOnly))
	{
		THROW(FileAccessException, "File could not be found: " + filename);
	}

	if ((!file.atEnd()) && (byte_range.length == 0))
	{
		qDebug() << "Reading the entire file at once";
		static_file.content = file.readAll();
	}

	if ((!file.atEnd()) && (byte_range.length > 0) && (file.seek(byte_range.start)))
	{
		qDebug() << "Partial file reading";
		static_file.content = file.read(byte_range.length);
	}

//	if (!content.isEmpty())
//	{
//		qDebug() << "Adding file to the cache:" << filename;
//		FileCache::addFileToCache(ServerHelper::generateUniqueStr(), filename, content);
//	}

	file.close();

	return static_file;
}

QString EndpointHelper::addFileToCache(QString filename)
{
	readFileContent(filename, ByteRange{});
	return FileCache::getFileIdIfInCache(filename);
}

Response EndpointHelper::serveStaticFile(QString filename, ByteRange byte_range, ContentType type, bool is_downloadable)
{
	StaticFile static_file {};
	try
	{
		static_file = readFileContent(filename, byte_range);
	}
	catch(Exception& e)
	{
		return WebEntity::createError(ErrorType::INTERNAL_SERVER_ERROR, ContentType::TEXT_HTML, e.message());
	}

	return (Response{generateHeaders(getFileNameWithExtension(filename), static_file.content.length(), byte_range, static_file.size, type, is_downloadable), static_file.content});
}

Response EndpointHelper::serveStaticFileFromCache(QString id, ByteRange byte_range, ContentType type, bool is_downloadable)
{
	StaticFile static_file = FileCache::getFileById(id);

	if (static_file.content.isEmpty() || static_file.content.isNull())
	{
		return WebEntity::createError(ErrorType::INTERNAL_SERVER_ERROR, ContentType::TEXT_HTML, "Empty or corrpupted file");
	}

	return (Response{generateHeaders(getFileNameWithExtension(FileCache::getFileById(id).filename_with_path), static_file.content.length(), byte_range, static_file.size, type, is_downloadable), static_file.content});
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

QByteArray EndpointHelper::generateHeaders(QString filename, int length, ByteRange byte_range, qint64 file_size, ContentType type, bool is_downloadable)
{
	QByteArray headers {};
	if ((byte_range.end > 0) && (byte_range.length > 0))
	{
		headers.append("HTTP/1.1 206 Partial Content\n");
	}
	else
	{
		headers.append("HTTP/1.1 200 OK\n");
	}
	headers.append("Date: " + QDateTime::currentDateTime().toUTC().toString() + "\n");
	headers.append("Server: " + ServerHelper::getAppName() + "\n");
	headers.append("Connection: Keep-Alive\n");
	headers.append("Keep-Alive: timeout=5, max=1000\n");
	headers.append("Content-Length: " + QString::number(length) + "\n");
	headers.append("Content-Type: " + WebEntity::convertContentTypeToString(type) + "\n");

	if ((byte_range.end > 0) && (byte_range.length > 0))
	{
		headers.append("Accept-Ranges: bytes\n");
		headers.append("Content-Range: bytes " + QString::number(byte_range.start) + "-" + QString::number(byte_range.end) + "/" + QString::number(file_size) + "\n");
	}
	if (is_downloadable)
	{
		headers.append("Content-Disposition: form-data; name=file_download; filename=" + filename + "\n");
	}

	headers.append("\n");	
	return headers;
}

QByteArray EndpointHelper::generateHeaders(int length, ContentType type)
{
	return generateHeaders("", length, ByteRange{}, 0, type, false);
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
	ByteRange byte_range {};
	byte_range.start = 0;
	byte_range.end = 0;
	if (request.headers.contains("range"))
	{
		QString range_value = request.headers.value("range");
		qDebug() << "Reading byte range header:" << range_value;
		range_value = range_value.replace("bytes", "");
		range_value = range_value.replace("=", "");
		range_value = range_value.trimmed();
		if (range_value.count("-") == 1)
		{
			byte_range.start = static_cast<quint64>(range_value.mid(0, range_value.indexOf("-")).trimmed().toULongLong());
			byte_range.end = static_cast<quint64>(range_value.mid(range_value.indexOf("-")+1, range_value.length()-range_value.indexOf("-")).trimmed().toULongLong());
		}
	}
	qDebug() << "Minus = " << byte_range.end - byte_range.start;
	byte_range.length = ((byte_range.end - byte_range.start) > -1.0) ? (byte_range.end - byte_range.start) : 0;
	qDebug() << "bytes = " << byte_range.start << ", " << byte_range.end << ", len = " << byte_range.length;

	path = WebEntity::getUrlWithoutParams(path.trimmed() + request.path_params[0]);
	return serveStaticFile(path, byte_range, WebEntity::getContentTypeByFilename(path), false);
}

Response EndpointHelper::serveStaticFileFromCache(Request request)
{
	qDebug() << "Accessing static content from cache";
	QString path = WebEntity::getUrlWithoutParams(FileCache::getFileById(request.path_params[0]).filename_with_path);
	return serveStaticFile(path, ByteRange{}, WebEntity::getContentTypeByFilename(path), false);
}

Response EndpointHelper::serveProtectedStaticFile(Request request)
{
	if (!isEligibileToAccess(request)) return WebEntity::createError(ErrorType::FORBIDDEN, request.return_type, "Secure token has not been provided");

	return serveStaticFile(":/assets/client/example.png", ByteRange{}, ContentType::APPLICATION_OCTET_STREAM, true);
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
