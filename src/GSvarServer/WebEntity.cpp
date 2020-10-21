#include "WebEntity.h"

WebEntity::WebEntity()
{

}

WebEntity::~WebEntity()
{

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
