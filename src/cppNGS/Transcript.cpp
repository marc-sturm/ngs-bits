#include "Transcript.h"
#include "Exceptions.h"
#include "NGSHelper.h"
#include <QRegularExpression>

Transcript::Transcript()
	: coding_start_(0)
	, coding_end_(0)
{
}

void Transcript::setRegions(const BedFile& regions, int coding_start, int coding_end)
{
	regions_ = regions;

	coding_start_ = coding_start;
	coding_end_ = coding_end;

	if (isCoding())
	{
		coding_regions_.clear();

		for (int i=0; i<regions_.count(); ++i)
		{
			const BedLine& line = regions_[i];

			int start = std::max(line.start(), coding_start_);
			int end = std::min(line.end(), coding_end_);

			if (end<coding_start_ || start>coding_end_) continue;

			coding_regions_.append(BedLine(line.chr(), start, end));
		}

		coding_regions_.merge();
	}
}

QString Transcript::sourceToString(Transcript::SOURCE source)
{
	switch(source)
	{
		case CCDS:
			return "CCDS";
		case ENSEMBL:
			return "ENSEMBL";
	}

	THROW(ProgrammingException, "Unknown transcript source enum value '" + QString::number(source) + "!");
}

Transcript::SOURCE Transcript::stringToSource(QString source)
{
	source = source.toUpper();
	if (source=="CCDS")
	{
		return CCDS;
	}
	if (source=="ENSEMBL")
	{
		return ENSEMBL;
	}

	THROW(ProgrammingException, "Unknown transcript source string '" + source + "!");
}

QByteArray Transcript::strandToString(Transcript::STRAND strand)
{
	switch(strand)
	{
		case PLUS:
			return "+";
		case MINUS:
			return "-";
	}

	THROW(ProgrammingException, "Unknown transcript strand enum value '" + QString::number(strand) + "!");
}

Transcript::STRAND Transcript::stringToStrand(QByteArray strand)
{
	strand = strand.toUpper();
	if (strand=="+")
	{
		return PLUS;
	}
	if (strand=="-")
	{
		return MINUS;
	}

	THROW(ProgrammingException, "Unknown transcript strand string '" + strand + "!");
}

int Transcript::cDnaToGenomic(int coord)
{
	if (coord<1) THROW(ArgumentException, "Invalid cDNA coordinate " + QString::number(coord) + " given for transcript " + name_ +"!");

	int tmp = coord;
	if(strand_==PLUS)
	{
		for (int i=0; i<coding_regions_.count(); ++i)
		{
			const BedLine& line = coding_regions_[i];
			int start = line.start();
			int end = line.end();
			int length = end - start + 1;

			tmp -= length;
			//qDebug() << start << end << length << tmp;

			if (tmp<=0) return end + tmp;
		}
	}
	else
	{
		for (int i=coding_regions_.count()-1; i>=0; --i)
		{
			const BedLine& line = coding_regions_[i];
			int start = line.start();
			int end = line.end();
			int length = end - start + 1;

			tmp -= length;
			//qDebug() << start << end << length << tmp;

			if (tmp<=0) return start - tmp;
		}
	}

	THROW(ArgumentException, "Invalid cDNA coordinate " + QString::number(coord) + " (bigger than coding region) given for transcript " + name_ +"!");
}

Variant Transcript::hgvsToVariant(QString hgvs_c, const FastaFileIndex& genome_idx)
{
	//init
	const Chromosome& chr = regions()[0].chr();
	int start = -1;
	int end = -1;
	Sequence ref;
	Sequence obs;

	//check prerequisites
	if (regions_.count()==0) THROW(ProgrammingException, "Transcript '" + name_ + "' has no regions() defined!");

	//remove prefix
	hgvs_c = hgvs_c.trimmed();
	if (hgvs_c.startsWith("c.")) hgvs_c = hgvs_c.mid(2);
	int length = hgvs_c.length();
	if (length<4) THROW(ProgrammingException, "Invalid cDNA change '" + hgvs_c + "'!");
	//qDebug() << "### cDNA:" << hgvs_c << "###";

	//SNV
	if(hgvs_c.at(length-4).isDigit() && hgvs_c.at(length-3).isLetter() && hgvs_c.at(length-2)=='>' && hgvs_c.at(length-1).isLetter())
	{
		//detemine position
		QString position = hgvs_c.left(length-3);
		int offset = 0;
		hgvsParsePosition(position, start, offset);
		start += strand_==Transcript::PLUS ? offset : -1 * offset;
		end = start;

		//set sequence
		ref.append(hgvs_c[length-3].toUpper());
		obs.append(hgvs_c[length-1].toUpper());

		//convert reference to correct strand
		if(strand_==Transcript::MINUS)
		{
			ref = NGSHelper::changeSeq(ref, true, true);
			obs = NGSHelper::changeSeq(obs, true, true);
		}
	}
	//DUP e.g. "39-286dup" or "289-102_289-100dup"
	else if (hgvs_c.endsWith("dup"))
	{
		//coordinates (of dup)
		QString position = hgvs_c.left(hgvs_c.length()-3);
		int pos_underscore = position.indexOf('_');
		if (pos_underscore!=-1)
		{
			int offset1 = 0;
			hgvsParsePosition(position.left(pos_underscore), start, offset1);
			start += (strand_==Transcript::PLUS ? offset1 : -1 * offset1);
			int offset2 = 0;
			hgvsParsePosition(position.mid(pos_underscore+1), end, offset2);
			end += (strand_==Transcript::PLUS ? offset2 : -1 * offset2);

			if (start>end)
			{
				int tmp = start;
				start = end;
				end = tmp;
			}
		}
		else
		{
			int offset = 0;
			hgvsParsePosition(position, start, offset);
			start += (strand_==Transcript::PLUS ? offset : -1 * offset);
			end = start;
		}

		//sequence
		ref.append('-');
		obs = genome_idx.seq(chr, start, end-start+1);

		//coordinates
		start = start-1; //in GSvar format insertions are to the right of the coordinate
		end = start;
	}
	//DEL e.g. "c.134-3651del" or "c.134-1926_134-1925del"
	else if (hgvs_c.endsWith("del"))
	{
		//coordinates
		QString position = hgvs_c.left(hgvs_c.length()-3);
		int pos_underscore = position.indexOf('_');
		if (pos_underscore!=-1)
		{
			int offset1 = 0;
			hgvsParsePosition(position.left(pos_underscore), start, offset1);
			start += (strand_==Transcript::PLUS ? offset1 : -1 * offset1);
			int offset2 = 0;
			hgvsParsePosition(position.mid(pos_underscore+1), end, offset2);
			end += (strand_==Transcript::PLUS ? offset2 : -1 * offset2);

			if (start>end)
			{
				int tmp = start;
				start = end;
				end = tmp;
			}
		}
		else
		{
			int offset = 0;
			hgvsParsePosition(position, start, offset);
			start += (strand_==Transcript::PLUS ? offset : -1 * offset);
			end = start;
		}

		//sequence
		ref = genome_idx.seq(chr, start, end-start+1);
		obs.append('-');
	}
	//INS+DEL
	else if (hgvs_c.contains("delins"))
	{
		int delins_pos = hgvs_c.indexOf("delins");

		//coordinates
		QString position = hgvs_c.left(delins_pos);
		int pos_underscore = position.indexOf('_');
		if (pos_underscore!=-1)
		{
			int offset1 = 0;
			hgvsParsePosition(position.left(pos_underscore), start, offset1);
			start += (strand_==Transcript::PLUS ? offset1 : -1 * offset1);
			int offset2 = 0;
			hgvsParsePosition(position.mid(pos_underscore+1), end, offset2);
			end += (strand_==Transcript::PLUS ? offset2 : -1 * offset2);
		}
		else
		{
			int offset = 0;
			hgvsParsePosition(position, start, offset);
			start += (strand_==Transcript::PLUS ? offset : -1 * offset);
			end = start;
		}

		//sequence
		ref = genome_idx.seq(chr, start, end-start+1);
		obs = hgvs_c.mid(delins_pos + 6).toLatin1();

		//convert reference to correct strand
		if(strand_==Transcript::MINUS)
		{
			obs = NGSHelper::changeSeq(obs, true, true);
		}
	}
	//INS
	else if (hgvs_c.contains("ins") && hgvs_c.indexOf('_')!=-1)
	{
		int ins_pos = hgvs_c.indexOf("ins");

		//coordinates
		QString position = hgvs_c.left(ins_pos);
		int pos_underscore = position.indexOf('_');
		int offset1 = 0;
		hgvsParsePosition(position.left(pos_underscore), start, offset1);
		start += (strand_==Transcript::PLUS ? offset1 : -1 * offset1);
		int offset2 = 0;
		hgvsParsePosition(position.mid(pos_underscore+1), end, offset2);
		end += (strand_==Transcript::PLUS ? offset2 : -1 * offset2);
		if (start>end)
		{
			int tmp = start;
			start = end;
			end = tmp;
		}

		if (start!=end-1) THROW(ArgumentException, "Insertion '" + hgvs_c + "' has coordinates that are not next to each other: " + QString::number(start) + "/" + QString::number(end) + "!");
		end = start;

		//sequence
		ref.append('-');
		obs.append(hgvs_c.mid(ins_pos+3));

		//convert reference to correct strand
		if(strand_==Transcript::MINUS)
		{
			obs = NGSHelper::changeSeq(obs, true, true);
		}
	}
	else
	{
		QStringList lines;
		lines << "Unsupported cDNA change '" + hgvs_c + "'. Please note:";
		lines << "- Adjacent SNVs changes e.g. 'c.1234CA>TC' are not supported. Spit them in single base changes or format them as 'delins'.";
		lines << "- Duplication must end with 'dup'. Remove everyhing after.";
		lines << "- Deletions must end with 'del'. Remove everyhing after.";
		THROW(ArgumentException, lines.join("\n"));
	}

	//check reference length
	int length_pos = end - start + 1;
	int length_bases = ref.length();
	if(length_pos!=length_bases)
	{
		THROW(ProgrammingException, "HGVS.c '" + name_ + ":" + hgvs_c + "': reference length of coordinates (" + QString::number(length_pos) + ") and sequence (" + QString::number(length_bases) + ") do not match!");
	}

	//left-aign (GSVar variants are always left-aligned)
	Variant variant(chr, start, end, ref, obs);
	//qDebug() << "  After conversion:" << variant.toString(true, -1, true);
	variant.leftAlign(genome_idx);
	//qDebug() << "  Left aligned    :" << variant.toString(true, -1, true);

	return variant;
}

void Transcript::hgvsParsePosition(const QString& position, int& pos, int& offset)
{
	//determine positions of special characters
	QList<int> special_char_positions;
	for (int i=0; i<position.length(); ++i)
	{
		if (!position[i].isDigit()) special_char_positions << i;
	}

	//determine offset
	if (special_char_positions.count()==0)
	{
		pos = cDnaToGenomic(position.toInt());
		offset = 0;
		return;
	}
	else if (special_char_positions.count()==1)
	{
		int s_pos = special_char_positions[0];
		QChar s_char = position[s_pos];

		if (s_char=='+')
		{
			pos = cDnaToGenomic(position.left(s_pos).toInt());
			offset = position.mid(s_pos+1).toInt();
			return;
		}
		else if (s_char=='-')
		{
			pos = cDnaToGenomic(s_pos==0 ? 1 : position.left(s_pos).toInt());
			offset = -1 * position.mid(s_pos+1).toInt();
			return;
		}
		else if (s_char=='*')
		{
			pos = cDnaToGenomic(s_pos==0 ? coding_regions_.baseCount() : position.left(s_pos).toInt());
			offset = position.mid(s_pos+1).toInt();
			return;
		}
	}
	else if (special_char_positions.count()==2)
	{
		int s_pos1 = special_char_positions[0];
		QChar s_char1 = position[s_pos1];
		int s_pos2 = special_char_positions[1];
		QChar s_char2 = position[s_pos2];

		if (s_pos1==0 && s_char1=='-' && s_char2=='-')
		{
			pos = cDnaToGenomic(1);
			offset = -1 * position.mid(1, s_pos2-1).toInt() - position.mid(s_pos2+1).toInt();
			return;
		}
	}

	THROW(ProgrammingException, "Unsupported HGVS.c position string '" + position + "'!");
}
