### NGSDExportAnnotationData tool help
	NGSDExportAnnotationData (2019_11-21-ga4bba306)
	
	Generates a VCF file with all variants and annotations from the NGSD and a BED file containing the gene information of the NGSD.
	
	Mandatory parameters:
	  -variants <file>   Output variant list as VCF.
	
	Optional parameters:
	  -genes <file>      Optional BED file containing the genes and the gene info (only germline).
	                     Default value: ''
	  -reference <file>  Reference genome FASTA file. If unset 'reference_genome' from the 'settings.ini' file is used.
	                     Default value: ''
	  -test              Uses the test database instead of on the production database.
	                     Default value: 'false'
	  -maxAF <float>     Determines the maximum allel frequency for the variants (default: 0.05).
	                     Default value: '0.05'
	  -gene_offset <int> Defines the number of bases by which the region of each gene is extended.
	                     Default value: '5000'
	  -chr <string>      Limit export to the given chromosome.
	                     Default value: ''
	  -mode <enum>       Determines the database which is exported.
	                     Default value: 'germline'
	                     Valid: 'germline,somatic'
	
	Special parameters:
	  --help             Shows this help and exits.
	  --version          Prints version and exits.
	  --changelog        Prints changeloge and exits.
	  --tdx              Writes a Tool Definition Xml file. The file name is the application name with the suffix '.tdx'.
	
### NGSDExportAnnotationData changelog
	NGSDExportAnnotationData 2019_11-21-ga4bba306
	
	2019-12-06 Comments are now URL encoded.
	2019-09-25 Added somatic mode.
	2019-07-29 Added BED file for genes.
	2019-07-25 Initial version of this tool.
[back to ngs-bits](https://github.com/imgag/ngs-bits)