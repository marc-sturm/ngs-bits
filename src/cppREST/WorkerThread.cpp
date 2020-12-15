#include "WorkerThread.h"

WorkerThread::WorkerThread(Request request)
	: request_(request)
{
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
