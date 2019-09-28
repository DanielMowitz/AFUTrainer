
#include "osziparchive.h"

#include "zlib/zlib.h"

#include <qbytearray.h>
#include <qdatastream.h>
#include <qtextstream.h>


void CZipFileHeader::clear()
{
	m_uVersionMadeBy = 0;
	m_uVersionNeeded = 0;
	m_uFlags = 0;
	m_uCompression = 0;
	m_uTime = 0;
	m_uDate = 0;
	m_uCRC = 0;
	m_uSizeCompressed = 0;
	m_uSizeUncompressed = 0;
	m_uDiskNumberStart = 0;
	m_uInternalFileAttributes = 0;
	m_uExternalFileAttributes = 0;
	m_uRelativeOffsetLocalHeader = 0;
	m_strFileName.clear();
	m_strComment.clear();
	m_baExtraField.clear();

	// helpers
	m_uDataPosition = 0;
	m_uLocalHeaderPosition = 0;
	m_uCentralHeaderPosition = 0;
}

void CZipFileHeader::integrate(const CZipFileHeader& head)
{
	if (m_uVersionMadeBy == 0) m_uVersionMadeBy = head.m_uVersionMadeBy;
	if (m_uVersionNeeded == 0) m_uVersionNeeded = head.m_uVersionNeeded;
	if (m_uFlags == 0) m_uFlags = head.m_uFlags;
	if (m_uCompression == 0) m_uCompression = head.m_uCompression;
	if (m_uTime == 0) m_uTime = head.m_uTime;
	if (m_uDate == 0) m_uDate = head.m_uDate;
	if (m_uCRC == 0) m_uCRC = head.m_uCRC;
	if (m_uSizeCompressed == 0) m_uSizeCompressed = head.m_uSizeCompressed;
	if (m_uSizeUncompressed == 0) m_uSizeUncompressed = head.m_uSizeUncompressed;
	if (m_uDiskNumberStart == 0) m_uDiskNumberStart = head.m_uDiskNumberStart;
	if (m_uInternalFileAttributes == 0) m_uInternalFileAttributes = head.m_uInternalFileAttributes;
	if (m_uExternalFileAttributes == 0) m_uExternalFileAttributes = head.m_uExternalFileAttributes;
	if (m_uRelativeOffsetLocalHeader == 0) m_uRelativeOffsetLocalHeader = head.m_uRelativeOffsetLocalHeader;

	if (m_strFileName.isEmpty()) m_strFileName = head.m_strFileName;
	if (m_baExtraField.isEmpty()) m_baExtraField = head.m_baExtraField;
	if (m_strComment.isEmpty()) m_strComment = head.m_strComment;

	if (m_uDataPosition == 0) m_uDataPosition = head.m_uDataPosition;
	if (m_uLocalHeaderPosition == 0) m_uLocalHeaderPosition = head.m_uLocalHeaderPosition;
	if (m_uCentralHeaderPosition == 0) m_uCentralHeaderPosition = head.m_uCentralHeaderPosition;
}

bool CZipFileHeader::readLocalFileHeader(QFile& file)
{
QDataStream in(&file);
quint16 uFileNameLength=0, uExtraFieldLength=0;

	clear();
	in.setByteOrder(QDataStream::LittleEndian);

	m_uLocalHeaderPosition = file.pos();

	in >> m_uVersionNeeded;
	in >> m_uFlags;
	in >> m_uCompression;
	in >> m_uTime;
	in >> m_uDate;
	in >> m_uCRC;
	in >> m_uSizeCompressed;
	in >> m_uSizeUncompressed;
	in >> uFileNameLength;
	in >> uExtraFieldLength;

	if (uFileNameLength != 0)
	{
		QByteArray a (uFileNameLength, 0);
		in.readRawData(a.data(), uFileNameLength);
		m_strFileName = a;
	}
	if (uExtraFieldLength != 0)
	{
		m_baExtraField.resize(uExtraFieldLength);
		in.readRawData(m_baExtraField.data(), uExtraFieldLength);
	}

	m_uDataPosition = file.pos();
	in.skipRawData(m_uSizeCompressed);

	if (hasDataDescriptor())
	{
		in >> m_uCRC;
		in >> m_uSizeCompressed;
		in >> m_uSizeUncompressed;
	}
	return true;
}

void CZipFileHeader::writeLocalFileHeader(QFile& file)
{
	Q_UNUSED(file);
}

bool CZipFileHeader::readCentralDirectoryHeader(QFile& file)
{
QDataStream in(&file);
quint16 uFileNameLength=0, uExtraFieldLength=0, uCommentLength=0;

	clear();
	in.setByteOrder(QDataStream::LittleEndian);

	m_uCentralHeaderPosition = file.pos();

	in >> m_uVersionMadeBy;
	in >> m_uVersionNeeded;
	in >> m_uFlags;
	in >> m_uCompression;
	in >> m_uTime;
	in >> m_uDate;
	in >> m_uCRC;
	in >> m_uSizeCompressed;
	in >> m_uSizeUncompressed;
	in >> uFileNameLength;
	in >> uExtraFieldLength;
	in >> uCommentLength;
	in >> m_uDiskNumberStart;
	in >> m_uInternalFileAttributes;
	in >> m_uExternalFileAttributes;
	in >> m_uRelativeOffsetLocalHeader;

	if (uFileNameLength != 0)
	{
		QByteArray a (uFileNameLength, 0);
		in.readRawData(a.data(), uFileNameLength);
		m_strFileName = a;
	}
	if (uExtraFieldLength != 0)
	{
		m_baExtraField.resize(uExtraFieldLength);
		in.readRawData(m_baExtraField.data(), uExtraFieldLength);
	}

	if (uCommentLength != 0)
	{
		QByteArray a (uCommentLength, 0);
		in.readRawData(a.data(), uCommentLength);
		m_strComment = a;
	}
	return true;
}

void CZipFileHeader::writeCentralDirectoryHeader(QFile& file)
{
	Q_UNUSED(file);
}

void CZipEndRecord::clear()
{
	m_uDisk = 0;
	m_uDiskCentralDir = 0;
	m_uEntriesCentralDirDisk = 0;
	m_uEntriesCentralDir = 0;
	m_uCentralDirSize = 0;
	m_uCentralDirOffset = 0;
	m_strComment.clear();
}

bool CZipEndRecord::read (QFile& file)
{
QDataStream in(&file);
quint16 uCommentLength=0;

	clear();
	in.setByteOrder(QDataStream::LittleEndian);

	in >> m_uDisk;
	in >> m_uDiskCentralDir;
	in >> m_uEntriesCentralDirDisk;
	in >> m_uEntriesCentralDir;
	in >> m_uCentralDirSize;
	in >> m_uCentralDirOffset;
	in >> uCommentLength;

	if (uCommentLength != 0)
	{
		QByteArray a (uCommentLength, 0);
		in.readRawData(a.data(), uCommentLength);
		m_strComment = a;
	}
	return true;
}

void CZipEndRecord::write (QFile& file)
{
	Q_UNUSED(file);
}


CZipFile::CZipFile(CZipArchive *pArchive)
{
	m_pArchive = pArchive;
	clear();
}

CZipFile::~CZipFile()
{
}

void CZipFile::clear()
{
}

bool CZipFile::readHeader ()
{
QFile& file = m_pArchive->m_file;

	return (m_head.readLocalFileHeader(file));
}

QByteArray CZipFile::deflateToByteArray()
{
/*QByteArray a, b;

	if (m_head.m_uDataPosition == 0 || m_head.m_uSizeUncompressed == 0) return QByteArray();

	m_pArchive->m_file.seek (m_head.m_uDataPosition);
	a = m_pArchive->m_file.read (m_head.m_uSizeCompressed);

	b.resize(m_head.m_uSizeUncompressed);
	quint32 u = m_head.m_uSizeUncompressed;
	Bytef* src = (Bytef*)a.data();
	Bytef* dst = (Bytef*)b.data();
	if (uncompress(dst, (uLongf*) &u, src, m_head.m_uSizeCompressed) != Z_OK)
		return QByteArray();

	return b;*/

QByteArray a;
z_stream strm;
char *pIn=0, *pOut=0;
int ret;

	memset(&strm, 0, sizeof(z_stream));
	if (m_head.m_uDataPosition == 0 || m_head.m_uSizeUncompressed == 0) return QByteArray();

	// prepare zip-stream
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END.
		 */
	if (inflateInit2(&strm, -MAX_WBITS) != Z_OK) return QByteArray();

	// read data
	pIn = new char[m_head.m_uSizeCompressed+1];
	memset (pIn, 0, m_head.m_uSizeCompressed+1);
	m_pArchive->m_file.seek (m_head.m_uDataPosition);
	m_pArchive->m_file.read (pIn, m_head.m_uSizeCompressed);

	// prepare output
	pOut = new char[m_head.m_uSizeUncompressed];
	memset(pOut, 0, m_head.m_uSizeUncompressed);

/*
// DEBUG START
	FILE *fpt;
	fpt = fopen ("questions.xml", "rt");
	fread (pOut, m_head.m_uSizeUncompressed, 1, fpt);
	fclose (fpt);

	unsigned u = m_head.m_uSizeCompressed;
	fpt = fopen ("test2.gz", "wb");
	compress2((Bytef*)pIn, (uLongf*) &u, (Bytef*)pOut, m_head.m_uSizeUncompressed, 9);
	fwrite (pIn, u, 1, fpt);
	fclose (fpt);
// DEBUG ENDE
*/
	do
	{
		strm.avail_in = m_head.m_uSizeCompressed+1;
		strm.next_in = (Bytef*) pIn;
		do
		{
			strm.avail_out = m_head.m_uSizeUncompressed;
			strm.next_out = (Bytef*) pOut;
			ret = inflate (&strm, Z_SYNC_FLUSH);
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				delete [] pIn;
				delete [] pOut;
				return QByteArray();
            }
		}
		while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	}
	while (ret != Z_STREAM_END);

	inflateEnd(&strm);

	a = QByteArray (pOut, m_head.m_uSizeUncompressed);
	delete [] pIn;
	delete [] pOut;

	return a;
}
/*
QString CZipFile::deflateToString()
{
QByteArray a = deflateToByteArray();
QTextStream in (a, QIODevice::ReadOnly);
//QString str;
//	in >> str;
	return in.readAll();
}
*/

#define CHUNK (1<<16)

bool CZipFile::deflateToFile (QIODevice& dev)
{
z_stream strm;
char cIn[CHUNK], cOut[CHUNK];
int ret;

	if ((dev.openMode() & QIODevice::WriteOnly) == 0)
		return false;

	memset(&strm, 0, sizeof(z_stream));
	if (m_head.m_uDataPosition == 0 || m_head.m_uSizeUncompressed == 0) return false;

	// prepare zip-stream
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END.
		 */
	if (inflateInit2(&strm, -MAX_WBITS) != Z_OK) return false;

	// prepare archive
	m_pArchive->m_file.seek (m_head.m_uDataPosition);

	// unzip
	unsigned uRead=0, uBytes=0;
	do
	{
		uBytes = m_pArchive->m_file.read(cIn, CHUNK);
		if (uRead + uBytes > m_head.m_uSizeCompressed)
			uBytes = m_head.m_uSizeCompressed - uRead + 1; // one dummy byte extra
		uRead += uBytes;

		strm.avail_in = uBytes;
		if (strm.avail_in == 0) break;
		strm.next_in = (Bytef*) cIn;

		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = (Bytef*) cOut;
			ret = inflate (&strm, Z_NO_FLUSH);
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				return false;
            }
			unsigned uHave = CHUNK - strm.avail_out;
			dev.write(cOut, uHave);
		}
		while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	}
	while (ret != Z_STREAM_END);

	inflateEnd(&strm);
	return true;
}


CZipArchive::CZipArchive()
{
}

CZipArchive::~CZipArchive()
{
	qDeleteAll(m_listFiles);
	qDeleteAll(m_listEndRecords);
}

bool CZipArchive::open (const QString& strFileName, const OpenMode om)
{
unsigned uSignature=0;
CZipFile *pzf=0;

	if (strFileName.isEmpty()) return false;
	if (om != OpenReadOnly) return false;

	m_strFileName = strFileName;
	m_file.close();
	m_file.setFileName(strFileName);
	if (!m_file.open(QIODevice::ReadOnly)) return false;
	QDataStream in(&m_file);
	in.setByteOrder(QDataStream::LittleEndian);

	while (!in.atEnd())
	{
		uSignature = 0;
		in >> uSignature;
		if (uSignature == 0x04034b50)
		{ // local file header with data
			pzf = new CZipFile(this);
			if (pzf->readHeader())
				m_listFiles.append(pzf);
			//qDebug("%7i %7i %s", pzf->m_head.m_uSizeUncompressed, pzf->m_head.m_uSizeCompressed, qPrintable(pzf->fileName()));
//			file.deflateToFile("test.tmp");
		}
		else if (uSignature == 0x02014b50)
		{ // central directory file header
			CZipFileHeader head;
			head.readCentralDirectoryHeader(m_file);
			pzf = findFile(head.m_strFileName);
			if (pzf) pzf->m_head.integrate(head);
		}
		else if (uSignature == 0x06054b50)
		{ // End of central directory record
			CZipEndRecord *pzer = new CZipEndRecord();
			pzer->read(m_file);
			m_listEndRecords.append(pzer);
		}
		else
		{
			qDebug("Unknown signature: %X", uSignature);
			break;
		}
	}

	return true;
}

CZipFile* CZipArchive::findFile (const QString& strFileName)
{
	for (int i=0; i<m_listFiles.size(); i++)
	{
		if (m_listFiles.at(i)->fileName() == strFileName)
			return m_listFiles.at(i);
	}

	return 0;
}

