#include "RefGenDownloadWorker.h"

RefGenDownloadWorker::RefGenDownloadWorker()
: WorkerBase("Reference genome download")
{
}

void RefGenDownloadWorker::process()
{
	qDebug() << "Debug statement in a thread";

	if (!QFile(Settings::string("reference_genome")).exists())
	{
		qint64 reply = HttpHandler(HttpRequestHandler::NONE).getFileSize(Settings::string("remote_reference_genome"));

		QFile file(Settings::string("reference_genome"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			messaged("Cannot save the reference genome locally");
		}
		QTextStream out(&file);

		int chunk_count = 100;
		qint64 chunk_size = reply / chunk_count;
		int remainder = reply % chunk_count;

		for (int i = 0; i < (chunk_count+1); i++)
		{
			qDebug() << "Download part #:" << i;
			emit updated(i);
			HttpHeaders headers {};

			QString range = "bytes=" + QString::number(i*chunk_size) + "-" + QString::number(((i+1)*chunk_size)-1);
			if (i == (chunk_count))
			{
				range = "bytes=" + QString::number(chunk_count*chunk_size) + "-" + QString::number((chunk_count*chunk_size) + remainder);
			}

			qDebug() << range;
			headers.insert("Range", range.toLocal8Bit());
			QString chunk = HttpHandler(HttpRequestHandler::NONE).get(Settings::string("remote_reference_genome"), headers);
			out << chunk;
		}
		file.close();
	}	
}
