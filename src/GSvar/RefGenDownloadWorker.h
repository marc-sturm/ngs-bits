#ifndef REFGENDOWNLOADWORKER_H
#define REFGENDOWNLOADWORKER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFile>

#include "WorkerBase.h"
#include "Settings.h"
#include "HttpHandler.h"
#include "HttpRequestHandler.h"


class RefGenDownloadWorker : public WorkerBase
{
	Q_OBJECT

public:
	RefGenDownloadWorker();
	virtual void process();

signals:
	void updated(int value);
	void messaged(QString message);
	void interrupted();

};

#endif // REFGENDOWNLOADWORKER_H
