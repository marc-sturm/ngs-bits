#include "ToolBase.h"
#include "VariantList.h"
#include "Helper.h"
#include "NGSHelper.h"
#include "Statistics.h"

#include <QTextStream>
#include <QFileInfo>

class ConcreteTool
        : public ToolBase
{
    Q_OBJECT

public:
    ConcreteTool(int& argc, char *argv[])
        : ToolBase(argc, argv)
    {
    }

    virtual void setup()
    {
		setDescription("Estimates the ancestry of a sample based on variants.");
		setExtendedDescription(QStringList() << "The ancestry estimation is based on a simple correlation to the most informative SNPs for each population."
											 << "It is ");

		addInfileList("in", "Input variant list(s) in VCF format.", false);
		//optional
		addOutfile("out", "Output TSV file. If unset, writes to STDOUT.", true);
		addInt("min_snps", "Minimum number of informative SNPs for population determination. If less SNPs are found, 'UNKNOWN' is returned.", true, 1000);
		addFloat("pop_dist", "Minimum relative distance between first/second population score. If below this score 'ADMIXED/UNKNOWN' is called.", true, 0.20);

		changeLog(2018, 07, 03, "First version.");
	}

	virtual void main()
	{
		//init
		QStringList in = getInfileList("in");
		QSharedPointer<QFile> outfile = Helper::openFileForWriting(getOutfile("out"), true);
		QTextStream out(outfile.data());
		int min_snps = getInt("min_snps");
		double pop_dist = getFloat("pop_dist");

		//process
		out << "#sample\tsnps\tAFR\tEUR\tSAS\tEAS" << endl;
		foreach(QString filename, in)
		{

			//load variant list
			VariantList vl;
			vl.load(filename);

			SampleAncestry ancestry = Statistics::ancestry(vl, min_snps, pop_dist);
			out << QFileInfo(filename).fileName()
				<< "\t" << ancestry.snps
				<< "\t" << QString::number(ancestry.afr, 'f', 4)
				<< "\t" << QString::number(ancestry.eur, 'f', 4)
				<< "\t" << QString::number(ancestry.sas, 'f', 4)
				<< "\t" << QString::number(ancestry.eas, 'f', 4)
				<< "\t" << ancestry.population << endl;
		}
	}
};

#include "main.moc"

int main(int argc, char *argv[])
{
    ConcreteTool tool(argc, argv);
    return tool.execute();
}

