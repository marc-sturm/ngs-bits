### VariantRanking tool help
	VariantRanking (2020_09-90-g55257954)
	
	Annotatates variants in GSvar format with a score/rank.
	
	Mandatory parameters:
	  -in <file>         Input variant list in GSvar format.
	  -hpo_ids <string>  Comma-separated list of HPO identifiers.
	  -out <file>        Output variant list in GSvar format.
	
	Optional parameters:
	  -algorithm <enum>  Algorithm used for ranking.
	                     Default value: 'GSvar_v1'
	                     Valid: 'GSvar_v1,GSvar_v1_noNGSD'
	  -add_explaination  Add a third column with an explaination how that score was calculated.
	                     Default value: 'false'
	  -test              Uses the test database instead of on the production database.
	                     Default value: 'false'
	
	Special parameters:
	  --help             Shows this help and exits.
	  --version          Prints version and exits.
	  --changelog        Prints changeloge and exits.
	  --tdx              Writes a Tool Definition Xml file. The file name is the application name with the suffix '.tdx'.
	
### VariantRanking changelog
	VariantRanking 2020_09-90-g55257954
	
	2020-11-20 Initial commit.
[back to ngs-bits](https://github.com/imgag/ngs-bits)