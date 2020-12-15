#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "cppREST_global.h"
#include <QObject>
#include <QTcpSocket>
#include <QList>
#include <QDebug>
#include <QHostAddress>
#include <QSslSocket>
#include "WebEntity.h"
#include "Exceptions.h"
#include "NGSD.h"
#include "Settings.h"
#include "WorkerThread.h"

Q_DECLARE_METATYPE(Response)

class CPPRESTSHARED_EXPORT RequestHandler : public QObject
{
    Q_OBJECT

public:
	RequestHandler(QTcpSocket *socket);
	~RequestHandler();

private slots:
	void dataReceived();

private:
	QTcpSocket *socket;	
	Request::MethodType inferRequestMethod(QByteArray in);
	void writeResponse(Response response);
	bool hasEndOfLineCharsOnly(QByteArray line);
	void handleResults(const Response &response);
	QList<QByteArray> getRequestBody();
	QList<QByteArray> getKeyValuePair(QByteArray in);
	QMap<QString, QString> getVariables(QByteArray in);
	QByteArray getVariableSequence(QByteArray url);
	QString getRequestPrefix(QList<QString> path_items);
	QString getRequestPath(QList<QString> path_items);
	QList<QString> getRequestPathParams(QList<QString> path_items);
	void parseRequest(QList<QByteArray> body);
};

#endif // REQUESTHANDLER
