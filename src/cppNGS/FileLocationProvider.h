#ifndef FILELOCATIONPROVIDER_H
#define FILELOCATIONPROVIDER_H
#include "Exceptions.h"

#include "cppNGS_global.h"
#include "VariantList.h"
#include "qfileinfo.h"
#include "Helper.h"
#include "FileLocationHelper.h"

class CPPNGSSHARED_EXPORT FileLocationProvider
{
public:
	virtual ~FileLocationProvider(){}

	//Returns a map of sample identifier to filename
	virtual QList<FileLocation> getBamFiles() = 0;
	virtual QList<FileLocation> getSegFilesCnv() = 0;
	virtual QList<FileLocation> getIgvFilesBaf() = 0;
	virtual QList<FileLocation> getMantaEvidenceFiles() = 0;

	virtual QList<FileLocation> getAnalysisLogFiles() = 0;
	virtual QList<FileLocation> getCircosPlotFiles() = 0;
	virtual QList<FileLocation> getVcfGzFiles() = 0;
	virtual QList<FileLocation> getExpansionhunterVcfFiles() = 0;
	virtual QList<FileLocation> getPrsTsvFiles() = 0;
	virtual QList<FileLocation> getClincnvTsvFiles() = 0;
	virtual QList<FileLocation> getLowcovBedFiles() = 0;

private:
	VariantList variants;
	QString filename;
};




#endif // FILELOCATIONPROVIDER_H
