#include <QDebug>
#include <QFile>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslConfiguration>
#include <QSslSocket>

#include "SslServer.h"
#include "RequestHandler.h"
#include "HttpsServer.h"

HttpsServer::HttpsServer(quint16 port)
{
	QFile certFile(":/assets/ssl/dev-cert.crt");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to load certificate";
        return;
    }

	QFile keyFile(":/assets/ssl/dev-key.key");
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to load key";
        return;
    }

    QSslCertificate cert(&certFile);
    QSslKey key(&keyFile, QSsl::Rsa);

	server = new SslServer(this);
	connect(server, SIGNAL(securelyConnected()), this, SLOT(handleConnection()));

	QSslConfiguration config = server->getSslConfiguration();
    config.setLocalCertificate(cert);
    config.setPrivateKey(key);
    server->setSslConfiguration(config);
	if (server->listen(QHostAddress::Any, port)) {
		qDebug() << "HTTPS server is running on port #" << port;		
	}
	else
	{
		qDebug() << "Could not start the HTTPS server on port #" << port << ":" << server->serverError();
	}

	api = new Api();
}

HttpsServer::~HttpsServer()
{
}

void HttpsServer::handleConnection()
{
    while(server->hasPendingConnections()) {
        QSslSocket *sock = server->nextPendingConnection();
		RequestHandler *handler = new RequestHandler(sock, api);

		qDebug() << "------------------------------";
		qDebug() << "Pendingg connection!!!!!";
		qDebug() << "------------------------------";
    }
}
