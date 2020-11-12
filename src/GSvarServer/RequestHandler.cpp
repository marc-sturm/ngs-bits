#include "RequestHandler.h"

static qint64 MAX_REQUEST_LENGTH = 2048; // for the IE compatibility

RequestHandler::RequestHandler(QTcpSocket *sock)
	: socket(sock)
{
	qRegisterMetaType<Response>();
	connect(sock, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

RequestHandler::~RequestHandler()
{
}

Request::MethodType RequestHandler::inferRequestMethod(QByteArray in)
{
	if (in.toUpper() == QByteArrayLiteral("GET"))
	{
		return Request::MethodType::GET;
	}
	if (in.toUpper() == QByteArrayLiteral("POST"))
	{
		return Request::MethodType::POST;
	}
	if (in.toUpper() == QByteArrayLiteral("DELETE"))
	{
		return Request::MethodType::DELETE;
	}
	if (in.toUpper() == QByteArrayLiteral("PUT"))
	{
		return Request::MethodType::PUT;
	}
	if (in.toUpper() == QByteArrayLiteral("PATCH"))
	{
		return Request::MethodType::PATCH;
	}

	THROW(ArgumentException, "Incorrect request method");
}

QList<QByteArray> RequestHandler::getRequestBody()
{
	QList<QByteArray> request_items;

	if (socket->canReadLine())
	{
		request_items = socket->readAll().split('\n');
		for (int i = 0; i < request_items.count(); ++i)
		{
			request_items[i] = request_items[i].trimmed();
		}
	}
	else if (socket->bytesAvailable() > MAX_REQUEST_LENGTH)
	{
                writeResponse(WebEntity::createError(ErrorType::BAD_REQUEST, ContentType::TEXT_HTML, "Maximum request lenght has been exceeded"));
	}

	return request_items;
}

QList<QByteArray> RequestHandler::getKeyValuePair(QByteArray in)
{
	QList<QByteArray> result {};

	if (in.indexOf('=')>-1)
	{
		result = in.split('=');
	}

	return result;
}

QMap<QString, QString> RequestHandler::getVariables(QByteArray in)
{
	QMap<QString, QString> url_vars {};
	QList<QByteArray> var_list = in.split('&');
	QByteArray cur_key {};

	for (int i = 0; i < var_list.count(); ++i)
	{
		QList<QByteArray> pair = getKeyValuePair(var_list[i]);
		if (pair.length()==2)
		{
			url_vars.insert(pair[0], pair[1]);
		}
	}

	return url_vars;
}

QByteArray RequestHandler::getVariableSequence(QByteArray url)
{
	QByteArray var_string {};
	if (url.indexOf('?') == -1) return var_string;
	return url.split('?')[1];
}

QString RequestHandler::getRequestPrefix(QList<QString> path_items)
{
	if (path_items.count()>1)
	{
		return WebEntity::getUrlWithoutParams(path_items[1]);
	}
	return "";
}

QString RequestHandler::getRequestPath(QList<QString> path_items)
{
	if (path_items.count()>2)
	{		
		return WebEntity::getUrlWithoutParams(path_items[2]);
	}
	return "";
}

QList<QString> RequestHandler::getRequestPathParams(QList<QString> path_items)
{
	QList<QString> params {};
	if (path_items.count()>3)
	{
		for (int p = 3; p < path_items.count(); ++p)
		{
			if (!path_items[p].trimmed().isEmpty())
			{
				params.append(path_items[p].trimmed());
			}
		}
	}
	return params;
}

void RequestHandler::parseRequest(QList<QByteArray> body)
{
	Request request {};
	request.remote_address = socket->peerAddress().toString();

	for (int i = 0; i < body.count(); ++i)
	{
		// First line with method type and URL
		if (i == 0)
		{
			QList<QByteArray> request_info = body[i].split(' ');
			if (request_info.length() < 2)
			{
                                writeResponse(WebEntity::createError(ErrorType::BAD_REQUEST, ContentType::TEXT_HTML, "Cannot process the request. It is possible a URL is missing or incorrect"));
				return;
			}
			try
			{
				request.method = inferRequestMethod(request_info[0].toUpper());
			}
			catch (ArgumentException& e)
			{
                                writeResponse(WebEntity::createError(ErrorType::BAD_REQUEST, ContentType::TEXT_HTML, e.message()));
				return;
			}


			QList<QString> path_items = QString(request_info[1]).split('/');

			request.prefix = getRequestPrefix(path_items);
			request.path = getRequestPath(path_items);
			request.path_params = getRequestPathParams(path_items);

			qDebug() << request.path_params;
			request.url_params = getVariables(getVariableSequence(request_info[1]));
			continue;
		}

		// Reading headers and params
		int header_separator = body[i].indexOf(':');
		int param_separator = body[i].indexOf('=');
		if ((header_separator == -1) && (param_separator == -1) && (body[i].length() > 0))
		{
                        writeResponse(WebEntity::createError(ErrorType::BAD_REQUEST, ContentType::TEXT_HTML, "Malformed element: " + body[i]));
			return;
		}

		if (header_separator > -1)
		{
			request.headers.insert(body[i].left(header_separator).toLower(), body[i].mid(header_separator+1).trimmed());
		}
		else if (param_separator > -1)
		{
			request.form_urlencoded = getVariables(body[i]);
		}
	}

        request.return_type = ContentType::TEXT_HTML;
        if (request.headers.contains("accept"))
        {
            if (WebEntity::getContentTypeFromString(request.headers["accept"]) == ContentType::APPLICATION_JSON)
            {
                request.return_type = ContentType::APPLICATION_JSON;
            }
        }


	qDebug() << "Request headers";
	QMap<QString, QString>::const_iterator i = request.headers.constBegin();
	while (i != request.headers.constEnd())
	{
		qDebug() << i.key() << ": " << i.value();
		++i;
	}
	qDebug() << "URL params: " << request.url_params;
	qDebug() << "Form: " << request.form_urlencoded;
	qDebug() << "";

	// Executing in a separate thread
	WorkerThread *workerThread = new WorkerThread(request);
	connect(workerThread, &WorkerThread::resultReady, this, &RequestHandler::handleResults);
	connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
	workerThread->start();
}

void RequestHandler::dataReceived()
{	
	parseRequest(getRequestBody());
}

void RequestHandler::writeResponse(Response response)
{
	socket->write(response.headers);
	socket->write(response.body);
	socket->flush();
	socket->close();
	socket->deleteLater();
}

bool RequestHandler::hasEndOfLineCharsOnly(QByteArray line)
{
	if (line == QByteArrayLiteral("\r\n") || line == QByteArrayLiteral("\n"))
	{
		return true;
	}
	return false;
}

void RequestHandler::handleResults(const Response &response)
{
	writeResponse(response);
}
