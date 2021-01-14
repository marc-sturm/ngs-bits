#ifndef FILELOCATIONHELPER_H
#define FILELOCATIONHELPER_H

#include "cppNGS_global.h"
#include "Exceptions.h"

enum class PathType
{
	PROJECT_FOLDER, // project root folder
	SAMPLE_FOLDER, // folder with samples
	BAM, // binary alignment map with sequence alignment data
	GSVAR, // GSVar tool sample data
	VCF, // variant call format file storing gene sequence variations
	BAF, // b-allele frequency file
	COPY_NUMBER_CALLS, // BED files
	COPY_NUMBER_RAW_DATA, // SEG file with copy
	MANTA_EVIDENCE, // also BAM files
	OTHER // everything else
};

class CPPNGSSHARED_EXPORT FileLocationHelper
{
public:
	///Returns a string representation for PathType
	static QString pathTypeToString(PathType type);

	///Converts a string into PathType value
	static PathType stringToPathType(QString in);

	//Returns the file path to the Manta evididence file for a given BAM file.
	static QString getEvidenceFile(const QString& bam_file);

};

#endif // FILELOCATIONHELPER_H
