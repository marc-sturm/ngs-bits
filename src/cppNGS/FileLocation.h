#ifndef FILELOCATION_H
#define FILELOCATION_H

#include "cppNGS_global.h"
#include <QString>

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

	ANALYSIS_LOG, // analysis log files *.log
	ANALYSIS_ANY_LOG, // *_log?_*.log
	CIRCOS_PLOT, // *_circos.png
	CNVS_CLINCNV_TSV, // *_cnvs_clincnv.tsv
	CLINCNV_TSV, // *_clincnv.tsv
	CNVS_TSV, // *_cnvs.tsv
	PRS_TSV, // *_prs.tsv
	ROHS_TSV, // *_rohs.tsv
	VAR_FUSIONS_TSV, // *_var_fusions.tsv
	LOWCOV_BED, // *_lowcov.bed
	STAT_LOWCOV_BED, // *_stat_lowcov.bed
	ANY_BED, // *.bed
	VCF_GZ, // *_var_annotated.vcf.gz
	REPEATS_EXPANSION_HUNTER_VCF, // *_repeats_expansionhunter.vcf
	FASTQ_GZ, // *.fastq.gz
	CNVS_CLINCNV_SEG, // *_cnvs_clincnv.seg
	CNVS_SEG, // *_cnvs.seg

	OTHER // everything else
};

struct FileLocation
{
	QString id; //sample identifier/name
	PathType type; //file type
	QString filename; //file name
	bool is_found; // indicates if a file exists or not

	bool operator == (const FileLocation& x) const
	{
	  return (id == x.id && type == x.type && filename == x.filename);
	}
};

#endif // FILELOCATION_H
