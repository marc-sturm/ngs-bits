#ifndef HTTPSSERVER_H
#define HTTPSSERVER_H

#include <QObject>
#include "SslServer.h"

class HttpsServer : public QObject
{
    Q_OBJECT

public:
	HttpsServer(quint16 port);
    ~HttpsServer();

protected slots:
    void handleConnection();

private:
	SslServer *server;	
};

#endif // HTTPSSERVER_H
