#ifndef REQUESTENTITY_H
#define REQUESTENTITY_H

#include <QMap>
#include <QUuid>
#include <QDebug>
#include <QDateTime>

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
	QMap<QString, QString> url_params;
	QMap<QString, QString> form_urlencoded;
};

struct Response
{
	QByteArray headers;
	QByteArray body;
};

struct FolderItem
{
	QString name;
	bool is_folder;
	int size;
	QDateTime modified;
};

class WebEntity
{
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

	enum FolderItemIcon
	{
		TO_PARENT_FOLDER,
		GENERIC_FILE,
		BINARY_FILE,
		CODE_FILE,
		PICTURE_FILE,
		TEXT_FILE,
		TABLE_FILE,
		FOLDER
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



	static QString contentTypeToString(WebEntity::ContentType in);
	static QString folderItemIconToString(WebEntity::FolderItemIcon in);
	static QString errorTypeToText(WebEntity::ErrorType in);
	static int errorCodeByType(WebEntity::ErrorType in);
	static QString generateToken();

	static QString getErrorPageTemplate();
	static Response createError(WebEntity::ErrorType type, QString message);
	static Response cretateFolderListing(QList<FolderItem> in);


private:
	WebEntity();
	static WebEntity& instance();

	const QList<QString> BINARY_EXT = {"bam", "exe"};
	const QList<QString> CODE_EXT = {"xml", "html", "yml"};
	const QList<QString> PICTURE_EXT = {"jpg", "jpeg", "png", "gif", "svg"};
	const QList<QString> TEXT_EXT = {"txt", "ini", "rtf", "doc", "docx"};
	const QList<QString> TABLE_EXT = {"csv", "xls", "xlsx"};

	static QString getPageHeader();
	static QString getPageFooter();
	static QString getFolderIcons();
	static FolderItemIcon getIconType(FolderItem item);
	static QString createFolderItemLink(QString name, QString url, WebEntity::FolderItemIcon type);
};

#endif // WEBENTITY_H
