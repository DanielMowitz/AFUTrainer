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
	
#ifndef EXAM_H
#define EXAM_H

#include <qstring.h>
#include <qlist.h>
#include <qdom.h>
#include <qdatetime.h>
#include <qstringlist.h>

class CQuestion;

class CExamPart
{
public:
	CExamPart()										{ clear(); }
	~CExamPart()									{}

	void clear();

	inline int questionCount() const				{ return m_iQuestionCount; }
	inline void setQuestionCount(int i)				{ m_iQuestionCount = i; }

	inline QString group() const					{ return m_strGroup; }
	inline void setGroup(const QString& str)		{ m_strGroup = str; }

	inline QString regexp() const					{ return m_strRegExp; }
	inline void setRegExp(const QString& str)		{ m_strRegExp = str; }

	bool load (QDomElement elem);
	void save (QDomElement& parent, QDomDocument& doc) const;

	QList<CQuestion*> createQuestionPool(const QList<CQuestion*>& listAllQuestions) const;

protected:
	int m_iQuestionCount;
	QString m_strGroup;
	QString m_strRegExp;
};

class CExam
{
public:
	CExam()											{ clear(); }
	~CExam()										{}

	//! Zurücksetzen aller Werte
	/*! Es werden alle Daten der Prüfung gelöscht. */
	void clear();

	//! ID abfragen
	inline QString id() const							{ return m_strId; }
	//! ID setzen
	inline void setId(const QString& strId)				{ m_strId = strId; }

	inline QString name() const						{ return m_strName; }
	inline QString comment() const					{ return m_strComment; }
	unsigned duration() const						{ return m_uDuration; }
	unsigned maxErrorPoints() const					{ return m_uMaxErrorPoints; }

	unsigned questionCount() const;

	bool load (QDomElement elem);
	void save (QDomElement& parent, QDomDocument& doc) const;

	QList<CQuestion*> createQuestionPool(const QList<CQuestion*>& listAllQuestions) const;

protected:
	QString m_strId;				//< ID der Prüfung
	QString m_strName;				//< Name / Bezeichnung
	QString m_strComment;			//< Kommentar / Hinweis
	unsigned m_uDuration;			//< Prüfungsdauer in min
	unsigned m_uMaxErrorPoints;		//< Max. Anzahl an erlaubten Fehlerpunkten, um die Prüfung zu bestehen
	QList<CExamPart> m_listParts;	//< Abschnitte, aus denen sich die Prüfung zusammensetzt.
};

class CExamStat
{
public:
	CExamStat()											{ clear(); }
	CExamStat(const CExam& exam);
	~CExamStat()										{}

	//! Zurücksetzen aller Werte
	/*! Es werden alle Daten dieser Prüfungsstatistik gelöscht. */
	void clear();

	bool load (QDomElement elem);
	void save (QDomElement& parent, QDomDocument& doc) const;

	//! ID abfragen
	inline QString id() const							{ return m_strId; }
	//! ID setzen
	inline void setId(const QString& strId)				{ m_strId = strId; }

	inline void setSecs(const unsigned uSecs)			{ m_uSecs = uSecs; }
	void setQuestions(const QList<CQuestion*>& listQuestion, const QList<unsigned>& listAnswerMask);
	void setResult (const unsigned uCorrect, const unsigned uWrong, const unsigned uErrorPoints, const bool bPassed);

	inline unsigned duration() const					{ return m_uSecs; }
	inline bool passed() const							{ return m_bPassed; }
	inline unsigned correctAnswers() const				{ return m_uCorrect; }
	inline unsigned wrongAnswers() const				{ return m_uWrong; }
	inline unsigned errorPoints() const					{ return m_uErrorPoints; }
	QDateTime datetime() const							{ return m_dt; }

protected:
	QString m_strId;				//< ID der Prüfung
	QDateTime m_dt;					//< Datum/Uhrzeit
	unsigned m_uSecs;				//< Benötigte Zeit in sec

	QStringList m_strlQuestionId;	//< IDs der Fragen
	QList<unsigned> m_listAnswer;	//< Antworten

	unsigned m_uCorrect;			//< Anzahl d. richtigen Fragen
	unsigned m_uWrong;				//< Anzahl d. falschen Fragen
	unsigned m_uErrorPoints;		//< Fehlerpunkte
	bool m_bPassed;					//< Bestanden Ja/Nein
};

#endif

