#include "TestFramework.h"
#include "OntologyTermCollection.h"

TEST_CLASS(OntologyTermCollection_Test)
{
Q_OBJECT
private slots:

	void loadFromOboFile_SequencOntology()
	{
		IS_THROWN(FileAccessException, OntologyTermCollection test("LKJDSAFL", true));

		OntologyTermCollection colletion("://Resources/so-xp_3_0_0.obo", true);

		//not contained
		IS_FALSE(colletion.containsByName("lajfdslajfe"));
		IS_FALSE(colletion.containsByID("id: SO:0000038")); //obsolte
		IS_THROWN(ArgumentException, colletion.getByID("hdskafhkj"));

		//term 1
		IS_TRUE(colletion.containsByID("SO:0000013"));
		IS_TRUE(colletion.containsByName("scRNA"));
		S_EQUAL(colletion.getByID("SO:0000013").name(), "scRNA");
		S_EQUAL(colletion.getByID("SO:0000013").definition(), "A small non coding RNA sequence, present in the cytoplasm.");
		I_EQUAL(colletion.getByID("SO:0000013").synonyms().count(), 1);
		S_EQUAL(colletion.getByID("SO:0000013").synonyms()[0], "small cytoplasmic RNA");
		IS_TRUE(colletion.getByID("SO:0000013").isChildOf("SO:0000655"));
		IS_FALSE(colletion.getByID("SO:0000013").isChildOf("SO:0000658"));
		IS_TRUE(colletion.getByID("SO:0000013").isChildOf("SO:0000655"));

		//term 2
		IS_TRUE(colletion.containsByID("SO:0000014"));
		S_EQUAL(colletion.getByID("SO:0000014").name(), "INR_motif");
		S_EQUAL(colletion.getByID("SO:0000014").definition(), "A sequence element characteristic of some RNA polymerase II promoters required for the correct positioning of the polymerase for the start of transcription. Overlaps the TSS. The mammalian consensus sequence is YYAN(T|A)YY; the Drosophila consensus sequence is TCA(G|T)t(T|C). In each the A is at position +1 with respect to the TSS. Functionally similar to the TATA box element.");
		I_EQUAL(colletion.getByID("SO:0000014").synonyms().count(), 3);
		S_EQUAL(colletion.getByID("SO:0000014").synonyms()[0], "initiator");
		S_EQUAL(colletion.getByID("SO:0000014").synonyms()[1], "initiator motif");
		S_EQUAL(colletion.getByID("SO:0000014").synonyms()[2], "INR motif");
	}


	void loadFromOboFile_HPO()
	{
		OntologyTermCollection colletion("://Resources/qcML.obo", true);

		IS_TRUE(colletion.containsByID("QC:2000015"));
		IS_TRUE(colletion.containsByName("high-impact variants percentage"));
		S_EQUAL(colletion.getByID("QC:2000015").name(), "high-impact variants percentage");
		S_EQUAL(colletion.getByID("QC:2000015").type(), "float");
		S_EQUAL(colletion.getByID("QC:2000015").definition(), "Percentage of variants with high impact on the protein, i.e. stop-gain, stop-loss, frameshift, splice-acceptor or splice-donor variants.");
		I_EQUAL(colletion.getByID("QC:2000015").synonyms().count(), 0);
		IS_TRUE(colletion.getByID("QC:2000015").isChildOf("QC:2000004"));
	}

};
