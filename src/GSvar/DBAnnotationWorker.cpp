#include "DBAnnotationWorker.h"
#include "Exceptions.h"

DBAnnotationWorker::DBAnnotationWorker(QString filename, QString genome, VariantList& variants, BusyDialog* busy)
	: WorkerBase("Database annotation")
	, filename_(filename)
	, genome_(genome)
	, variants_(variants)
	, gpd_()
	, ngsd_()
{
	connect(&gpd_, SIGNAL(initProgress(QString, bool)), busy, SLOT(init(QString, bool)));
	connect(&gpd_, SIGNAL(updateProgress(int)), busy, SLOT(update(int)));
	connect(&ngsd_, SIGNAL(initProgress(QString, bool)), busy, SLOT(init(QString, bool)));
	connect(&ngsd_, SIGNAL(updateProgress(int)), busy, SLOT(update(int)));
}

void DBAnnotationWorker::process()
{
	try
	{
		gpd_.annotate(variants_);
		ngsd_.annotate(variants_, filename_, genome);
	}
	catch (Exception& e)
	{
		error_message_ = e.message();
	}
}

