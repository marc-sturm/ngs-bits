#ifndef REQUESTENTITY_H
#define REQUESTENTITY_H

#include <QObject>
#include <QMap>

struct Request
{
	enum MethodType
	{
		GET,
		POST,
		DELETE,
		PUT,
		PATCH
	};
	MethodType method;
	QMap<QString, QString> headers;
	QByteArray path;
	QString remote_address;
	QMap<QString, QString> payload;

};

struct Response
{
	QByteArray headers;
	QByteArray body;
};

class WebEntity : public QObject
{
	Q_OBJECT

public:
	enum ContentType
	{
		APPLICATION_OCTET_STREAM,
		APPLICATION_JSON,
		IMAGE_JPEG,
		IMAGE_PNG,
		IMAGE_SVG_XML,
		TEXT_PLAIN,
		TEXT_CSV,
		TEXT_HTML,
		MULTIPART_FORM_DATA
	};

	enum ErrorType
	{
		BAD_REQUEST,
		UNAUTHORIZED,
		PAYMENT_REQUIRED,
		FORBIDDEN,
		NOT_FOUND,
		METHOD_NOT_ALLOWED,
		NOT_ACCEPTABLE,
		PROXY_AUTH_REQUIRED,
		REQUEST_TIMEOUT,
		CONFLICT,
		GONE,
		LENGTH_REQUIRED,
		PRECONDITION_FAILED,
		ENTITY_TOO_LARGE,
		URI_TOO_LONG,
		UNSUPPORTED_MEDIA_TYPE,
		RANGE_NOT_SATISFIABLE,
		EXPECTATION_FAILED,
		INTERNAL_SERVER_ERROR,
		NOT_IMPLEMENTED,
		BAD_GATEWAY,
		SERVICE_UNAVAILABLE,
		GATEWAY_TIMEOUT,
		VERSION_NOT_SUPPORTED,
		UNKNOWN_ERROR
	};

	WebEntity();
	~WebEntity();

	static QString contentTypeToString(WebEntity::ContentType in);
	static QString errorTypeToText(WebEntity::ErrorType in);
	static int errorCodeByType(WebEntity::ErrorType in);

};

#endif // WEBENTITY_H
