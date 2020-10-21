#include <QDebug>
#include <QHostAddress>
#include <QTcpSocket>
#include <QSslSocket>
#include "RequestHandler.h"

static qint64 MAX_REQUEST_LENGTH = 2048; // for the IE compatibility

RequestHandler::RequestHandler(QTcpSocket *sock, Api *api)
	: state(State::PROCESSING_REQUEST),
	  socket(sock),
	  api(api)
{
	connect(sock, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

RequestHandler::~RequestHandler()
{
}

Request::MethodType RequestHandler::inferRequestMethod(QByteArray in)
{
	if (in.toUpper() == QByteArrayLiteral("GET")) {
		return Request::MethodType::GET;
	}
	if (in.toUpper() == QByteArrayLiteral("POST")) {
		return Request::MethodType::POST;
	}
	if (in.toUpper() == QByteArrayLiteral("DELETE")) {
		return Request::MethodType::DELETE;
	}
	if (in.toUpper() == QByteArrayLiteral("PUT")) {
		return Request::MethodType::PUT;
	}
	if (in.toUpper() == QByteArrayLiteral("PATCH")) {
		return Request::MethodType::PATCH;
	}

	// should be an exception
	return Request::MethodType::GET;
}

void RequestHandler::dataReceived()
{
	Response result {};
	Request request {};
	request.remote_address = socket->peerAddress().toString();

	switch (state)
	{
		case State::PROCESSING_REQUEST:
		{
			if (socket->canReadLine()) {
				QByteArray sent_data = socket->readLine();

				QList<QByteArray> sent_data_items = sent_data.split(' ');
				if (sent_data_items.length() < 2)
				{
					writeResponse(api->showError(WebEntity::BAD_REQUEST, "Cannot process the request. It is possible a URL is missing or incorrect"));
					return;
				}

				request.method = inferRequestMethod(sent_data_items[0].toUpper());
				request.path = sent_data_items[1];

				state = State::PROCESSING_HEADERS;
			}
			else if (socket->bytesAvailable() > MAX_REQUEST_LENGTH) {
				writeResponse(api->showError(WebEntity::BAD_REQUEST, "Maximum request lenght has been exceeded"));
				return;
			}
			[[fallthrough]];
		}

		case State::PROCESSING_HEADERS:
		{
			while (socket->canReadLine()) {
				QByteArray sent_data = socket->readLine();


				if (sent_data == QByteArrayLiteral("\r\n") || sent_data == QByteArrayLiteral("\n")) {
					result = api->processRequest(request);
					state = State::FINISHED;
					break;
				}

				int colon = sent_data.indexOf(':');
				if (colon == -1) {					
					writeResponse(api->showError(WebEntity::BAD_REQUEST, "Incorrect header: " + sent_data.toHex()));
					return;
				}

				QByteArray header = sent_data.left(colon);
				QByteArray value = sent_data.mid(colon+1);

				request.headers.insert(header.toUpper(), value.trimmed());


			}

			[[fallthrough]];
		}

		case State::FINISHED:
		{			
			writeResponse(result);
			break;
		}
    }



	QString uniqueToken = QUuid::createUuid().toString(QUuid::WithoutBraces);
	qDebug() << uniqueToken;
	QMap<QString, QString>::const_iterator i = request.headers.constBegin();
	while (i != request.headers.constEnd()) {
		qDebug() << i.key() << ": " << i.value();
		++i;
	}

}

void RequestHandler::writeResponse(Response response)
{
	socket->write(response.headers);
	socket->write(response.body);
	socket->flush();
	socket->close();
	socket->deleteLater();
}
