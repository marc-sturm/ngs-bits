#include "Api.h"

Api::Api()
{

}

Api::~Api()
{

}

Response Api::processRequest(Request request)
{
	Response response {};
	qDebug() << "PATH = ";
	qDebug() << request.path;
	QList<QByteArray> path_items = request.path.split('/');
	if (path_items.isEmpty())
	{
		return showError(WebEntity::ErrorType::NOT_FOUND, "No path has been provided");;
	}

	if ((path_items[1] == "") && request.method == Request::MethodType::GET)
	{
		return serveStaticFile(":/assets/client/info.html", WebEntity::TEXT_HTML);
	}

	if ((path_items[1] == QByteArrayLiteral("info")) && request.method == Request::MethodType::GET)
	{
		return serveStaticFile(":/assets/client/api.json", WebEntity::APPLICATION_JSON);
	}


	return showError(WebEntity::ErrorType::NOT_FOUND, "The page you are looking for does not exist");
}

QByteArray Api::readFileContent(QString filename)
{
	QByteArray content {};
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "File has not been found";
		return content;
	}

	if (!file.atEnd())
	{
		content = file.readAll();
	}
	return content;
}

Response Api::serveStaticFile(QString filename, WebEntity::ContentType type)
{
	QByteArray headers {};
	QByteArray content = readFileContent(filename);

	headers.append("HTTP/1.1 200 OK\n");
	headers.append("Content-Length: " + QString::number(content.length()) + "\n");
	headers.append("Content-Type: " + WebEntity::contentTypeToString(type) + "\n");
	headers.append("\n");

	return Response{headers, content};
}

Response Api::showError(WebEntity::ErrorType type, QString message)
{
	QByteArray headers {};
	QString caption = WebEntity::errorTypeToText(type);
	QByteArray content = readFileContent(":/assets/client/error.html");
	QString body = QString(content);

	if (message.isEmpty())
	{
		message	= "No information provided";
	}
	body.replace("%TITLE%", "Error " + QString::number(WebEntity::errorCodeByType(type)) + " - " + WebEntity::errorTypeToText(type));
	body.replace("%MESSAGE%", message);

	headers.append("HTTP/1.1 " + QString::number(WebEntity::errorCodeByType(type)) + " FAIL\n");
	headers.append("Content-Length: " + QString::number(body.length()) + "\n");
	headers.append("\n");

	return Response{headers, body.toLocal8Bit()};
}
