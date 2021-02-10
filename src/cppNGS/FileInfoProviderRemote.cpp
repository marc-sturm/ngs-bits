#include "FileInfoProviderRemote.h"

FileInfoProviderRemote::FileInfoProviderRemote(QString file)
	: file_(file)
{
	server_host_ = Settings::string("server_host", false);
	server_port_ = Settings::integer("server_port");
}

QString FileInfoProviderRemote::absolutePath()
{
	QString output {};
	QJsonObject file_info = getFileInfo(file_);

	if (file_info.contains("absolute_path"))
	{
		output = file_info.value("absolute_path").toString();
	}

	return output;
}

QString FileInfoProviderRemote::absoluteFilePath()
{
	return "";
}

QString FileInfoProviderRemote::dirAbsolutePath()
{
	return "";
}

QString FileInfoProviderRemote::parentDirAbsolutePath()
{
	return "";
}

QString FileInfoProviderRemote::parentSubDirAbsolutePath(QString subdir)
{
	return "";
}

QString FileInfoProviderRemote::baseName()
{
	QString output {};
	QJsonObject file_info = getFileInfo(file_);

	if (file_info.contains("base_name"))
	{
		output = file_info.value("base_name").toString();
	}

	return output;
}

QString FileInfoProviderRemote::fileName()
{
	QString output {};
	QJsonObject file_info = getFileInfo(file_);

	if (file_info.contains("file_name"))
	{
		output = file_info.value("file_name").toString();
	}

	return output;
}

QString FileInfoProviderRemote::suffix()
{
	QString output = "";
	QJsonObject file_info = getFileInfo(file_);

	if (file_info.contains("file_name"))
	{
		QString file_name = file_info.value("file_name").toString();
		QList<QString> file_name_parts = file_name.split(".");
		if (file_name_parts.size() > 0)
		{
			output = file_name_parts[file_name_parts.size()-1];
		}
	}

	return output;
}

QDateTime FileInfoProviderRemote::lastModified()
{
	QDateTime output {};
	QJsonObject file_info = getFileInfo(file_);

	if (file_info.contains("last_modified"))
	{
		output = output.addSecs(file_info.value("last_modified").toString().toLongLong());
	}

	return output;
}

bool FileInfoProviderRemote::exists()
{
	bool output = false;
	QJsonObject file_info = getFileInfo(file_);
	qDebug() << "EXISTS = " << file_info;
	if (file_info.contains("exists"))
	{
		output = file_info.value("exists").toBool();
	}

	return output;
}

QJsonObject FileInfoProviderRemote::getFileInfo(QString file)
{
	if (file == nullptr)
	{
		THROW(ArgumentException, "File name has not been specified")
		return QJsonObject{};
	}

	QString reply = ApiRequestHandler(server_host_, server_port_).sendGetRequest("/v1/file_info?file=" + file);
	QJsonDocument json_doc = QJsonDocument::fromJson(reply.toLatin1());

	return json_doc.object();
}
