/***************************************************************************
 *   Copyright (C) 2003-2007 by Oliver Saal                                *
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

#ifndef CATALOG_H
#define CATALOG_H

#include "chapter.h"
#include "exam.h"

#include <qlist.h>
#include <qtemporaryfile.h>

class CCatalog : public CChapter
{
public:
	CCatalog() : CChapter()									{ clear(); }
	~CCatalog();

	void clear(); 
	bool isEmpty();

	inline QString name() const								{ return m_strText; }
	inline void setName(const QString& strName)				{ m_strText = strName; }

	inline QString contact() const							{ return m_strContact; }
	inline void setContact(const QString& strContact)		{ m_strContact = strContact; }
	inline QString publisher() const						{ return m_strPublisher; }
	inline void setPublisher(const QString& strPublisher)	{ m_strPublisher = strPublisher; }

	// Validation
	inline QDate validFrom() const							{ return m_dateValidFrom; }
	inline void setValidFrom(const QDate& date)				{ m_dateValidFrom = date; }
	inline QDate validUntil() const							{ return m_dateValidUntil; }
	inline void setValidUntil(const QDate& date)			{ m_dateValidUntil = date; }
	bool isValid() const;
	inline QDate created() const							{ return m_dateCreated; }
	inline void setCreated(const QDate& date)				{ m_dateCreated = date; }
	inline QDate published() const							{ return m_datePublished; }
	inline void setPublished(const QDate& date)				{ m_datePublished = date; }
	inline QString versionText() const						{ return m_strVersion; }
	inline void setVersionText(const QString& str)			{ m_strVersion = str; }

	// file operations
	inline QString fileName() const							{ return m_strFileName; }
	bool load (const QString& strFileName, QWidget *pParent);
	bool save (const QString& strFileName, QWidget *pParent);

	bool loadStatistic(QWidget *pParent);
	bool saveStatistic(QWidget *pParent);

	// hints
	inline int countHint() const							{ return m_listHint.size(); }
	inline const CHint& hintAt(int i) const					{ return m_listHint.at(i); }
	inline CHint& hintAt(int i)								{ return m_listHint[i]; }
	inline void appendHint(const CHint& a)					{ m_listHint.append(a); }
	bool hasHints (const QString& strQuestionId) const;
	QString hintText (const QString& strQuestionId) const;

	// exams
	inline int countExam() const							{ return m_listExam.size(); }
	inline const CExam& examAt(int i) const					{ return m_listExam.at(i); }
	inline CExam& examAt(int i)								{ return m_listExam[i]; }
	inline void appendExam(const CExam& a)					{ m_listExam.append(a); }
	CExam examById(const QString& strId) const;

	inline void appendExamStat(const CExamStat& a)			{ m_listExamStat.append(a); }
	inline int countExamStat() const						{ return m_listExamStat.size(); }
	inline const CExamStat& examStatAt(int i) const			{ return m_listExamStat.at(i); }

	// options
	inline bool useMixedAnswers() const						{ return m_bMixAnswers; }

	// others
	QList<CChapter*> chapters();

protected:
	QString statisticFileName() const;
	bool loadExamStatistic (QDomElement& elemRoot);
	void saveExamStatistic (QDomElement& parent, QDomDocument& doc);

protected:
	QString m_strFileName;
	QString m_strUniqueName;
	QList<QTemporaryFile*> m_listFiles;

	QList<CHint> m_listHint;
	QString m_strContact;
	QString m_strPublisher;
	QDate m_dateValidFrom;
	QDate m_dateValidUntil;
	QDate m_dateCreated;
	QDate m_datePublished;
	QString m_strVersion;
	QList<CExam> m_listExam;
	QList<CExamStat> m_listExamStat;

	// Optionen
	bool m_bMixAnswers;
	bool m_bSort;
};

#endif
