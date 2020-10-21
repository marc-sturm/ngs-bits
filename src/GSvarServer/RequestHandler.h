#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>
#include <QTcpSocket>

#include <QList>
#include <QUuid>
#include "WebEntity.h"
#include "Api.h"
#include "Exceptions.h"


class RequestHandler : public QObject
{
    Q_OBJECT

public:
    enum State {
		PROCESSING_REQUEST,
		PROCESSING_HEADERS,
		FINISHED
    };

	RequestHandler(QTcpSocket *socket, Api *api);
	~RequestHandler();

private slots:
	void dataReceived();

private:
    State state;
	QTcpSocket *socket;
	Api *api;
	Request::MethodType inferRequestMethod(QByteArray in);
	void writeResponse(Response response);

};

#endif // REQUESTHANDLER
