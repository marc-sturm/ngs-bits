#ifndef API_H
#define API_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include "WebEntity.h"

class Api : public QObject
{
	Q_OBJECT

public:
	Api();
	~Api();

	Response processRequest(Request request);
	Response showError(WebEntity::ErrorType type, QString message);

private:
	QByteArray readFileContent(QString filename);
	Response serveStaticFile(QString filename, WebEntity::ContentType type);

};

#endif // API_H
