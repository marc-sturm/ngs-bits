#include "WebEntity.h"

WebEntity::WebEntity()
{
}

WebEntity& WebEntity::instance()
{
	static WebEntity web_entity;
	return web_entity;
}

QString WebEntity::contentTypeToString(WebEntity::ContentType in)
{
	switch(in)
	{
		case APPLICATION_OCTET_STREAM: return "application/octet-stream";
		case APPLICATION_JSON: return "application/json";
		case IMAGE_JPEG: return "image/jpeg";
		case IMAGE_PNG: return "image/png";
		case IMAGE_SVG_XML: return "image/svg+xml";
		case TEXT_PLAIN: return "text/plain";
		case TEXT_CSV: return "text/csv";
		case TEXT_HTML: return "text/html";
		case MULTIPART_FORM_DATA: return "multipart/form-data";
	}
	return "";
}

QString WebEntity::folderItemIconToString(WebEntity::FolderItemIcon in)
{
	switch(in)
	{
		case TO_PARENT_FOLDER: return "up_dir";
		case GENERIC_FILE: return "generic_file";
		case BINARY_FILE: return "binary_file";
		case CODE_FILE: return "code_file";
		case PICTURE_FILE: return "picture_file";
		case TEXT_FILE: return "text_file";
		case TABLE_FILE: return "table_file";
		case FOLDER: return "folder";
	};
	return "";
}

QString WebEntity::errorTypeToText(WebEntity::ErrorType in)
{
	switch(in)
	{
		case BAD_REQUEST: return "Bad Request";
		case UNAUTHORIZED: return "Unauthorized";
		case PAYMENT_REQUIRED: return "Payment Required";
		case FORBIDDEN: return "Forbidden";
		case NOT_FOUND: return "Not Found";
		case METHOD_NOT_ALLOWED: return "Method Not Allowed";
		case NOT_ACCEPTABLE: return "Not Acceptable";
		case PROXY_AUTH_REQUIRED: return "Proxy Authentication Required";
		case REQUEST_TIMEOUT: return "Request Timeout";
		case CONFLICT: return "Conflict";
		case GONE: return "Gone";
		case LENGTH_REQUIRED: return "Length Required";
		case PRECONDITION_FAILED: return "Precondition Failed";
		case ENTITY_TOO_LARGE: return "Request Entity Too Large";
		case URI_TOO_LONG: return "Request-URI Too Long";
		case UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
		case RANGE_NOT_SATISFIABLE: return "Requested Range Not Satisfiable";
		case EXPECTATION_FAILED: return "Expectation Failed";
		case INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case NOT_IMPLEMENTED: return "Not Implemented";
		case BAD_GATEWAY: return "Bad Gateway";
		case SERVICE_UNAVAILABLE: return "Service Unavailable";
		case GATEWAY_TIMEOUT: return "Gateway Timeout";
		case VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
		case UNKNOWN_ERROR:
		default: return "Unknown Error";
	}
}

int WebEntity::errorCodeByType(WebEntity::ErrorType in)
{
	switch(in)
	{
		case BAD_REQUEST: return 400;
		case UNAUTHORIZED: return 401;
		case PAYMENT_REQUIRED: return 402;
		case FORBIDDEN: return 403;
		case NOT_FOUND: return 404;
		case METHOD_NOT_ALLOWED: return 405;
		case NOT_ACCEPTABLE: return 406;
		case PROXY_AUTH_REQUIRED: return 407;
		case REQUEST_TIMEOUT: return 408;
		case CONFLICT: return 409;
		case GONE: return 410;
		case LENGTH_REQUIRED: return 411;
		case PRECONDITION_FAILED: return 412;
		case ENTITY_TOO_LARGE: return 413;
		case URI_TOO_LONG: return 414;
		case UNSUPPORTED_MEDIA_TYPE: return 415;
		case RANGE_NOT_SATISFIABLE: return 416;
		case EXPECTATION_FAILED: return 417;
		case INTERNAL_SERVER_ERROR: return 500;
		case NOT_IMPLEMENTED: return 501;
		case BAD_GATEWAY: return 502;
		case SERVICE_UNAVAILABLE: return 503;
		case GATEWAY_TIMEOUT: return 504;
		case VERSION_NOT_SUPPORTED: return 505;
		case UNKNOWN_ERROR:
		default: return 0;
	}
}

QString WebEntity::generateToken()
{
	return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString WebEntity::getPageHeader()
{
	QString output;
	QTextStream stream(&output);

	stream << "<!doctype html>\n";
	stream << "<html lang=\"en\">\n";
	stream << "		<head>\n";
	stream << "			<meta charset=\"utf-8\">\n";
	stream << "			<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">\n";
	stream << "			<title>%TITLE%</title>\n";
	stream << "			<style>\n";
	stream << "				html, body {\n";
	stream << "					padding: 10px;\n";
	stream << "					height: 100%;\n";
	stream << "				}\n";
	stream << "				hr {\n";
	stream << "					margin-top: 10px\n";
	stream << "				}\n";

	stream << "				* {\n";
	stream << "					box-sizing: border-box;\n";
	stream << "				}\n";
	stream << "				.column {\n";
	stream << "					float: left;\n";
	stream << "					width: 33.33%;\n";
	stream << "					padding: 10px;\n";
	stream << "				}\n";
	stream << "				.row:after {\n";
	stream << "					content: \"\";\n";
	stream << "					display: table;\n";
	stream << "					clear: both;\n";
	stream << "				}\n";

	stream << "				.file-list {\n";
	stream << "					display: inline-block;\n";
	stream << "					vertical-align: middle;\n";
	stream << "					padding-bottom: 5px;\n";
	stream << "				}\n";
	stream << "				.file-list svg {\n";
	stream << "					margin-right: 5px;\n";
	stream << "				}\n";
	stream << "				.centered {\n";
	stream << "					text-align: center;\n";
	stream << "				}\n";
	stream << "				.main-content {\n";
	stream << "					min-height: 100%;\n";
	stream << "					min-height: 100vh;\n";
	stream << "					display: -webkit-box;\n";
	stream << "					display: -moz-box;\n";
	stream << "					display: -ms-flexbox;\n";
	stream << "					display: -webkit-flex;\n";
	stream << "					display: flex;\n";
	stream << "					-webkit-box-align: center;\n";
	stream << "					-webkit-align-items: center;\n";
	stream << "					-moz-box-align: center;\n";
	stream << "					-ms-flex-align: center;\n";
	stream << "					align-items: center;\n";
	stream << "					width: 100%;\n";
	stream << "					-webkit-box-pack: center;\n";
	stream << "					-moz-box-pack: center;\n";
	stream << "					-ms-flex-pack: center;\n";
	stream << "					-webkit-justify-content: center;\n";
	stream << "					justify-content: center;\n";
	stream << "				}\n";
	stream << "				.data-container {\n";
	stream << "					width: 640px;\n";
	stream << "				}\n";
	stream << "				pre {\n";
	stream << "					font-size: 14px;\n";
	stream << "				}\n";
	stream << "			</style>\n";
	stream << "		</head>\n";
	stream << "		<body>\n";
	return output;
}

QString WebEntity::getPageFooter()
{
	QString output;
	QTextStream stream(&output);
	stream << "		</body>\n";
	stream << "</html>\n";
	return output;
}

QString WebEntity::getErrorPageTemplate()
{
	QString output;
	QTextStream stream(&output);

	stream << getPageHeader();
	stream << "			<div class=\"main-content\">\n";
	stream << "				<div class=\"data-container\">\n";
	stream << "					<h1 class=\"centered\">%TITLE%</h1>\n";
	stream << "					<p>An error has occurred. Below you will find a short summary\n";
	stream << "					that may help to fix it or to prevent it from happening:</p>\n";
	stream << "					<pre>%MESSAGE%</pre>\n";
	stream << "					<pre class=\"centered\">\n";
	stream << "O       o O       o O       o\n";
	stream << "| O   o | | O   o | | O   o |\n";
	stream << "| | O | | | | O | | | | O | |\n";
	stream << "| o   O | | o   O | | o   O |\n";
	stream << "o       O o       O o       O\n";
	stream << "					</pre>\n";
	stream << "				</div>\n";
	stream << "			</div>\n";
	stream << getPageFooter();

	return output;
}

QString WebEntity::getFolderIcons()
{
	QString output;
	QTextStream stream(&output);

	stream << "			<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"2em\" height=\"2em\" style=\"display: none;\" viewBox=\"0 0 16 16\">\n";
	stream << "				<symbol id=\"up_dir\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path fill-rule=\"evenodd\" d=\"M4.854 1.146a.5.5 0 0 0-.708 0l-4 4a.5.5 0 1 0 .708.708L4 2.707V12.5A2.5 2.5 0 0 0 6.5 15h8a.5.5 0 0 0 0-1h-8A1.5 1.5 0 0 1 5 12.5V2.707l3.146 3.147a.5.5 0 1 0 .708-.708l-4-4z\"/>\n";
	stream << "				</symbol>\n";
	stream << "				<symbol id=\"generic_file\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path d=\"M4 0h5.5v1H4a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h8a1 1 0 0 0 1-1V4.5h1V14a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V2a2 2 0 0 1 2-2z\"/>\n";
	stream << "					<path d=\"M9.5 3V0L14 4.5h-3A1.5 1.5 0 0 1 9.5 3z\"/>\n";
	stream << "				</symbol>\n";
	stream << "				<symbol id=\"binary_file\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path fill-rule=\"evenodd\" d=\"M4 0h8a2 2 0 0 1 2 2v12a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V2a2 2 0 0 1 2-2zm0 1a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h8a1 1 0 0 0 1-1V2a1 1 0 0 0-1-1H4z\"/>\n";
	stream << "					<path d=\"M5.526 13.09c.976 0 1.524-.79 1.524-2.205 0-1.412-.548-2.203-1.524-2.203-.978 0-1.526.79-1.526 2.203 0 1.415.548 2.206 1.526 2.206zm-.832-2.205c0-1.05.29-1.612.832-1.612.358 0 .607.247.733.721L4.7 11.137a6.749 6.749 0 0 1-.006-.252zm.832 1.614c-.36 0-.606-.246-.732-.718l1.556-1.145c.003.079.005.164.005.249 0 1.052-.29 1.614-.829 1.614zm5.329.501v-.595H9.73V8.772h-.69l-1.19.786v.688L8.986 9.5h.05v2.906h-1.18V13h3z\"/>\n";
	stream << "				</symbol>\n";
	stream << "				<symbol id=\"code_file\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path d=\"M4 0h5.5v1H4a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h8a1 1 0 0 0 1-1V4.5h1V14a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V2a2 2 0 0 1 2-2z\"/>\n";
	stream << "					<path d=\"M9.5 3V0L14 4.5h-3A1.5 1.5 0 0 1 9.5 3z\"/>\n";
	stream << "					<path fill-rule=\"evenodd\" d=\"M8.646 6.646a.5.5 0 0 1 .708 0l2 2a.5.5 0 0 1 0 .708l-2 2a.5.5 0 0 1-.708-.708L10.293 9 8.646 7.354a.5.5 0 0 1 0-.708zm-1.292 0a.5.5 0 0 0-.708 0l-2 2a.5.5 0 0 0 0 .708l2 2a.5.5 0 0 0 .708-.708L5.707 9l1.647-1.646a.5.5 0 0 0 0-.708z\"/>\n";
	stream << "				</symbol>\n";
	stream << "				<symbol id=\"picture_file\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path fill-rule=\"evenodd\" d=\"M12 16a2 2 0 0 0 2-2V4.5L9.5 0H4a2 2 0 0 0-2 2v12a2 2 0 0 0 2 2h8zM3 2a1 1 0 0 1 1-1h5.5v2A1.5 1.5 0 0 0 11 4.5h2V10l-2.083-2.083a.5.5 0 0 0-.76.063L8 11 5.835 9.7a.5.5 0 0 0-.611.076L3 12V2z\"/>\n";
	stream << "					<path fill-rule=\"evenodd\" d=\"M6.502 7a1.5 1.5 0 1 0 0-3 1.5 1.5 0 0 0 0 3z\"/>\n";
	stream << "				</symbol>\n";
	stream << "				<symbol id=\"text_file\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path d=\"M4 0h5.5v1H4a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h8a1 1 0 0 0 1-1V4.5h1V14a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V2a2 2 0 0 1 2-2z\"/>\n";
	stream << "					<path d=\"M9.5 3V0L14 4.5h-3A1.5 1.5 0 0 1 9.5 3z\"/>\n>";
	stream << "					<path fill-rule=\"evenodd\" d=\"M5 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm0-2a.5.5 0 0 1 .5-.5h5a.5.5 0 0 1 0 1h-5a.5.5 0 0 1-.5-.5zm0-2a.5.5 0 0 1 .5-.5h5a.5.5 0 0 1 0 1h-5a.5.5 0 0 1-.5-.5z\"/>\n";
	stream << "				</symbol>\n";
	stream << "				<symbol id=\"table_file\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path fill-rule=\"evenodd\" d=\"M5 10H3V9h10v1h-3v2h3v1h-3v2H9v-2H6v2H5v-2H3v-1h2v-2zm1 0v2h3v-2H6z\"/>\n";
	stream << "					<path d=\"M4 0h5.5v1H4a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h8a1 1 0 0 0 1-1V4.5h1V14a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V2a2 2 0 0 1 2-2z\"/>\n";
	stream << "					<path d=\"M9.5 3V0L14 4.5h-3A1.5 1.5 0 0 1 9.5 3z\"/>\n";
	stream << "				</symbol>\n";
	stream << "				<symbol id=\"folder\" viewBox=\"0 0 16 16\">\n";
	stream << "					<path d=\"M9.828 4a3 3 0 0 1-2.12-.879l-.83-.828A1 1 0 0 0 6.173 2H2.5a1 1 0 0 0-1 .981L1.546 4h-1L.5 3a2 2 0 0 1 2-2h3.672a2 2 0 0 1 1.414.586l.828.828A2 2 0 0 0 9.828 3v1z\"/>\n";
	stream << "					<path fill-rule=\"evenodd\" d=\"M13.81 4H2.19a1 1 0 0 0-.996 1.09l.637 7a1 1 0 0 0 .995.91h10.348a1 1 0 0 0 .995-.91l.637-7A1 1 0 0 0 13.81 4zM2.19 3A2 2 0 0 0 .198 5.181l.637 7A2 2 0 0 0 2.826 14h10.348a2 2 0 0 0 1.991-1.819l.637-7A2 2 0 0 0 13.81 3H2.19z\"/>\n";
	stream << "				</symbol>\n";
	stream << "			</svg>\n";

	return output;
}

WebEntity::FolderItemIcon WebEntity::getIconType(FolderItem item)
{
	if (item.is_folder) return FolderItemIcon::FOLDER;

	QString found_extention = "";
	QList<QString> name_items = item.name.split(".");
	if (name_items.count()>0) found_extention = name_items[name_items.count()-1];

	if (WebEntity::instance().BINARY_EXT.indexOf(found_extention)>-1) return FolderItemIcon::BINARY_FILE;
	if (WebEntity::instance().CODE_EXT.indexOf(found_extention)>-1) return FolderItemIcon::CODE_FILE;
	if (WebEntity::instance().PICTURE_EXT.indexOf(found_extention)>-1) return FolderItemIcon::PICTURE_FILE;
	if (WebEntity::instance().TEXT_EXT.indexOf(found_extention)>-1) return FolderItemIcon::TEXT_FILE;
	if (WebEntity::instance().TABLE_EXT.indexOf(found_extention)>-1) return FolderItemIcon::TABLE_FILE;

	return FolderItemIcon::GENERIC_FILE;
}

QString WebEntity::createFolderItemLink(QString name, QString url, WebEntity::FolderItemIcon type)
{
	return "<a class=\"file-list\" href=\"" +url + "\"><svg class=\"file-list\" width=\"2em\" height=\"2em\"><use xlink:href=\"#" + folderItemIconToString(type) + "\" /></svg> <span>" + name + "</span></a>";
}

Response WebEntity::createError(WebEntity::ErrorType type, QString message)
{
	qDebug() << "An error has been detected:" << message;

	QByteArray headers {};
	QString caption = errorTypeToText(type);
	QString body = getErrorPageTemplate();

	if (message.isEmpty())
	{
		message	= "No information provided";
	}
	body.replace("%TITLE%", "Error " + QString::number(errorCodeByType(type)) + " - " + errorTypeToText(type));
	body.replace("%MESSAGE%", message);

	headers.append("HTTP/1.1 " + QString::number(errorCodeByType(type)) + " FAIL\n");
	headers.append("Content-Length: " + QString::number(body.length()) + "\n");
	headers.append("\n");

	return Response{headers, body.toLocal8Bit()};
}

Response WebEntity::cretateFolderListing(QList<FolderItem> in)
{
	QString output;
	QTextStream stream(&output);

	QString folder_name = "Folder name";

	stream << getPageHeader();
	stream << getFolderIcons();

	stream << "			<h1>" << folder_name << "</h1><br />\n";
	stream << "			<div class=\"row\">\n";
	stream << "				<div class=\"column\">" << createFolderItemLink("to the parent folder", "", FolderItemIcon::TO_PARENT_FOLDER) << "</div>\n";
	stream << "				<div class=\"column\"><b>Size</b></div>\n";
	stream << "				<div class=\"column\"><b>Modified</b></div>\n";
	stream << "			</div>\n";

	for (int i = 0; i < in.count(); ++i)
	{
		stream << "			<div class=\"row\">\n";
		stream << "				<div class=\"column\">" << createFolderItemLink(in[i].name, "", getIconType(in[i])) << "</div>\n";
		stream << "				<div class=\"column\">" << in[i].size << "</div>\n";
		stream << "				<div class=\"column\">" << in[i].modified.toString() << "</div>\n";
		stream << "			</div>\n";
	}

	stream << "<hr />\n";
	stream << "GSvarServer v.1.0\n";

	stream << getPageFooter();


	QByteArray headers {};
	headers.append("HTTP/1.1 200 OK\n");
	headers.append("Content-Length: " + QString::number(output.length()) + "\n");
	headers.append("Content-Type: " + WebEntity::contentTypeToString(ContentType::TEXT_HTML) + "\n");
	headers.append("\n");

	return Response{headers, output.toLocal8Bit()};
}
