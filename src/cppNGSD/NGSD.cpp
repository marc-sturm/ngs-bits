#include "NGSD.h"
#include "Exceptions.h"
#include "Helper.h"
#include "Log.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QFileInfo>
#include "Settings.h"


NGSD::NGSD()
{
	//connect to DB
	db_ = QSqlDatabase::addDatabase("QMYSQL", "NGSD_" + Helper::randomString(20));
	db_.setHostName(Settings::string("ngsd_host"));
	db_.setDatabaseName(Settings::string("ngsd_name"));
	db_.setUserName(Settings::string("ngsd_user"));
	db_.setPassword(Settings::string("ngsd_pass"));

	if (!db_.open())
	{
		THROW(DatabaseException, "Could not connect to the NGSD database!");
	}
	//Log::info("MYSQL openend  - name: " + db_.connectionName() + " valid: " + (db_.isValid() ? "yes" : "no"));
}

QVariant NGSD::getValue(const QString& query, bool no_value_is_ok)
{
	SqlQuery q = getQuery();
	q.exec(query);

	if (q.size()==0)
	{
		if (no_value_is_ok)
		{
			return QVariant();
		}
		else
		{
			THROW(DatabaseException, "NGSD single value query returned no value: " + query);
		}
	}
	if (q.size()>1)
	{
		THROW(DatabaseException, "NGSD single value query returned several values: " + query);
	}

	q.next();
	return q.value(0);
}

QVariantList NGSD::getValues(const QString& query)
{
	SqlQuery q = getQuery();
	q.exec(query);

	QVariantList output;
	output.reserve(q.size());
	while(q.next())
	{
		output.append(q.value(0));
	}
	return output;
}

int NGSD::addColumn(VariantList& variants, QString name, QString description)
{
	variants.annotations().append(VariantAnnotationDescription(name, description));
	for (int i=0; i<variants.count(); ++i)
	{
		variants[i].annotations().append("");
	}

	return variants.annotations().count() - 1;
}

bool NGSD::removeColumnIfPresent(VariantList& variants, QString name, bool exact_name_match)
{
	int index = variants.annotationIndexByName(name, exact_name_match, false);
	if (index==-1) return false;

	variants.removeAnnotation(index);
	return true;
}

NGSD::~NGSD()
{
	//Log::info("MYSQL closing  - name: " + db_.connectionName());
	db_.close();
}

QString NGSD::getExternalSampleName(const QString& filename)
{
	//query
	QVariant value = getValue("SELECT name_external FROM sample WHERE name='" + sampleName(filename) + "'");

	//output
	if (value.isNull() || !value.isValid() || value.toString().trimmed().isEmpty())
	{
		return "n/a";
	}
	return value.toString().trimmed();
}

QString NGSD::getProcessingSystem(const QString& filename, SystemType type)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processing system ID
	QString sys_id = getValue("SELECT processing_system_id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	QString what;
	if (type==SHORT)
	{
		what = "name_short";
	}
	else if (type==LONG)
	{
		what = "name";
	}
	else if (type==BOTH)
	{
		what = "CONCAT(name, ' (', name_short, ')')";
	}
	else
	{
		THROW(ProgrammingException, "Unknown SystemType '" + QString::number(type) + "'!");
	}
	return getValue("SELECT " + what + " FROM processing_system WHERE id='" + sys_id + "'").toString().trimmed();
}


QString NGSD::getGenomeBuild(const QString& filename)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	return getValue("SELECT g.build FROM processed_sample ps, processing_system sys, genome g WHERE ps.id='" + ps_id + "' AND ps.processing_system_id=sys.id AND sys.genome_id=g.id").toString();
}

QPair<QString, QString> NGSD::getValidationStatus(const QString& filename, const Variant& variant)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	//get variant ID
	QString v_id = getValue("SELECT id FROM variant WHERE chr='"+variant.chr().str()+"' AND start='"+QString::number(variant.start())+"' AND end='"+QString::number(variant.end())+"' AND ref='"+variant.ref()+"' AND obs='"+variant.obs()+"'", false).toString();

	//get validation info
	SqlQuery result = getQuery();
	result.exec("SELECT validated, validation_comment FROM detected_variant WHERE processed_sample_id='" + ps_id + "' AND variant_id='" + v_id + "'");
	result.next();
	return qMakePair(result.value(0).toString().trimmed(), result.value(1).toString().trimmed());
}

QCCollection NGSD::getQCData(const QString& filename)
{
	//get sample ids
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();
	QString ps_number = processedSampleNumber(filename);
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + ps_number + "'").toString();

	//get NGSO data
	SqlQuery q = getQuery();
	q.exec("SELECT n.name, nm.value, n.description, n.ngso_id FROM nm_processed_sample_ngso as nm, ngso as n WHERE nm.processed_sample_id='" + ps_id + "' AND nm.ngso_id=n.id");
	QCCollection output;
	while(q.next())
	{
		output.insert(QCValue(q.value(0).toString(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString()));
	}

	//get KASP data
	SqlQuery q2 = getQuery();
	q2.exec("SELECT random_error_prob FROM kasp_status WHERE processed_sample_id='" + ps_id + "'");
	QString value = "n/a";
	if (q2.size()>0)
	{
		q2.next();
		float numeric_value = 100.0 * q2.value(0).toFloat();
		if (numeric_value>1.0)
		{
			value = "<font color=red>"+QString::number(numeric_value)+"%</font>";
		}
		else
		{
			value = QString::number(numeric_value)+"%";
		}
	}
	output.insert(QCValue("kasp", value));

	return output;
}

QVector<double> NGSD::getQCValues(const QString& accession, const QString& filename)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processing system ID
	QString sys_id = getValue("SELECT processing_system_id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	//get NGSO id
	QString ngso_id = getValue("SELECT id FROM ngso WHERE ngso_id='" + accession + "'").toString();

	//get QC data
	SqlQuery q = getQuery();
	q.exec("SELECT nm.value FROM nm_processed_sample_ngso as nm, processed_sample as ps WHERE ps.processing_system_id='" + sys_id + "' AND nm.ngso_id='" + ngso_id + "' AND nm.processed_sample_id=ps.id ");

	//fill output datastructure
	QVector<double> output;
	while(q.next())
	{
		bool ok = false;
		double value = q.value(0).toDouble(&ok);
		if (ok) output.append(value);
	}

	return output;
}

void NGSD::annotate(VariantList& variants, QString filename)
{
	initProgress("NGSD annotation", true);

	//get sample ids
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();
	QString ps_number = processedSampleNumber(filename);
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + ps_number + "'").toString();
	QString sys_id = getValue("SELECT processing_system_id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + ps_number + "'").toString();

	//check if we could determine the sample
	bool found_in_db = true;
	if (s_id=="" || ps_number=="" || ps_id=="" || sys_id=="")
	{
		Log::warn("Could not find processed sample in NGSD by name '" + filename + "'. Annotation will be incomplete because processing system could not be determined!");
		found_in_db = false;
	}

	//get sample ids that have processed samples with the same processing system (not same sample, variants imported, same processing system, good quality of sample, not tumor)
	QSet<int> sys_sample_ids;
	QVariantList ps_ids_var = getValues("SELECT DISTINCT s.id FROM processed_sample as ps, sample s WHERE ps.processing_system_id='" + sys_id + "' AND ps.sample_id=s.id AND s.tumor='0' AND s.quality='good' AND s.id!='" + s_id + "' AND (SELECT count(id) FROM detected_variant as dv WHERE dv.processed_sample_id = ps.id)>0");
	foreach(const QVariant& var, ps_ids_var)
	{
		sys_sample_ids.insert(var.toInt());
	}

	//remove all NGSD-specific columns
	QList<VariantAnnotationDescription> descs = variants.annotations();
	foreach(const VariantAnnotationDescription& desc, descs)
	{
		if (desc.name().startsWith("ihdb_"))
		{
			removeColumnIfPresent(variants, desc.name(), true);
		}
	}
	removeColumnIfPresent(variants, "classification", true);
	removeColumnIfPresent(variants, "validated", true);
	removeColumnIfPresent(variants, "comment", true);

	//get required column indices
	QString num_samples = QString::number(sys_sample_ids.count());
	int ihdb_hom_idx = addColumn(variants, "ihdb_hom", "Homozygous variant counts in NGSD for the same processing system (" + num_samples + " samples).");
	int ihdb_het_idx = addColumn(variants, "ihdb_het", "Heterozyous variant counts in NGSD for the same processing system (" + num_samples + " samples).");
	int ihdb_wt_idx  = addColumn(variants, "ihdb_wt", "Wildtype variant counts in NGSD for the same processing system (" + num_samples + " samples).");
	int ihdb_all_hom_idx = addColumn(variants, "ihdb_allsys_hom", "Homozygous variant counts in NGSD independent of the processing system.");
	int ihdb_all_het_idx =  addColumn(variants, "ihdb_allsys_het", "Heterozygous variant counts in NGSD independent of the processing system.");
	int class_idx = addColumn(variants, "classification", "VUS classification from the NGSD.");
	int valid_idx = addColumn(variants, "validated", "Validation information from the NGSD.");
	if (variants.annotationIndexByName("comment", true, false)==-1) addColumn(variants, "comment", "Comments from the NGSD.");
	int comment_idx = variants.annotationIndexByName("comment", true, false);

	//(re-)annotate the variants
	QSqlQuery query = getQuery();
	for (int i=0; i<variants.count(); ++i)
	{
		//QTime timer;
		//timer.start();

		//variant infos
		Variant& v = variants[i];
		query.exec("SELECT id, vus FROM variant WHERE chr='"+v.chr().str()+"' AND start='"+QString::number(v.start())+"' AND end='"+QString::number(v.end())+"' AND ref='"+v.ref()+"' AND obs='"+v.obs()+"'");
		query.next();
		QByteArray v_id = query.value(0).toByteArray();
		v.annotations()[class_idx] = query.value(1).toByteArray().replace("n/a", "");
		//int t_v = timer.elapsed();
		//timer.restart();

		//detected variant infos
		QByteArray comment = "";
		QByteArray validated = "";
		int dv_id = -1;
		if (found_in_db)
		{
			query.exec("SELECT id, validated, comment FROM detected_variant WHERE processed_sample_id='" + ps_id + "' AND variant_id='"+v_id+"'");
			if (query.size()==1)
			{
				query.next();
				dv_id = query.value(0).toInt();
				validated = query.value(1).toByteArray().replace("n/a", "");
				comment = query.value(2).toByteArray();
			}
		}
		//int t_dv = timer.elapsed();
		//timer.restart();

		//validation info other samples
		int tps = 0;
		int fps = 0;
		query.exec("SELECT id, validated FROM detected_variant WHERE variant_id='"+v_id+"' AND validated!='n/a'");
		while(query.next())
		{
			if (query.value(0).toInt()==dv_id) continue;
			if (query.value(1).toByteArray()=="true positive") ++tps;
			else if (query.value(1).toByteArray()=="false positive") ++fps;
		}
		if (tps>0 || fps>0)
		{
			if (validated=="") validated = "n/a";
			validated += " (" + QByteArray::number(tps) + "xTP, " + QByteArray::number(fps) + "xFP)";
		}
		//int t_val = timer.elapsed();
		//timer.restart();

		//comments other samples
		QList<QByteArray> comments;
		query.exec("SELECT id, comment FROM detected_variant WHERE variant_id='"+v_id+"' AND comment IS NOT NULL");
		while(query.next())
		{
			if (query.value(0).toInt()==dv_id) continue;
			QByteArray tmp = query.value(1).toByteArray().trimmed();
			if (tmp!="") comments.append(tmp);
		}
		if (comments.size()>0)
		{
			if (comment=="") comment = "n/a";
			comment += " (" + comments.join(", ") + ")";
		}
		//int t_com = timer.elapsed();
		//timer.restart();

		//genotype counts
		int allsys_hom_count = 0;
		int allsys_het_count = 0;
		int sys_hom_count = 0;
		int sys_het_count = 0;
		QSet<int> s_ids_done;
		int s_id_int = s_id.toInt();
		query.exec("SELECT dv.genotype, ps.sample_id FROM detected_variant as dv, processed_sample ps WHERE dv.processed_sample_id=ps.id AND dv.variant_id='" + v_id + "'");
		while(query.next())
		{
			//skip this sample id
			int current_sample = query.value(1).toInt();
			if (current_sample==s_id_int) continue;

			//skip already seen samples (there could be several processings of the same sample because of different processing systems or because of experment repeats due to quality issues)
			if (s_ids_done.contains(current_sample)) continue;
			s_ids_done.insert(current_sample);

			QByteArray current_geno = query.value(0).toByteArray();
			if (current_geno=="hom")
			{
				++allsys_hom_count;
				if (sys_sample_ids.contains(current_sample))
				{
					++sys_hom_count;
				}
			}
			else if (current_geno=="het")
			{
				++allsys_het_count;
				if (sys_sample_ids.contains(current_sample))
				{
					++sys_het_count;
				}
			}
		}
		//qDebug() << (v.isSNV() ? "S" : "I") << query.size() << t_v << t_dv << t_val << t_com << timer.elapsed();

		v.annotations()[ihdb_all_hom_idx] = QByteArray::number(allsys_hom_count);
		v.annotations()[ihdb_all_het_idx] = QByteArray::number(allsys_het_count);
		if (found_in_db)
		{
			v.annotations()[ihdb_hom_idx] = QByteArray::number((double)sys_hom_count / sys_sample_ids.count(), 'f', 4);
			v.annotations()[ihdb_het_idx] =  QByteArray::number((double)sys_het_count / sys_sample_ids.count(), 'f', 4);
			v.annotations()[ihdb_wt_idx] =  QByteArray::number((double)(sys_sample_ids.count() - sys_hom_count - sys_het_count) / sys_sample_ids.count(), 'f', 4);
			v.annotations()[valid_idx] = validated;
			v.annotations()[comment_idx] = comment.replace("\n", " ");
		}
		else
		{
			v.annotations()[ihdb_hom_idx] = "n/a";
			v.annotations()[ihdb_het_idx] = "n/a";
			v.annotations()[ihdb_wt_idx] = "n/a";
			v.annotations()[valid_idx] = "n/a";
			v.annotations()[comment_idx] = "n/a";
		}

		emit updateProgress(100*i/variants.count());
	}
}

void NGSD::annotateSomatic(VariantList& variants, QString filename, QString ref_file)
{
	//open refererence genome file
	FastaFileIndex reference(ref_file);

	//get sample ids
	QStringList samples = filename.split('-');
	QString ts_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(samples[0]) + "'").toString();

	//check if we could determine the sample
	if (ts_id=="")
	{
		Log::warn("Could not find processed sample in NGSD by name '" + sampleName(samples[0]) + "'. Annotation will be incomplete because processing system could not be determined!");
	}

	//remove all NGSD-specific columns
	QList<VariantAnnotationDescription> descs = variants.annotations();
	foreach(const VariantAnnotationDescription& desc, descs)
	{
		if (desc.name().startsWith("som_ihdb"))
		{
			removeColumnIfPresent(variants, desc.name(), true);
		}
	}

	//get required column indices
	int som_ihdb_c_idx = addColumn(variants, "som_ihdb_c", "Somatic variant count within NGSD.");
	int som_ihdb_p_idx = addColumn(variants, "som_ihdb_p", "Projects with somatic variant in NGSD.");
	//(re-)annotate the variants
	for (int i=0; i<variants.count(); ++i)
	{
		Variant& v = variants[i];

		SqlQuery query = getQuery();
		if (v.isSNV())
		{
			query.exec("SELECT s.id, dsv.processed_sample_id_tumor, p.name FROM detected_somatic_variant as dsv, variant as v, processed_sample ps, sample as s, project as p WHERE ps.project_id=p.id AND dsv.processed_sample_id_tumor=ps.id and dsv.variant_id=v.id AND  ps.sample_id=s.id  AND s.tumor='1' AND v.chr='"+v.chr().str()+"' AND v.start='"+QString::number(v.start())+"' AND v.end='"+QString::number(v.end())+"' AND v.ref='"+v.ref()+"' AND v.obs='"+v.obs()+"'");
		}
		else
		{
			QPair<int, int> indel_region = Variant::indelRegion(v.chr(), v.start(), v.end(), v.ref(), v.obs(), reference);
			query.exec("SELECT s.id, dsv.processed_sample_id_tumor, p.name FROM project as p, detected_somatic_variant as dsv, variant as v, processed_sample ps, sample as s WHERE ps.project_id=p.id AND dsv.processed_sample_id_tumor=ps.id and dsv.variant_id=v.id AND  ps.sample_id=s.id  AND s.tumor='1' AND v.chr='"+v.chr().str()+"' AND v.start>='"+QString::number(indel_region.first)+"' AND v.end<='"+ QString::number(indel_region.second)+"' AND v.ref='"+v.ref()+"' AND v.obs='"+v.obs()+"'");
		}

		//process variants
		QMap<QByteArray, int> project_map;
		QSet<QByteArray> processed_ps_ids;
		QSet<QByteArray> processed_s_ids;
		while(query.next())
		{
			QByteArray current_sample = query.value(0).toByteArray();
			QByteArray current_ps_id = query.value(1).toByteArray();
			QByteArray current_project = query.value(2).toByteArray();

			//skip already seen processed samples (there could be several variants because of indel window, but we want to process only one)
			if (processed_ps_ids.contains(current_ps_id)) continue;
			processed_ps_ids.insert(current_ps_id);

			//skip the current sample for general statistics
			if (current_sample==ts_id) continue;

			//skip already seen samples for general statistics (there could be several processings of the same sample because of different processing systems or because of experment repeats due to quality issues)
			if (processed_s_ids.contains(current_sample)) continue;
			processed_s_ids.insert(current_sample);

			// count
			if(!project_map.contains(current_project))	project_map.insert(current_project,0);
			++project_map[current_project];
		}

		QByteArray somatic_projects;
		int somatic_count = 0;
		QMap<QByteArray, int>::const_iterator j = project_map.constBegin();
		while(j!=project_map.constEnd())
		{
			somatic_count += j.value();
			somatic_projects += j.key() + ",";
			++j;
		}
		v.annotations()[som_ihdb_c_idx] = QByteArray::number(somatic_count);
		v.annotations()[som_ihdb_p_idx] = somatic_projects;
	}
}


void NGSD::setValidationStatus(const QString& filename, const Variant& variant, const QString& status, const QString& comment)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	//get variant ID
	QString v_id = getValue("SELECT id FROM variant WHERE chr='"+variant.chr().str()+"' AND start='"+QString::number(variant.start())+"' AND end='"+QString::number(variant.end())+"' AND ref='"+variant.ref()+"' AND obs='"+variant.obs()+"'", false).toString();

	//set
	getQuery().exec("UPDATE detected_variant SET validated='" + status + "',validation_comment='" + comment + "' WHERE processed_sample_id='" + ps_id + "' AND variant_id='" + v_id + "'");
}

void NGSD::setClassification(const Variant& variant, const QString& classification)
{
	//get user ID
	QString user_id = getValue("SELECT id FROM user WHERE user_id='" + Helper::userName() + "'", false).toString();

	//get variant ID
	QString v_id = getValue("SELECT id FROM variant WHERE chr='"+variant.chr().str()+"' AND start='"+QString::number(variant.start())+"' AND end='"+QString::number(variant.end())+"' AND ref='"+variant.ref()+"' AND obs='"+variant.obs()+"'", false).toString();

	//get variant ID
	getQuery().exec("UPDATE variant SET vus='" + classification + "', vus_user='" + user_id + "', vus_date=CURRENT_TIMESTAMP WHERE id='"+v_id+"'");
}

QString NGSD::comment(const QString& filename, const Variant& variant)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	//get variant ID
	QString v_id = getValue("SELECT id FROM variant WHERE chr='"+variant.chr().str()+"' AND start='"+QString::number(variant.start())+"' AND end='"+QString::number(variant.end())+"' AND ref='"+variant.ref()+"' AND obs='"+variant.obs()+"'", false).toString();

	//get comment
	return getValue("SELECT comment FROM detected_variant WHERE processed_sample_id='" + ps_id + "' AND variant_id='" + v_id + "'").toString();
}

QString NGSD::url(const QString& filename, const Variant& variant)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	//get variant ID
	QString v_id = getValue("SELECT id FROM variant WHERE chr='"+variant.chr().str()+"' AND start='"+QString::number(variant.start())+"' AND end='"+QString::number(variant.end())+"' AND ref='"+variant.ref()+"' AND obs='"+variant.obs()+"'").toString();

	//get detected variant ID
	QString dv_id = getValue("SELECT id FROM detected_variant WHERE processed_sample_id='" + ps_id + "' AND variant_id='" + v_id + "'", false).toString();

	return Settings::string("NGSD")+"/variants/view/" + dv_id;
}

QString NGSD::url(const QString& filename)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'", false).toString();

	//get url
	return Settings::string("NGSD")+"/processedsamples/view/" + ps_id;
}

QString NGSD::urlSearch(const QString& search_term)
{
	return Settings::string("NGSD")+"/search/processSearch/search_term="+search_term;
}

void NGSD::setComment(const QString& filename, const Variant& variant, const QString& text)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	//get variant ID
	QString v_id = getValue("SELECT id FROM variant WHERE chr='"+variant.chr().str()+"' AND start='"+QString::number(variant.start())+"' AND end='"+QString::number(variant.end())+"' AND ref='"+variant.ref()+"' AND obs='"+variant.obs()+"'", false).toString();

	//set
	getQuery().exec("UPDATE detected_variant SET comment='" + text + "' WHERE processed_sample_id='" + ps_id + "' AND variant_id='" + v_id + "'");
}

void NGSD::setReportVariants(const QString& filename, const VariantList& variants, QSet<int> selected_indices)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();

	//get variant ID
	for(int i=0; i<variants.count(); ++i)
	{
		const Variant& variant = variants[i];
		QString v_id = getValue("SELECT id FROM variant WHERE chr='"+variant.chr().str()+"' AND start='"+QString::number(variant.start())+"' AND end='"+QString::number(variant.end())+"' AND ref='"+variant.ref()+"' AND obs='"+variant.obs()+"'", false).toString();
		getQuery().exec("UPDATE detected_variant SET report=" + QString(selected_indices.contains(i) ? "1" : "0" ) + " WHERE processed_sample_id='" + ps_id + "' AND variant_id='" + v_id + "'");
	}
}

QStringList NGSD::getEnum(QString table, QString column)
{
	//check cache
	static QMap<QString, QStringList> cache;
	QString hash = table+"."+column;
	if (cache.contains(hash))
	{
		return cache.value(hash);
	}

	//DB query
	SqlQuery q = getQuery();
	q.exec("DESCRIBE "+table+" "+column);
	while (q.next())
	{
		QString type = q.value(1).toString();
		type.replace("'", "");
		type.replace("enum(", "");
		type.replace(")", "");
		cache[hash] = type.split(",");
		return cache[hash];
	}

	THROW(ProgrammingException, "Could not determine enum values of column '"+column+"' in table '"+table+"'!");
}

QStringList NGSD::getDiagnosticStatus(const QString& filename)
{
	//get sample ID
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sampleName(filename) + "'").toString();

	//get processed sample ID
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + processedSampleNumber(filename) + "'").toString();
	if (ps_id=="") return QStringList();

	//get status
	SqlQuery q = getQuery();
	q.exec("SELECT s.status, u.name, s.date, s.outcome FROM diag_status as s, user as u WHERE s.processed_sample_id='" + ps_id +  "' AND s.user_id=u.id");
	if (q.size()==0) return (QStringList() << "n/a" << "n/a" << "n/a" << "n/a");
	q.next();
	return (QStringList() << q.value(0).toString() << q.value(1).toString() << q.value(2).toString().replace('T', ' ') << q.value(3).toString());
}

void NGSD::setDiagnosticStatus(const QString& filename, QString status)
{
	//get sample ID
	QString sample_name = sampleName(filename);
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sample_name + "'").toString();

	//get processed sample ID
	QString ps_num = processedSampleNumber(filename);
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + ps_num + "'").toString();

	//get user ID
	QString user_id = getValue("SELECT id FROM user WHERE user_id='" + Helper::userName() + "'", false).toString();

	//update status
	getQuery().exec("INSERT INTO diag_status (processed_sample_id, status, user_id) VALUES ("+ps_id+",'"+status+"', "+user_id+") ON DUPLICATE KEY UPDATE status='"+status+"',user_id="+user_id+"");

	//add new processed sample if scheduled for repetition
	if (status.startsWith("repeat"))
	{
		QString next_ps_num = getValue("SELECT MAX(process_id)+1 FROM processed_sample WHERE sample_id=" + s_id).toString();
		SqlQuery query = getQuery();
		query.exec("SELECT mid1_i7, mid2_i5, operator_id, processing_system_id, project_id, molarity FROM processed_sample WHERE id=" + ps_id);
		query.next();
		QString mid1 = query.value(0).toString();
		if (mid1=="0") mid1="NULL";
		QString mid2 = query.value(1).toString();
		if (mid2=="0") mid2="NULL";
		QString op_id = query.value(2).toString();
		if (op_id=="0") op_id="NULL";
		QString sys_id = query.value(3).toString();
		QString proj_id = query.value(4).toString();
		QString molarity = query.value(5).toString();
		if (molarity=="0") molarity="NULL";
		QString user_name = getValue("SELECT name FROM user WHERE id='" + user_id + "'").toString();
		QString comment = user_name + " requested re-sequencing (" + status + ") of sample " + sample_name + "_" + ps_num.rightJustified(2, '0') + " on " + Helper::dateTime("dd.MM.yyyy hh:mm:ss");
		if (status=="repeat sequencing only")
		{
			getQuery().exec("INSERT INTO processed_sample (sample_id, process_id, mid1_i7, mid2_i5, operator_id, processing_system_id, comment, project_id, molarity, status) VALUES ("+ s_id +","+ next_ps_num +","+ mid1 +","+ mid2 +","+ op_id +","+ sys_id +",'"+ comment +"',"+ proj_id +","+ molarity +",'todo')");
		}
		else if (status=="repeat library and sequencing")
		{
			getQuery().exec("INSERT INTO processed_sample (sample_id, process_id, operator_id, processing_system_id, comment, project_id, status) VALUES ("+ s_id +","+ next_ps_num +","+ op_id +","+ sys_id +",'"+ comment +"',"+ proj_id +",'todo')");
		}
		else
		{
			THROW(ProgrammingException, "Unknown diagnostic status '" + status +"!'");
		}
	}
}

void NGSD::setReportOutcome(const QString& filename, QString outcome)
{
	//get sample ID
	QString sample_name = sampleName(filename);
	QString s_id = getValue("SELECT id FROM sample WHERE name='" + sample_name + "'").toString();

	//get processed sample ID
	QString ps_num = processedSampleNumber(filename);
	QString ps_id = getValue("SELECT id FROM processed_sample WHERE sample_id='" + s_id + "' AND process_id='" + ps_num + "'").toString();

	//get user ID
	QString user_id = getValue("SELECT id FROM user WHERE user_id='" + Helper::userName() + "'", false).toString();

	//update status
	getQuery().exec("INSERT INTO diag_status (processed_sample_id, status, user_id, outcome) VALUES ("+ps_id+",'pending',"+user_id+",'"+outcome+"') ON DUPLICATE KEY UPDATE user_id="+user_id+",outcome='"+outcome+"'");
}

QString NGSD::sampleName(const QString& filename)
{
	QStringList parts = QFileInfo(filename).baseName().append('_').split('_');

	return parts[0];
}

QString NGSD::processedSampleNumber(const QString& filename)
{
	QStringList parts = QFileInfo(filename).baseName().append('_').split('_');

	bool ok = false;
	int ps_num = parts[1].toInt(&ok);
	if (!ok) return "";

	return QString::number(ps_num);
}

