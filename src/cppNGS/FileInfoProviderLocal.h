#ifndef FILEINFOPROVIDERLOCAL_H
#define FILEINFOPROVIDERLOCAL_H

#include <QDateTime>
#include <QDir>
#include "cppNGS_global.h"
#include "FileInfoProvider.h"

class CPPNGSSHARED_EXPORT FileInfoProviderLocal : virtual public FileInfoProvider
{
public:
	FileInfoProviderLocal(QString file);
	virtual ~FileInfoProviderLocal() {}

	QString absolutePath() override;
	QString absoluteFilePath() override;
	QString dirAbsolutePath() override;
	QString parentDirAbsolutePath() override;
	QString parentSubDirAbsolutePath(QString subdir) override;
	QString baseName() override;
	QString fileName() override;
	QString suffix() override;
	QDateTime lastModified() override;
	bool exists() override;
private:
	QString file_;
};

#endif // FILEINFOPROVIDERLOCAL_H
