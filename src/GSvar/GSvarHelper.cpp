#include "GSvarHelper.h"
#include "LoginManager.h"
#include "NGSD.h"
#include "HttpHandler.h"
#include "Settings.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

GeneSet GSvarHelper::imprinting_genes_ = GeneSet();
GeneSet GSvarHelper::hi0_genes_ = GeneSet();
GeneSet GSvarHelper::pseudogene_genes_ = GeneSet();
QMap<QByteArray, QByteArrayList> GSvarHelper::preferred_transcripts_ = QMap<QByteArray, QByteArrayList>();
QMap<QByteArray, QList<BedLine>> GSvarHelper::special_regions_ = QMap<QByteArray, QList<BedLine>>();
QMap<QByteArray, QByteArrayList> GSvarHelper::transcript_matches_ = QMap<QByteArray, QByteArrayList>();

const GeneSet& GSvarHelper::impritingGenes()
{
	static bool initialized = false;

	if (!initialized)
	{
        imprinting_genes_.clear();

		//load imprinting gene list
		QStringList lines = Helper::loadTextFile(":/Resources/imprinting_genes.tsv", true, '#', true);
		foreach(const QString& line, lines)
		{
			QStringList parts = line.split("\t");
			if (parts.count()==2)
			{
				imprinting_genes_ << parts[0].toLatin1();
			}
		}

		initialized = true;
	}

	return imprinting_genes_;
}

const GeneSet& GSvarHelper::hi0Genes()
{
	static bool initialized = false;

	if (!initialized)
	{
		hi0_genes_.clear();

		//load imprinting gene list
		QStringList lines = Helper::loadTextFile(":/Resources/genes_actionable_hi0.tsv", true, '#', true);
		foreach(const QString& line, lines)
		{
			hi0_genes_ << line.toLatin1();
		}

		initialized = true;
	}

	return hi0_genes_;
}

const GeneSet& GSvarHelper::genesWithPseudogene()
{
	static bool initialized = false;

	if (!initialized)
	{
		if (LoginManager::active())
		{
			NGSD db;
			QStringList genes = db.getValues("SELECT DISTINCT(g.symbol) FROM gene g, gene_pseudogene_relation gpr WHERE g.id=gpr.parent_gene_id");
			foreach(QString gene, genes)
			{
				pseudogene_genes_ << gene.toLatin1();
			}

			initialized = true;
		}

	}

	return pseudogene_genes_;
}

const QMap<QByteArray, QByteArrayList>& GSvarHelper::preferredTranscripts(bool reload)
{
    static bool initialized = false;

    if (!initialized || reload)
    {
		if (LoginManager::active())
		{
			NGSD db;
			preferred_transcripts_ = db.getPreferredTranscripts();

			initialized = true;
		}

    }

    return preferred_transcripts_;
}

const QMap<QByteArray, QList<BedLine>> & GSvarHelper::specialRegions()
{
    static bool initialized = false;

    if (!initialized)
    {
        special_regions_.clear();

        QString filename = GSvarHelper::applicationBaseName() + "_special_regions.tsv";
        QStringList lines = Helper::loadTextFile(filename, true, '#', true);
        foreach(const QString& line, lines)
        {
            QByteArrayList parts = line.toLatin1().split('\t');
            if (parts.count()>=2)
            {
                QByteArray gene = parts[0].trimmed();
                for (int i=1; i<parts.count(); ++i)
                {
                    special_regions_[gene] << BedLine::fromString(parts[i]);
                }
            }
        }

        initialized = true;
    }

	return special_regions_;
}

const QMap<QByteArray, QByteArrayList>& GSvarHelper::transcriptMatches()
{
	static bool initialized = false;

	if (!initialized)
	{
		QStringList lines = Helper::loadTextFile(":/Resources/hg19_ensembl_transcript_matches.tsv", true, '#', true);
		foreach(const QString& line, lines)
		{
			QByteArrayList parts = line.toLatin1().split('\t');
			if (parts.count()>=2)
			{
				QByteArray enst = parts[0];
				QByteArray match = parts[1];
				transcript_matches_[enst] << match;
			}
		}

		initialized = true;
	}

	return transcript_matches_;
}

QString GSvarHelper::applicationBaseName()
{
	return QCoreApplication::applicationDirPath() + QDir::separator() + QCoreApplication::applicationName();
}

void GSvarHelper::colorGeneItem(QTableWidgetItem* item, const GeneSet& genes)
{
	//init
	static const GeneSet& imprinting_genes = impritingGenes();
	static const GeneSet& hi0_genes = hi0Genes();
	static const GeneSet& pseudogene_genes = genesWithPseudogene();

	QStringList messages;

	GeneSet intersect = genes.intersect(imprinting_genes);
	foreach(const QByteArray& gene, intersect)
	{
		messages << (gene + ": Imprinting gene");
	}

	intersect = genes.intersect(hi0_genes);
	foreach(const QByteArray& gene, intersect)
	{
		messages << (gene + ": No evidence for haploinsufficiency");
	}

	intersect = genes.intersect(pseudogene_genes);
	foreach(const QByteArray& gene, intersect)
	{
		messages << (gene + ": Has pseudogene(s)");
	}

	//mark gene
	if (!messages.isEmpty())
	{
		messages.sort();
		item->setBackgroundColor(Qt::yellow);
		item->setToolTip(messages.join("\n"));
	}
}

QString GSvarHelper::getEvidenceFile(const QString& bam_file)
{
	if (!bam_file.endsWith(".bam", Qt::CaseInsensitive))
	{
		THROW(ArgumentException, "Invalid BAM file path \"" + bam_file + "\"!");
	}
	QFileInfo bam_file_info(bam_file);
	QDir evidence_dir(bam_file_info.absolutePath() + "/manta_evid/");
	QString ps_name = bam_file_info.fileName().left(bam_file_info.fileName().length() - 4);
	return evidence_dir.absoluteFilePath(ps_name + "_manta_evidence.bam");
}

BedLine GSvarHelper::liftOver(const Chromosome& chr, int start, int end)
{
	//call lift-over webservice
	QString base_url = Settings::string("liftover_webservice");
	HttpHandler handler(HttpHandler::NONE);
	QString output = handler.get(base_url + "?chr=" + chr.strNormalized(true) + "&start=" + QString::number(start) + "&end=" + QString::number(end));

	//handle error from webservice
	if (output.contains("ERROR")) THROW(ArgumentException, "GSvarHelper::liftOver: " + output);

	//convert output to region
	BedLine region = BedLine::fromString(output);
	if (!region.isValid()) THROW(ArgumentException, "GSvarHelper::liftOver: Could not convert output '" + output + "' to region");

	return region;
}

QString GSvarHelper::gnomaADLink(const Variant& v)
{
	QString url = "http://gnomad.broadinstitute.org/variant/" + v.chr().strNormalized(false) + "-";

	if (v.obs()=="-") //deletion
	{
		int pos = v.start()-1;
		FastaFileIndex idx(Settings::string("reference_genome"));
		QString base = idx.seq(v.chr(), pos, 1);
		url += QString::number(pos) + "-" + base + v.ref() + "-" + base;
	}
	else if (v.ref()=="-") //insertion
	{
		int pos = v.start();
		FastaFileIndex idx(Settings::string("reference_genome"));
		QString base = idx.seq(v.chr(), pos, 1);
		url += QString::number(v.start()) + "-" + base + "-" + base + v.obs();
	}
	else //snv
	{
		url += QString::number(v.start()) + "-" + v.ref() + "-" + v.obs();
	}

	return url;
}
