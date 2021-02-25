#include "TestFramework.h"
#include "TsvFile.h"

TEST_CLASS(TsvFile_Test)
{
Q_OBJECT
private slots:

	void load_local_file()
	{
		TsvFile file;
		file.load(TESTDATA("data_in/tsv_many_lines.txt"));

		I_EQUAL(file.comments().size(), 3);
		I_EQUAL(file.headers().size(), 3);
		S_EQUAL(file.headers()[0] , "one");
		S_EQUAL(file.headers()[1] , "two");
		S_EQUAL(file.headers()[2] , "three");
		I_EQUAL(file.rowCount(), 3);
		S_EQUAL(file.row(file.rowCount()-1)[0], "7");
		S_EQUAL(file.row(file.rowCount()-1)[1], "8");
		S_EQUAL(file.row(file.rowCount()-1)[2], "9");
	}
};
