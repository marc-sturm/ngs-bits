#include "EndpointHandler.h"

EndpointHandler::EndpointHandler()
{
}

EndpointHandler::~EndpointHandler()
{
}

bool EndpointHandler::isValidUser(QString name, QString password)
{
	try
	{
		NGSD db;
		QString message = db.checkPassword(name, password, true);
		if (message.isEmpty())
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	catch (DatabaseException& e)
	{
		qCritical() << e.message();
//		WebEntity::createError(WebEntity::ErrorType::INTERNAL_SERVER_ERROR, WebEntity::ContentType::TEXT_HTML, e.message());
	}
	return false;
}

QString EndpointHandler::getGSvarFile(QString sample_name, bool search_multi)
{
	QString file;
	try
	{
		//convert name to file
		NGSD db;
		QString processed_sample_id = db.processedSampleId(sample_name);		
		file = db.processedSamplePath(processed_sample_id, PathType::GSVAR);

		//determine all analyses of the sample
		QStringList analyses;
		if (QFile::exists(file)) analyses << file;

		//somatic tumor sample > ask user if he wants to open the tumor-normal pair
		QString normal_sample = db.normalSample(processed_sample_id);
		if (normal_sample!="")
		{
			analyses << db.secondaryAnalyses(sample_name + "-" + normal_sample, "somatic", true);
		}
		//check for germline trio/multi analyses
		else if (search_multi)
		{
			analyses << db.secondaryAnalyses(sample_name, "trio", true);
			analyses << db.secondaryAnalyses(sample_name, "multi sample", true);
		}

		//determine analysis to load
		if (analyses.count()==0)
		{
			qWarning() << "The GSvar file does not exist:" << file;
			return "";
		}
		else if (analyses.count()==1)
		{
			file = analyses[0];
		}
		else
		{
			bool ok = false;
			QString filename = ""; //QInputDialog::getItem(this, "Several analyses of the sample present", "select analysis:", analyses, 0, false, &ok);
			if (!ok)
			{
				return "";
			}
			file = filename;
		}
	}
	catch (Exception& e)
	{
		qWarning() << "Error opening processed sample from NGSD:" << e.message();
	}
	return file;
}

Response EndpointHandler::serveIndexPage(Request request)
{
	qInfo() << "Index page has been requested: " << request.remote_address;
	return EndpointHelper::serveStaticFile(":/assets/client/info.html", ContentType::TEXT_HTML, false);
}

Response EndpointHandler::serveApiInfo(Request request)
{
	qInfo() << "API information has been requested: " << request.remote_address;
	return EndpointHelper::serveStaticFile(":/assets/client/api.json", ContentType::APPLICATION_JSON, false);
}

Response EndpointHandler::locateFileByType(Request request)
{
	qDebug() << "File location service";
	QString path = ServerHelper::getStringSettingsValue("sample_data");
	QString found_file = getGSvarFile(request.url_params["ps"], false);

	if (found_file.isEmpty())
	{
		return WebEntity::createError(ErrorType::NOT_FOUND, request.return_type, "Could not find the sample: " + request.url_params["ps"]);
	}

	VariantList variants {};
	variants.load(found_file);

	FileLocationProviderFileSystem* file_locator = new FileLocationProviderFileSystem(found_file, variants.getSampleHeader(), variants.type());

	QList<FileLocation> file_list {};
	QJsonDocument json_doc_output {};
	QJsonArray json_list_output {};

	switch(FileLocationHelper::stringTopathType(request.url_params["type"].toLower()))
	{
		case PathType::BAM:
			file_list = file_locator->getBamFiles();
			break;
		case PathType::CNV_ESTIMATES:
			file_list = file_locator->getSegFilesCnv();
			break;
		case PathType::BAF:
			file_list = file_locator->getMantaEvidenceFiles();
			break;
		case PathType::MANTA_EVIDENCE:
			file_list = file_locator->getMantaEvidenceFiles();
			break;

		default:
			{
				QJsonObject gsvar_file_json {};
				gsvar_file_json.insert("id", request.url_params["ps"]);
				gsvar_file_json.insert("type", FileLocationHelper::pathTypeToString(PathType::GSVAR));
				gsvar_file_json.insert("filename", found_file);
				gsvar_file_json.insert("is_found", true);
				json_list_output.append(gsvar_file_json);
			}
	}

	for (int i = 0; i < file_list.count(); ++i)
	{
		qDebug() << file_list[i].filename;
		QJsonObject cur_json_item {};
		cur_json_item.insert("id", file_list[i].id);
		cur_json_item.insert("type", FileLocationHelper::pathTypeToString(file_list[i].type));
		cur_json_item.insert("filename", file_list[i].filename);
		cur_json_item.insert("is_found", file_list[i].is_found);

		json_list_output.append(cur_json_item);
	}

	json_doc_output.setArray(json_list_output);
	return Response{EndpointHelper::generateHeaders(json_doc_output.toJson().length(), ContentType::APPLICATION_JSON), json_doc_output.toJson()};
}

Response EndpointHandler::performLogin(Request request)
{
	QByteArray body {};
	if (!request.form_urlencoded.contains("name") || !request.form_urlencoded.contains("password"))
	{
		return WebEntity::createError(ErrorType::FORBIDDEN, request.return_type, "No username or/and password were found");
	}

	if (isValidUser(request.form_urlencoded["name"], request.form_urlencoded["password"]))
	{
		QString secure_token = ServerHelper::generateUniqueStr();
		Session cur_session = Session{request.form_urlencoded["name"], QDateTime::currentDateTime()};

		SessionManager::addNewSession(secure_token, cur_session);
		body = secure_token.toLocal8Bit();
		return Response{EndpointHelper::generateHeaders(body.length(), ContentType::TEXT_PLAIN), body};
	}

	return WebEntity::createError(ErrorType::UNAUTHORIZED, request.return_type, "Invalid username or password");
}

Response EndpointHandler::performLogout(Request request)
{
	QByteArray body {};
	if (!request.form_urlencoded.contains("token"))
	{
		return WebEntity::createError(ErrorType::FORBIDDEN, request.return_type, "Secure token has not been provided");
	}
	if (SessionManager::isTokenValid(request.form_urlencoded["token"]))
	{
		try
		{
			SessionManager::removeSession(request.form_urlencoded["token"]);
		} catch (Exception& e)
		{
			return WebEntity::createError(ErrorType::INTERNAL_SERVER_ERROR, request.return_type, e.message());
		}
		body = request.form_urlencoded["token"].toLocal8Bit();
		return Response{EndpointHelper::generateHeaders(body.length(), ContentType::TEXT_PLAIN), body};
	}
	return WebEntity::createError(ErrorType::FORBIDDEN, request.return_type, "You have provided an invalid token");
}
