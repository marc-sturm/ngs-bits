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
	QString ssl_certificate = ServerHelper::getStringSettingsValue("ssl_certificate");
	if (ssl_certificate.isEmpty())
	{
		qFatal("SSL certificate has not been specified in the config");
	}

	QFile certFile(ssl_certificate);
	if (!certFile.open(QIODevice::ReadOnly))
	{
		qFatal("Unable to load SSL certificate");
        return;
    }

	QString ssl_key = ServerHelper::getStringSettingsValue("ssl_key");
	if (ssl_key.isEmpty())
	{
		qFatal("SSL key has not been specified in the config");
	}

	QFile keyFile(ssl_key);
	if (!keyFile.open(QIODevice::ReadOnly))
	{
		qFatal("Unable to load SSL key");
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
	if (server->listen(QHostAddress::Any, port))
	{
		qInfo() << "HTTPS server is running on port #" << port;
	}
	else
	{
		qCritical() << "Could not start the HTTPS server on port #" << port << ":" << server->serverError();
	}	
}

HttpsServer::~HttpsServer()
{
}

void HttpsServer::handleConnection()
{
	while(server->hasPendingConnections())
	{
        QSslSocket *sock = server->nextPendingConnection();
		RequestHandler *handler = new RequestHandler(sock);
	}
}
