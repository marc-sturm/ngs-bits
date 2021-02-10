#include "DirInfoProviderFileSystem.h"

DirInfoProviderFileSystem::DirInfoProviderFileSystem(QString directory)
	: directory_(directory)
{
}

QString DirInfoProviderFileSystem::absolutePath()
{
	return QDir(directory_).absolutePath();
}

QString DirInfoProviderFileSystem::absoluteFilePath(QString file)
{
	return QDir(directory_).absoluteFilePath(file);
}

QString DirInfoProviderFileSystem::parentAbsolutePath()
{
	QDir current_dir(directory_);
	current_dir.cdUp();
	return current_dir.absolutePath();
}

bool DirInfoProviderFileSystem::exists()
{
	return QDir(directory_).exists();
}

QStringList DirInfoProviderFileSystem::entryList(const QStringList& nameFilters, QDir::Filters filters, QDir::SortFlags sort) const
{
	return QDir(directory_).entryList(nameFilters, filters, sort);
}
