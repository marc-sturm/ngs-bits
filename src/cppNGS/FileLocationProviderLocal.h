#ifndef FILELOCATIONPROVIDERLOCAL_H
#define FILELOCATIONPROVIDERLOCAL_H

#include "cppNGS_global.h"
#include "FileLocationProvider.h"

class CPPNGSSHARED_EXPORT FileLocationProviderLocal : virtual public FileLocationProvider
{
public:
	FileLocationProviderLocal(QString gsvar_file, const SampleHeaderInfo header_info, const AnalysisType analysis_type);
	virtual ~FileLocationProviderLocal() {}

	QList<FileLocation> getBamFiles() override;
	QList<FileLocation> getSegFilesCnv() override;
	QList<FileLocation> getIgvFilesBaf() override;
	QList<FileLocation> getMantaEvidenceFiles() override;

	QList<FileLocation> getAnalysisLogFiles() override;
	QList<FileLocation> getCircosPlotFiles() override;
	QList<FileLocation> getVcfGzFiles() override;
	QList<FileLocation> getExpansionhunterVcfFiles() override;
	QList<FileLocation> getPrsTsvFiles() override;
	QList<FileLocation> getClincnvTsvFiles() override;
	QList<FileLocation> getLowcovBedFiles() override;

	QString processedSampleName();

private:
	QList<FileLocation> mapFoundFilesToFileLocation(QStringList& files, PathType type);
	void setIsFoundFlag(FileLocation& file);

protected:
	QString gsvar_file_;
	SampleHeaderInfo header_info_;
	AnalysisType analysis_type_;
};


#endif // FILELOCATIONPROVIDERLOCAL_H
