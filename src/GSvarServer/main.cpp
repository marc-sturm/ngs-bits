#include <QCoreApplication>
#include "HttpsServer.h"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	HttpsServer sslserver(8443);
	return app.exec();
}
