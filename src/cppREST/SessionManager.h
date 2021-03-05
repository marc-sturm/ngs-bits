#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "cppREST_global.h"
#include <QMap>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include "ServerHelper.h"
#include "Exceptions.h"

struct CPPRESTSHARED_EXPORT Session
{
	QString user_id;
	QDateTime login_time;
};

class CPPRESTSHARED_EXPORT SessionManager
{
public:
	static void addNewSession(QString id, Session in);
	static void removeSession(QString id);
	static Session getSessionByUserId(QString id);
	static Session getSessionBySecureToken(QString token);
	static bool hasValidToken(QString id);
	static bool isTokenValid(QString token);

protected:
	SessionManager();
	~SessionManager();

private:
	QMutex mutex_;
	static bool isSessionExpired(Session in);
	static SessionManager& instance();
	QMap<QString, Session> session_store_;
};

#endif // SESSIONMANAGER_H
