#include "HttpHandler.h"
#include "Exceptions.h"
#include "Settings.h"
#include "Helper.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkProxy>
#include <QInputDialog>
#include <QApplication>
#include <QAuthenticator>
#include <QFile>
#include <QPointer>
#include <QHttpMultiPart>

HttpHandler::HttpHandler(HttpRequestHandler::ProxyType proxy_type, QObject* parent)
	: QObject(parent)
//	, nmgr_()
//	, headers_()
	, proxy_type_(proxy_type)
{

	//default headers
//	setHeader("User-Agent", "GSvar");
//	setHeader("X-Custom-User-Agent", "GSvar");

	//proxy
//	if (proxy_type==HttpRequestHandler::SYSTEM)
//	{
//		QNetworkProxyFactory::setUseSystemConfiguration(true);
//	}
//	else if (proxy_type==HttpRequestHandler::INI)
//	{
//		QNetworkProxy proxy;
//		proxy.setType(QNetworkProxy::HttpProxy);
//		proxy.setHostName(Settings::string("proxy_host"));
//		proxy.setPort(Settings::integer("proxy_port"));
//		nmgr_.setProxy(proxy);
//	}
//	else
//	{
//		nmgr_.setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
//	}

	//signals+slots
//	connect(&nmgr_, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> &)), this, SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError>&)));
	connect(&nmgr_, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy& , QAuthenticator*)), this, SLOT(handleProxyAuthentification(const QNetworkProxy& , QAuthenticator*)));
}

const HttpHeaders& HttpHandler::headers() const
{
	return headers_;
}

void HttpHandler::setHeader(const QByteArray& key, const QByteArray& value)
{
	headers_.insert(key, value);
}

QString HttpHandler::get(QString url, const HttpHeaders& add_headers)
{	
	return HttpRequestHandler(proxy_type_, this).get(url, add_headers);
}

QString HttpHandler::post(QString url, const QByteArray& data, const HttpHeaders& add_headers)
{	
	return HttpRequestHandler(proxy_type_, this).post(url, data, add_headers);
}

QString HttpHandler::post(QString url, QHttpMultiPart* parts, const HttpHeaders& add_headers)
{
	return HttpRequestHandler(proxy_type_, this).post(url, parts, add_headers);
}

void HttpHandler::handleProxyAuthentification(const QNetworkProxy& proxy, QAuthenticator* auth)
{	
	QString proxy_user = QInputDialog::getText(QApplication::activeWindow(), "Proxy user required", "Proxy user for " + proxy.hostName());
	auth->setUser(proxy_user);
	QString proxy_pass = QInputDialog::getText(QApplication::activeWindow(), "Proxy password required", "Proxy password for " + proxy.hostName(), QLineEdit::Password);
	auth->setPassword(proxy_pass);

	nmgr_.setProxy(proxy);
}

