#include "TestFramework.h"

TEST_CLASS(BedSubtract_Test)
{
Q_OBJECT
private slots:
	
	void test_01()
	{
		EXECUTE("BedSubtract", "-in " + TESTDATA("data_in/BedSubtract_in1.bed") + " -in2 " + TESTDATA("data_in/BedSubtract_in2.bed") + " -out out/BedSubtract_test01_out.bed");
		COMPARE_FILES("out/BedSubtract_test01_out.bed", TESTDATA("data_out/BedSubtract_test01_out.bed"));
	}
	
	void test_02()
	{
		EXECUTE("BedSubtract", "-in " + TESTDATA("data_in/BedSubtract_in2.bed") + " -in2 " + TESTDATA("data_in/BedSubtract_in1.bed") + " -out out/BedSubtract_test02_out.bed");
		COMPARE_FILES("out/BedSubtract_test02_out.bed", TESTDATA("data_out/BedSubtract_test02_out.bed"));
	}
	
};
