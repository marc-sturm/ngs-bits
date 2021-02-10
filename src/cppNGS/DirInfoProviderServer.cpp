#include "DirInfoProviderServer.h"

DirInfoProviderServer::DirInfoProviderServer(QString directory)
	: directory_(directory)
{
	server_host_ = Settings::string("server_host", false);
	server_port_ = Settings::integer("server_port");
}

QString DirInfoProviderServer::absolutePath()
{
	return "";
}

QString DirInfoProviderServer::absoluteFilePath(QString file)
{
	return "";
}

QString DirInfoProviderServer::parentAbsolutePath()
{
	return "";
}

bool DirInfoProviderServer::exists()
{
	return true;
}

QStringList DirInfoProviderServer::entryList(const QStringList& nameFilters, QDir::Filters filters, QDir::SortFlags sort) const
{
	return QStringList{};
}
