#include "WorkerThread.h"

WorkerThread::WorkerThread(Request request)
	: request_(request)
{
}

QByteArray WorkerThread::readFileContent(QString filename_with_path)
{
	qDebug() << "Reading file:" << filename_with_path;
	QByteArray content {};

	QString found_id = FileCache::getFileIdIfInCache(filename_with_path);
	if (found_id.length() > 0)
	{
		qDebug() << "File has been found in the cache:" << found_id;
		return FileCache::getFileById(found_id).content;
	}


	QFile file(filename_with_path);
	if (!file.open(QIODevice::ReadOnly))
	{
		THROW(FileAccessException, "File could not be found: " + filename_with_path);
	}

	if (!file.atEnd())
	{
		content = file.readAll();
	}

	qDebug() << "Adding file to the cache:" << filename_with_path;
	FileCache::addFileToCache(WebEntity::generateToken(), filename_with_path, content);
	return content;
}

Response WorkerThread::serveStaticFile(QString filename_with_path, WebEntity::ContentType type, bool is_downloadable)
{
	QByteArray body {};
	try
	{
		body = readFileContent(filename_with_path);
	}
	catch(Exception& e)
	{
		return WebEntity::createError(WebEntity::INTERNAL_SERVER_ERROR, e.message());
	}

	return (Response{generateHeaders(getFileNameAndExtension(filename_with_path), body.length(), type, is_downloadable), body});
}

Response WorkerThread::serveFolderContent(QString folder)
{
	QDir dir(folder);
	if (!dir.exists())
	{
		return WebEntity::createError(WebEntity::INTERNAL_SERVER_ERROR, "Requested folder does not exist");
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
		current_item.modified = fileInfo.metadataChangeTime();
		current_item.is_folder = fileInfo.isDir() ? true : false;
		files.append(current_item);
		qDebug() << "File:" << fileInfo.fileName() << ", " << fileInfo.size() << fileInfo.isDir();

	}
	return WebEntity::cretateFolderListing(files);
}

QByteArray WorkerThread::generateHeaders(QString filename, int length, WebEntity::ContentType type, bool is_downloadable)
{
	QByteArray headers {};
	headers.append("HTTP/1.1 200 OK\n");
	headers.append("Connection: Keep-Alive\n");
	headers.append("Keep-Alive: timeout=5, max=1000\n");
	headers.append("Content-Length: " + QString::number(length) + "\n");
	headers.append("Content-Type: " + WebEntity::contentTypeToString(type) + "\n");
	if (is_downloadable)
	{
		headers.append("Content-Disposition: form-data; name=file_download; filename=" + filename + "\n");
	}

	headers.append("\n");

	return headers;
}

QByteArray WorkerThread::generateHeaders(int length, WebEntity::ContentType type)
{
	return generateHeaders("", length, type, false);
}

QString WorkerThread::getUrlPartWithoutParams(QByteArray url)
{
	QList<QByteArray> url_parts = url.split('?');
	return QString(url_parts[0]);
}

bool WorkerThread::isValidUser(QString user_id, QString password)
{
	try
	{
		NGSD db;
		QString message = db.checkPassword(user_id, password, true);
		if (message.isEmpty())
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	catch (DatabaseException& e)
	{
		qDebug() << e.message();
		emit resultReady(WebEntity::createError(WebEntity::INTERNAL_SERVER_ERROR, e.message()));
	}
	return false;
}

bool WorkerThread::isEligibileToAccess()
{
	if ((!request_.form_urlencoded.contains("token")) && (!request_.url_params.contains("token")))
	{
		emit resultReady(WebEntity::createError(WebEntity::FORBIDDEN, "Secure token has not been provided"));
		return false;
	}
	if ((!SessionManager::isTokenValid(request_.form_urlencoded["token"])) && (!SessionManager::isTokenValid(request_.url_params["token"])))
	{
		emit resultReady(WebEntity::createError(WebEntity::UNAUTHORIZED, "Secure token is invalid"));
		return false;
	}
	return true;
}

void WorkerThread::run()
{
	qDebug() << "Processing path:" << request_.path;
	QByteArray body {};

	QList<QByteArray> path_items = request_.path.split('/');
	if (path_items.isEmpty())
	{
		emit resultReady(WebEntity::createError(WebEntity::BAD_REQUEST, "No path has been provided"));
		return;
	}

	QString first_url_part = getUrlPartWithoutParams(path_items[1]);
	QMap<QString, QString> url_vars = request_.url_params;
	qDebug() << "Variables from the request URL:" << url_vars;

	// index page
	if ((first_url_part == "") && request_.method == Request::MethodType::GET)
	{
		qDebug() << "Valid user: " << SessionManager::hasValidToken("alex");
		emit resultReady(serveStaticFile(":/assets/client/info.html", WebEntity::TEXT_HTML, false));
		return;
	}

	// api info page (e.g. version info)
	if ((first_url_part == "info") && request_.method == Request::MethodType::GET)
	{
		emit resultReady(serveStaticFile(":/assets/client/api.json", WebEntity::APPLICATION_JSON, false));
		return;
	}

	if ((first_url_part == "folder") && request_.method == Request::MethodType::GET)
	{
		emit resultReady(serveFolderContent("./"));
		return;
	}

	if ((first_url_part == "file") && (request_.method == Request::MethodType::GET))
	{
		if (!isEligibileToAccess()) return;

		emit resultReady(serveStaticFile(":/assets/client/example.png", WebEntity::APPLICATION_OCTET_STREAM, true));
		return;
	}

	if ((first_url_part == "login") && request_.method == Request::MethodType::POST)
	{
		if (!request_.form_urlencoded.contains("name") || !request_.form_urlencoded.contains("password"))
		{
			emit resultReady(WebEntity::createError(WebEntity::FORBIDDEN, "No username or/and password were found"));
			return;
		}

		if (isValidUser(request_.form_urlencoded["name"], request_.form_urlencoded["password"]))
		{
			QString secure_token = WebEntity::generateToken();
			Session cur_session = Session{request_.form_urlencoded["name"], secure_token, QDateTime::currentDateTime()};

			SessionManager::addNewSession(WebEntity::generateToken(), cur_session);
			body = secure_token.toLocal8Bit();
			emit resultReady(Response{generateHeaders(body.length(), WebEntity::TEXT_PLAIN), body});
			return;
		}

		emit resultReady(WebEntity::createError(WebEntity::UNAUTHORIZED, "Invalid username or password"));
		return;
	}

	if ((first_url_part == "logout") && request_.method == Request::MethodType::POST)
	{
		if (!request_.form_urlencoded.contains("token"))
		{
			emit resultReady(WebEntity::createError(WebEntity::FORBIDDEN, "Secure token has not been provided"));
			return;
		}
		if (SessionManager::isTokenValid(request_.form_urlencoded["token"]))
		{
			try
			{
				SessionManager::removeSession(SessionManager::getSessionIdBySecureToken(request_.form_urlencoded["token"]));
			} catch (Exception& e)
			{
				emit resultReady(WebEntity::createError(WebEntity::INTERNAL_SERVER_ERROR, e.message()));
				return;
			}
			body = request_.form_urlencoded["token"].toLocal8Bit();
			emit resultReady(Response{generateHeaders(body.length(), WebEntity::TEXT_PLAIN), body});
			return;
		}

	}

	emit resultReady(WebEntity::createError(WebEntity::NOT_FOUND, "This page does not exist. Check the URL and try again"));
}

QString WorkerThread::getFileNameAndExtension(QString filename_with_path)
{
	QList<QString> path_items = filename_with_path.split('/');
	return path_items.takeLast();
}
