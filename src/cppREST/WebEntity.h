#ifndef REQUESTENTITY_H
#define REQUESTENTITY_H

#include "cppREST_global.h"
#include <QMap>
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

typedef enum
{
	APPLICATION_OCTET_STREAM,
	APPLICATION_JSON,
	APPLICATION_JAVASCRIPT,
	IMAGE_JPEG,
	IMAGE_PNG,
	IMAGE_SVG_XML,
	TEXT_PLAIN,
	TEXT_CSV,
	TEXT_HTML,
	TEXT_XML,
	TEXT_CSS,
	MULTIPART_FORM_DATA
} ContentType;

typedef enum
{
	TO_PARENT_FOLDER,
	GENERIC_FILE,
	BINARY_FILE,
	CODE_FILE,
	PICTURE_FILE,
	TEXT_FILE,
	TABLE_FILE,
	FOLDER
} FolderItemIcon;

typedef enum
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
} ErrorType;

struct CPPRESTSHARED_EXPORT Request
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
	ContentType return_type;
	QMap<QString, QString> headers;
	QString prefix;
	QString path;
	QString remote_address;
	QMap<QString, QString> url_params;
	QMap<QString, QString> form_urlencoded;
	QList<QString> path_params;
};

struct CPPRESTSHARED_EXPORT Response
{
	QByteArray headers;
	QByteArray body;
};

struct CPPRESTSHARED_EXPORT FolderItem
{
	QString name;
	bool is_folder;
	int size;
	QDateTime modified;
};

class CPPRESTSHARED_EXPORT WebEntity
{
public:
	static ContentType getContentTypeFromString(QString in);
	static Request::MethodType getMethodTypeFromString(QString in);
	static QString convertMethodTypeToString(Request::MethodType in);

	static QString convertContentTypeToString(ContentType in);
	static ContentType getContentTypeByFilename(QString filename);
	static QString convertIconNameToString(FolderItemIcon in);
	static QString convertErrorTypeToText(ErrorType in);
	static int getErrorCodeByType(ErrorType in);

	static QString getErrorPageTemplate();
	static Response createError(ErrorType error_type, ContentType content_type, QString message);
	static Response cretateFolderListing(QList<FolderItem> in);

	static QString getPageHeader();
	static QString getPageFooter();

	static QString getApiHelpHeader(QString title);
	static QString getApiHelpEntiry(QString url, QString method, QList<QString> param_names, QList<QString> param_desc, QString comment);

	static QString getUrlWithoutParams(QString url);

protected:
	WebEntity();
	~WebEntity();

private:	
	static WebEntity& instance();

	const QList<QString> BINARY_EXT = {"bam", "exe"};
	const QList<QString> CODE_EXT = {"xml", "html", "yml"};
	const QList<QString> PICTURE_EXT = {"jpg", "jpeg", "png", "gif", "svg"};
	const QList<QString> TEXT_EXT = {"txt", "ini", "rtf", "doc", "docx"};
	const QList<QString> TABLE_EXT = {"csv", "xls", "xlsx"};

	static QString getFolderIcons();
	static FolderItemIcon getIconType(FolderItem item);
	static QString createFolderItemLink(QString name, QString url, FolderItemIcon type);
};

#endif // WEBENTITY_H
