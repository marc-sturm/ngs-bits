#include "FileLocationProviderServer.h"

FileLocationProviderServer::FileLocationProviderServer(const QString sample_id,const QString server_host, const int server_port)
	: sample_id_(sample_id)
	, server_host_(server_host)
	, server_port_(server_port)
{	
}

QString FileLocationProviderServer::sendGetRequest(QString path)
{
	QString reply {};

	if (server_host_.isEmpty())
	{
		THROW(ArgumentException, "Server host has not been specified in the config")
		return reply;
	}

	if (server_port_ == 0)
	{
		THROW(ArgumentException, "Server port has not been specified in the config")
		return reply;
	}

	try
	{
		HttpHeaders add_headers;
		add_headers.insert("Accept", "application/json");
		reply = HttpRequestHandler(HttpRequestHandler::NONE).get("https://" + server_host_ + ":" + QString::number(server_port_) + path);
	}
	catch(Exception& e)
	{
		qDebug() << e.message();
	}

	return reply;
}

QList<FileLocation> FileLocationProviderServer::requestFileInfoByType(PathType type)
{
	QList<FileLocation> output {};
	if (sample_id_ == nullptr)
	{
		THROW(ArgumentException, "File name has not been specified")
		return output;
	}

	QString reply = sendGetRequest("/v1/file_location?ps=" + sample_id_ + "&type=" + FileLocationHelper::pathTypeToString(type));
	QJsonDocument json_doc = QJsonDocument::fromJson(reply.toLatin1());
	QJsonArray file_list = json_doc.array();

	if (file_list.count() == 0)
	{
		THROW(Exception, "Could not find file info: " + FileLocationHelper::pathTypeToString(type));
	}

	qDebug() << "Requested files:" << file_list;
	output = mapJsonArrayToFileLocationList(file_list);
	return output;
}

FileLocation FileLocationProviderServer::mapJsonObjectToFileLocation(QJsonObject obj)
{
	return FileLocation {
		obj.value("id").toString(),
		FileLocationHelper::stringTopathType(obj.value("type").toString()),
		obj.value("filename").toString(),
		obj.value("is_found").toBool()
	};
}

QList<FileLocation> FileLocationProviderServer::mapJsonArrayToFileLocationList(QJsonArray array)
{
	QList<FileLocation> output {};
	for (int i = 0; i < array.count(); ++i)
	{
		output.append(mapJsonObjectToFileLocation(array[i].toObject()));
	}
	return output;
}

QList<FileLocation> FileLocationProviderServer::getBamFiles()
{
	return requestFileInfoByType(PathType::BAM);
}

QList<FileLocation> FileLocationProviderServer::getSegFilesCnv()
{
	return requestFileInfoByType(PathType::CNV_ESTIMATES);
}

QList<FileLocation> FileLocationProviderServer::getIgvFilesBaf()
{
	return requestFileInfoByType(PathType::BAF);
}

QList<FileLocation> FileLocationProviderServer::getMantaEvidenceFiles()
{
	return requestFileInfoByType(PathType::MANTA_EVIDENCE);
}
