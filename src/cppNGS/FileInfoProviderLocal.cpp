#include "FileInfoProviderLocal.h"

FileInfoProviderLocal::FileInfoProviderLocal(QString file)
	: file_(file)
{
}

QString FileInfoProviderLocal::absolutePath()
{
	return QFileInfo(file_).absolutePath();
}

QString FileInfoProviderLocal::absoluteFilePath()
{
	return QFileInfo(file_).absoluteFilePath();
}

QString FileInfoProviderLocal::dirAbsolutePath()
{
	return QFileInfo(file_).dir().absolutePath();
}

QString FileInfoProviderLocal::parentDirAbsolutePath()
{
	QDir directory = QFileInfo(file_).dir();
	directory.cdUp();
	return directory.absolutePath();
}

QString FileInfoProviderLocal::parentSubDirAbsolutePath(QString subdir)
{
	QDir dir = QFileInfo(file_).dir();
	dir.cdUp();
	dir.cd(subdir);
	return dir.absolutePath();
}

QString FileInfoProviderLocal::baseName()
{
	return QFileInfo(file_).baseName();
}

QString FileInfoProviderLocal::fileName()
{
	return QFileInfo(file_).fileName();
}

QString FileInfoProviderLocal::suffix()
{
	return QFileInfo(file_).suffix();
}

QDateTime FileInfoProviderLocal::lastModified()
{
	return QFileInfo(file_).lastModified();
}

bool FileInfoProviderLocal::exists()
{
//	return QFileInfo(file_).exists();
	return QFileInfo::exists(file_);
}
