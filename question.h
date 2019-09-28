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

#ifndef QUESTION_H
#define QUESTION_H

#include <qstring.h>
#include <qdom.h>
#include <qlist.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qicon.h>
#include <qpixmap.h>

#include "answer.h"
#include "helper.h"

unsigned afu_random (const unsigned uMin, const unsigned uMax);

class CChapter;

#define LEVEL_VERYOFTEN 0
#define LEVEL_OFTEN 1
#define LEVEL_NORMAL 2
#define LEVEL_RARE 3
#define LEVEL_VERYRARE 4
#define LEVEL_EXTREMERARE 5
#define LEVEL_MAX 5

class CDayStatistic
{
public:
	CDayStatistic()										{ clear(); }
	~CDayStatistic()									{}

	void clear();

	inline QDate date() const							{ return m_date; }
	
	inline unsigned clickedCount() const				{ return m_uClickedWrong + m_uClickedCorrect; }
	inline unsigned clickedCorrect() const				{ return m_uClickedCorrect; }
	inline unsigned clickedWrong() const				{ return m_uClickedWrong; }
	inline unsigned timeExpediture() const				{ return m_uTimeExpeditureCorrect + m_uTimeExpeditureWrong; }
	inline unsigned timeExpeditureCorrect() const		{ return m_uTimeExpeditureCorrect; }
	inline unsigned timeExpeditureWrong() const			{ return m_uTimeExpeditureWrong; }

	inline unsigned levelRounded() const				{ return (unsigned) (m_dLevel + 0.5); }
	inline double level() const							{ return m_dLevel; }

	CDayStatistic& operator += (const CDayStatistic& ds);

	void debug() const;

protected:
	QDate m_date;
	unsigned m_uClickedWrong;
	unsigned m_uClickedCorrect;
	unsigned m_uTimeExpeditureCorrect;
	unsigned m_uTimeExpeditureWrong;
	double m_dLevel;

	friend class CQuestion;
	friend class CChapter;
};

//! CQuestion speichert eine Frage mit allen Antworten und der Klick-Statistik.

class CQuestion
{
public:
	//! Standard-Konstruktor
	/*! Initialisiert die Klasse, indem die Funktion clear() aufgerufen wird. */
	CQuestion()										{ clear(); }
	//! Standard-Destruktor
	/*! In der Standard-Implementierung tut der Destruktor nichts. */
	~CQuestion()									{}

	/** @name Basisdaten
	* Auslesen und setzen der Basisdaten einer Frage
	*/
	//@{
	//! Zurücksetzen aller Werte
	/*! Es werden alle Daten der Frage gelöscht. */
	void clear();

	//! Elternkapitel abfragen
	/*!
	\return Elternkapitel m_pParentChapter
	\sa CChapter, setParentChapter(), m_pParentChapter
	*/
	inline CChapter* parentChapter() const				{ return m_pParentChapter; }

	//! Elternkapitel setzen
	/*!
	Durch diese Funktion wird lediglich die Variable m_pParentChapter gesetzt, jedoch nicht die Statistiken 
	des Eltern-Kapitels aktualisiert.

	\param pChapter Das zu setzende Elternkapitel der Frage	
	\sa CChapter, parentChapter(), m_pParentChapter
	*/
	inline void setParentChapter(CChapter *pChapter)	{ m_pParentChapter = pChapter; }
	//! ID abfragen
	inline QString id() const							{ return m_strId; }
	//! Text abfragen
	inline QString text() const							{ return m_strText; }

	//! ID setzen
	inline void setId(const QString& strId)				{ m_strId = strId; }
	//! Text setzen
	inline void setText(const QString& strText)			{ m_strText = strText; }

	//! Anhängen eines Textes an den Fragentext
	/*! 
	\param strText Anzuhängender Text
	\sa m_strText
	*/
	inline void appendText(const QString& strText)		{ m_strText += strText; }
	//@}

	QString plainText() const;
	QString firstTextLine() const;

	QString showText(CCatalog *pCatalog) const;
	QString learnText(CCatalog *pCatalog, const bool bShowId=true, const bool bShowHints=false);

	inline unsigned errorPoints() const					{ return m_uErrorPoints; }
	inline void setErrorPoints (const unsigned u)		{ m_uErrorPoints = u; }

	QStringList groups() const							{ return m_strlGroups; }

	bool load (QDomElement elem);
	void save (QDomElement& parent, QDomDocument& doc);
	static QString removeTempPath(const QString& strText);
	bool loadLearnStatistic (QDomElement elem);
	bool saveLearnStatistic (QDomElement& parent, QDomDocument& doc);
	QString checkForErrors() const;

	// answers
	inline int countAnswer() const						{ return m_listAnswer.size(); }
	inline const CAnswer& answerAt(int i) const			{ return m_listAnswer.at(i); }
	inline CAnswer& answerAt(int i)						{ return m_listAnswer[i]; }
	inline void appendAnswer(const CAnswer& a)			{ m_listAnswer.append(a); }
	int countCorrectAnswer() const;

	// helpers
//	inline bool hasHints() const						{ return !m_listHint.isEmpty(); }

	// learning
	void mixAnswers(const bool bMix=true);
	const CAnswer& learnAnswerAt(int i) const;
	unsigned correctAnswerMask() const;
	inline bool isCorrectAnswer (const unsigned uAnswerMask) const	{ return uAnswerMask == correctAnswerMask(); }
	unsigned orderedAnswerMask(const unsigned uMixedAnswerMask);
	void registerAnswerClicked (const unsigned uAnswerMask, const unsigned uNeededTime);
	static QString answerMaskToString(const unsigned uAnswerMask);
	QString correctionText(const unsigned uAnswerMask);

	// statistics
	//! Abfragehäufigkeit der Frage
	inline unsigned level() const						{ return m_uLevel; }
	inline QString levelIconName() const				{ return levelIconName (m_uLevel); }
	inline QIcon levelIcon() const						{ return levelIcon (m_uLevel); }
	inline QPixmap levelPixmap() const					{ return QPixmap(levelIconName (m_uLevel)); }
	inline QString levelText() const					{ return levelText (m_uLevel); }
	inline int clickedCount() const						{ return m_listAnswerClicked.size(); }
	inline unsigned clickedCorrect() const				{ return m_uCorrectAnswers; }
	inline unsigned clickedWrong() const				{ return m_uWrongAnswers; }
	inline unsigned clickedCorrectSuccessive() const	{ return m_uCorrectSuccessive; }
	inline unsigned clickedWrongSuccessive() const		{ return m_uWrongSuccessive; }
	inline bool isNeverAsked() const					{ return m_listAnswerClicked.isEmpty(); }

	unsigned levelAtEndOfDay(const QDate& date) const;	//!< Lernfortschritt am Ende eines bestimmten Tages
	CDayStatistic dayStatistic(const QDate& date) const;

	QDateTime firstClicked() const;
	QDateTime lastClicked() const;
	QString lastClickedText() const;
	QString lastClickedTextExtended() const;
	QDate repeatDate() const;							//!< Wiederhol-Datum der Frage bestimmen
	QString repeatDateText() const;
	QString repeatDateTextExtended() const;
	bool isRepeatToday() const;							//!< Frage sollte heute wiederholt werden

	bool isLearningNew() const;							//!< Ist dies eine neue Frage, die gerade gelernt wird?
	bool isKnownQuestion() const;						//!< Ist die Frage schon (länger) bekannt?

	//! Name des Icons einer bestimmten Abfragehäufigkeit ermitteln
	static QString levelIconName(const unsigned uLevel);
	//! Icon einer bestimmten Abfragehäufigkeit
	static QIcon levelIcon(const unsigned uLevel);
	//! Name einer bestimmten Abfragehäufigkeit
	static QString levelText(const unsigned uLevel);
	static QString tr (const char *sourceText, const char *comment=0);
	//! Berechnung der Wartezeit für eine bestimmte Abfragehäufigkeit
	static unsigned waitDaysForRepeat (const unsigned uLevel);

protected:
	
	CChapter *m_pParentChapter;			//!< Zeiger auf das (Eltern-) Kapitel, zu dem die Frage gehört.
	// masterdata
	QString m_strId;					//!< ID der Frage
	QString m_strText;					//!< Fragentext
	QList<CAnswer> m_listAnswer;		//!< Liste mit Antworten
	unsigned m_uErrorPoints;			//!< Fehlerpunkte bei Falschantwort in Prüfung
	QStringList m_strlGroups;			//!< Liste der Klassen/Gruppen, zu denen diese Frage gehört

	// learning
	//! Durchmischte Antworten
	/*! Die Liste enthält die Positionen der Liste m_listAnswer */
	QList<int> m_listMixedAnswer;		

    // Statistics
	QList<CAnswerClicked> m_listAnswerClicked;	//!< Liste mit sämtlichen Antwort-Klicks
	unsigned m_uCorrectAnswers;					//!< Anzahl der richtigen Antworten
	unsigned m_uCorrectSuccessive;				//!< Anzahl der richtigen Antworten zuletzt hintereinander
	unsigned m_uWrongAnswers;					//!< Anzahl der falschen Antworten
	unsigned m_uWrongSuccessive;				//!< Anzahl der falschen Antworten zuletzt hintereinander

	//! Aktuelle Stufe der Abfragehäufigkeit
	/*!
	Stufe 0: sehr häufige ... Stufe 5: sehr seltene Abfrage

	Die Abfragehäufigkeit wird nicht in der Statistik-Datei gespeichert, sondern bei jedem Laden der 
	Datei anhand der Beantwortungen in der Vergangenheit neu berechnet.
	*/
	unsigned m_uLevel;
};

#endif
