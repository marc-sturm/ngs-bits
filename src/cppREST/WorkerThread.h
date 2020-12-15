#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "cppREST_global.h"
#include <QThread>
#include <QFile>
#include <QDebug>
#include <QDir>
#include "Exceptions.h"
#include "WebEntity.h"
#include "FileCache.h"
#include "NGSD.h"
#include "SessionManager.h"

class CPPRESTSHARED_EXPORT WorkerThread : public QThread
{
	Q_OBJECT
public:
	explicit WorkerThread(Request request);
	void run();

private:	
	Response (*endpoint_action_)(Request request);
	Request request_;

signals:
	void resultReady(const Response &response);
};

#endif // WORKERTHREAD_H
