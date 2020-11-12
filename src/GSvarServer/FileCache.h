#ifndef FILECACHE_H
#define FILECACHE_H

#include <QMap>
#include <QDebug>
#include <QMutex>
#include <QDateTime>

struct CacheItem
{
	QString filename_with_path;
	QDateTime modified;
	QByteArray content;
};

class FileCache
{
public:		
	static void addFileToCache(QString id, QString filename_with_path, QByteArray content);
	static void removeFileFromCache(QString id);
	static QString getFileIdIfInCache(QString filename_with_path);
	static bool isInCacheAlready(QString filename_with_path);
	static CacheItem getFileById(QString id);

protected:
	FileCache();
	~FileCache();

private:
	static FileCache& instance();
	QMutex mutex_;		
	QMap<QString, CacheItem> file_cache_;
};

#endif // FILECACHE_H
