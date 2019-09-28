/***************************************************************************
 *   Copyright (C) 2003-2006 by Oliver Saal                                *
 *   osaal@gmx.de                                                          *
 *   http://www.oliver-saal.de/software/afutrainer/                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef OSZIPARCHIVE_H
#define OSZIPARCHIVE_H

#include <qfile.h>
#include <qstring.h>
#include <qlist.h>

class CZipFile;
class CZipArchive;

class CZipFileHeader
{
public:
	CZipFileHeader()						{ clear(); }
	~CZipFileHeader()						{}

	void clear();
	void integrate(const CZipFileHeader& head);

	bool readLocalFileHeader(QFile& file);
	void writeLocalFileHeader(QFile& file);

	bool readCentralDirectoryHeader(QFile& file);
	void writeCentralDirectoryHeader(QFile& file);

	inline bool hasDataDescriptor() const	{ return m_uFlags & (1<<3) ? true : false; }
public:
	quint16 m_uVersionMadeBy;				//!< version made by                 2 bytes
	quint16 m_uVersionNeeded;				//!< version needed to extract       2 bytes
	quint16 m_uFlags;						//!< general purpose bit flag        2 bytes
	quint16 m_uCompression;					//!< compression method              2 bytes
	quint16 m_uTime;						//!< last mod file time              2 bytes
	quint16 m_uDate;						//!< last mod file date              2 bytes
	quint32 m_uCRC;							//!< crc-32                          4 bytes
	quint32 m_uSizeCompressed;				//!< compressed size                 4 bytes
	quint32 m_uSizeUncompressed;			//!< uncompressed size               4 bytes
	quint16 m_uDiskNumberStart;				//!< disk number start               2 bytes
	quint16 m_uInternalFileAttributes;		//!< internal file attributes        2 bytes
	quint32 m_uExternalFileAttributes;		//!< external file attributes        4 bytes
	quint32 m_uRelativeOffsetLocalHeader;	//!< relative offset of local header 4 bytes

	QString m_strFileName;					//!< file name (variable size)
	QByteArray m_baExtraField;				//!< extra field (variable size)
	QString m_strComment;					//!< file comment (variable size)

	// helpers
	quint64 m_uLocalHeaderPosition;			//!< Position of the local file header, after the signature field
	quint64 m_uDataPosition;				//!< Position in the file of the data
	quint64 m_uCentralHeaderPosition;		//!< Position of the central directory file header, after the signature field
};

class CZipEndRecord
{
public:
	CZipEndRecord()							{ clear(); }
	~CZipEndRecord()						{ }

	void clear();

	bool read (QFile& file);
	void write (QFile& file);

public:
	quint16 m_uDisk;						//!< number of this disk             2	bytes
	quint16 m_uDiskCentralDir;				//!< number of the disk with the start of the central directory  2 bytes
	quint16 m_uEntriesCentralDirDisk;		//!< total number of entries in the central directory on this disk  2 bytes
	quint16 m_uEntriesCentralDir;			//!< total number of entries in the central directory           2 bytes
	quint32 m_uCentralDirSize;				//!< size of the central directory   4 bytes
	quint32 m_uCentralDirOffset;			//!< offset of start of central directory with respect to the starting disk number        4 bytes

	QString m_strComment;
};

//! Represents a file in a zip archive
class CZipFile
{
public:
	CZipFile()								{ m_pArchive = 0; clear(); }
	CZipFile(CZipArchive *pArchive);
	~CZipFile();
	
	void clear();
	inline bool isValid() const				{ return m_pArchive != 0; }

	//! Reads local file header
	//! Position has to be after the signature field
	//! After the operation, the current position in the file is after the file data
	bool readHeader();

	bool deflateToFile (QIODevice& dev);

	QByteArray deflateToByteArray();
	//QString deflateToString();

//	bool read (FILE *fpt, unsigned uPosition); 

	//! this file has a data descriptor

	inline QString fileName() const			{ return m_head.m_strFileName; }

public:
	CZipArchive *m_pArchive;
	CZipFileHeader m_head;

};

class CZipArchive
{
public:
	CZipArchive();
	~CZipArchive();

	enum OpenMode { OpenReadOnly, OpenCreate, OpenModify };

	bool open (const QString& strFileName, const OpenMode om);

	CZipFile* findFile (const QString& strFileName);

	inline int fileCount() const				{ return m_listFiles.size(); }
	const CZipFile* fileAt(const int i) const	{ return m_listFiles.at(i); }
	CZipFile* fileAt(const int i)				{ return m_listFiles.at(i); }

protected:
	friend class CZipFile;

	QFile m_file;
	QString m_strFileName;

	QList<CZipFile*> m_listFiles;
	QList<CZipEndRecord*> m_listEndRecords;
};

#endif

