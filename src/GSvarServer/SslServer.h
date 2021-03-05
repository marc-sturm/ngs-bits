#ifndef SSLSERVER_P_H
#define SSLSERVER_P_H

#include <QTcpServer>
#include <QSslSocket>
#include <QSslError>
#include <QSslConfiguration>
#include <QList>
#include "Exceptions.h"

class SslServer : public QTcpServer
{
    Q_OBJECT

public:
	SslServer(QObject *parent = nullptr);
	virtual ~SslServer();
	QSslConfiguration getSslConfiguration() const;
	void setSslConfiguration(const QSslConfiguration &ssl_configuration);
    QSslSocket *nextPendingConnection();

Q_SIGNALS:
	void sslFailed(const QList<QSslError> &error);
	void verificationFailed(const QSslError &error);
	void securelyConnected();

protected:
    virtual void incomingConnection(qintptr socket);

private:
	QSslConfiguration current_ssl_configuration_;
};

#endif // SSLSERVER_P_H
