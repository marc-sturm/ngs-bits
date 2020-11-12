#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "HttpsServer.h"
#include "ServerHelper.h"
#include "EndpointManager.h"
#include "EndpointHandler.h"

QFile gsvar_server_log_file("gsvar-server-log.txt");

void interceptLogMessage(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
	QString time_stamp = QTime::currentTime().toString("hh:mm:ss:zzz");
	QString log_statement;
	int msg_level = 0;
	switch (type) {
		case QtCriticalMsg:
			msg_level = 0;
			log_statement = QString("%1 - [Critical] %2").arg(time_stamp, msg);
			break;
		case QtFatalMsg:
			msg_level = 0;
			log_statement = QString("%1 - [Fatal] %2").arg(time_stamp, msg);
			break;
		case QtInfoMsg:
			msg_level = 1;
			log_statement = QString("%1 - [Info] %2").arg(time_stamp, msg);
			break;
		case QtWarningMsg:
			msg_level = 2;
			log_statement = QString("%1 - [Warning] %2").arg(time_stamp, msg);
			break;
		case QtDebugMsg:
			msg_level = 3;
			log_statement = QString("%1 - [Debug] %2").arg(time_stamp, msg);
			break;
		default:
			return;
	}

	// Log levels:
	// 0: only critical and fatal
	// 1: += info
	// 2: += warning
	// 3: += debug
	int log_level = Settings::integer("log_level");
	if (msg_level <= log_level)
	{
		printf("%s", qUtf8Printable(log_statement));
		printf("\n");

		QTextStream out_stream(&gsvar_server_log_file);
		out_stream.setCodec("UTF-8");
		out_stream.setGenerateByteOrderMark(false);
		out_stream << log_statement << endl;
	}

	if (type == QtFatalMsg)
	{
		abort();
	}
}

int main(int argc, char **argv)
{
	gsvar_server_log_file.open(QIODevice::WriteOnly | QIODevice::Append);
	qInstallMessageHandler(interceptLogMessage);
	QCoreApplication app(argc, argv);

	EndpointManager::appendEndpoint(Endpoint{
						"",
						QMap<QString, ParamProps>{},
						Request::MethodType::GET,
						ContentType::TEXT_HTML,
						"Index page with general information",
						&EndpointHandler::serveIndexPage
					});
	EndpointManager::appendEndpoint(Endpoint{
						"info",
						QMap<QString, ParamProps>{},
						Request::MethodType::GET,
						ContentType::APPLICATION_JSON,
						"General information about this API",
						&EndpointHandler::serveApiInfo
					});
	EndpointManager::appendEndpoint(Endpoint{
						"static",
						QMap<QString, ParamProps>{
						   {"filename", ParamProps{ParamProps::ParamType::STRING, ParamProps::ParamCategory::PATH_PARAM, false, "Name of the file to be served"}}
						},
						Request::MethodType::GET,
						ContentType::TEXT_HTML,
						"Static content served from the server root folder (defined in the config file)",
						&EndpointHandler::serveStaticFile
				   });

	EndpointManager::appendEndpoint(Endpoint{
						"help",
						QMap<QString, ParamProps>{
						   {"endpoint", ParamProps{ParamProps::ParamType::STRING, ParamProps::ParamCategory::PATH_PARAM, true,
							"Endpoint path the help is requested for. Help for all endpoints wiil be provided, if this parameter is ommited"}}
						},
						Request::MethodType::GET,
						ContentType::TEXT_HTML,
						"Help page on the usage of the endpoints",
						&EndpointHandler::serveEndpointHelp
					});

	EndpointManager::appendEndpoint(Endpoint{
						"file_location",
						QMap<QString, ParamProps> {
						   {"ps", ParamProps{ParamProps::ParamType::STRING, ParamProps::ParamCategory::GET_URL_PARAM, false, "Sample id"}},
						   {"type", ParamProps{ParamProps::ParamType::STRING, ParamProps::ParamCategory::GET_URL_PARAM, true, "Format of the requested file(s)"}}
						},
						Request::MethodType::GET,
						ContentType::APPLICATION_JSON,
						"Retrieve file location information for scecific file types",
						&EndpointHandler::locateFileByType
					});

	EndpointManager::appendEndpoint(Endpoint{
						"login",
						QMap<QString, ParamProps>{
							{"name", ParamProps{ParamProps::ParamType::STRING, ParamProps::ParamCategory::POST_URL_ENCODED, false, "User name"}},
							{"password", ParamProps{ParamProps::ParamType::STRING, ParamProps::ParamCategory::POST_URL_ENCODED, false, "Password"}}
						},
						Request::MethodType::POST,
						ContentType::TEXT_PLAIN,
						"Secure token generation, the token will be used to access protected resources and to perform  certain API calls",
						&EndpointHandler::performLogin
					});
	EndpointManager::appendEndpoint(Endpoint{
						"logout",
						QMap<QString, ParamProps>{
							{"token", ParamProps{ParamProps::ParamType::STRING, ParamProps::ParamCategory::POST_URL_ENCODED, false, "Secure token received after a successful login"}}
						},
						Request::MethodType::POST,
						ContentType::TEXT_PLAIN,
						"Secure token invalidation, after this step the token cannot longer be used",
						&EndpointHandler::performLogout
					});

	HttpsServer sslserver(ServerHelper::getNumSettingsValue("server_port"));

	return app.exec();
}
