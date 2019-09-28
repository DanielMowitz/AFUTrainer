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

#ifndef CHAPTER_H
#define CHAPTER_H

#include "question.h"
//#include "recommendation.h"
//#include "chapterstatistic.h"

#include <qmap.h>

//! Die Klasse CChapter speichert ein Kapitel mit allen Unterkapiteln und Fragen
/*!
*/

class CChapter
{
public:
	//! Empfehlung für Kapitel
	enum Recommendation
	{
		RecommendationNone=0,				//!< Keine Emfehlung
		RecommendationSubChapter=1,			//!< Untergeordnetes Kapitel lernen
		RecommendationParentChapter=2,		//!< Übergeordnetes Kapitel lernen
		RecommendationRepeatToday=3,		//!< Kapitel heute wiederholen
		RecommendationLearnNew=4,			//!< Neue Fragen lernen
		RecommendationRepeatLater=5,		//!< Kapitel später wiederholen
		RecommendationMax=6					//!< Für for-Schleifen, etc.
	};

	//! Standard-Konstruktor
	/*! Initialisiert die Klasse, indem die Funktion clear() aufgerufen wird. */
	CChapter()											{ clear(); }

	//! Standard-Destruktor
	/*! Es werden alle Unterkapitel und Fragen dieses Kapitels aus dem Speicher gelöscht. */
	~CChapter()											{ qDeleteAll(m_listChapter); qDeleteAll(m_listQuestion); }

	//! Zurücksetzen aller Werte
	/*! Es werden alle Unterkapitel und Fragen dieses Kapitels aus dem Speicher gelöscht und alle andere Daten auf die Default-Werte zurückgesetzt. */
	void clear(); 

	//! Elternkapitel auslesen
	/*!
	\return Elternkapitel m_pParentChapter
	\sa setParentChapter(), m_pParentChapter
	*/
	inline CChapter* parentChapter() const				{ return m_pParentChapter; }

	//! Elternkapitel setzen
	/*!
	Durch diese Funktion wird lediglich die Variable m_pParentChapter gesetzt, jedoch nicht die Statistiken 
	des Eltern-Kapitels aktualisiert.

	\param pChapter Das zu setzende Elternkapitel dieses Kapitels	
	\sa parentChapter(), m_pParentChapter
	*/
	inline void setParentChapter(CChapter *pChapter)	{ m_pParentChapter = pChapter; }

	//! ID dieses Kapitels auslesen
	inline QString id() const							{ return m_strId; }
	//! ID dieses Kapitels zusammengesetzt mit allen IDs der Eltern-Kapitel auslesen
	QString idWithParents() const;
	//! Kapitelbeschreibung auslesen
	inline QString text() const							{ return m_strText; }
	//! Kommentar
	inline QString comment() const						{ return m_strComment; }

	//! ID setzen
	inline void setId (const QString& strId)			{ m_strId = strId; }
	//! Kapitelbeschreibung setzen
	inline void setText (const QString& strText)		{ m_strText = strText; }
	//! Kommentar setzen
	inline void setComment (const QString& strComment)	{ m_strComment = strComment; }

	//! Anhängen eines Textes an die Kapitelbeschreibung
	/*! 
	\param strText Anzuhängender Text
	\sa m_strText
	*/
	inline void appendText(const QString& strText)		{ m_strText += strText; }
	

	//! Anzahl der Unterkapitel ermitteln
	/*! \return Anzahl der Unterkapitel */
	inline int countChapter() const						{ return m_listChapter.size(); }
	inline const CChapter* chapterAt(int i) const		{ return m_listChapter.at(i); }
	inline int indexOfChapter(CChapter* c) const		{ return m_listChapter.indexOf(c); }
	inline void appendChapter(CChapter* c)				{ m_listChapter.append(c); c->setParentChapter(this); }
//	inline void removeChapter(int i)					{ delete m_listChapter.takeAt(i); }
	QList<CChapter*> subChapters() const;
	inline void sortAll()								{ sortSubChapters(true); sortQuestions(); }
	void sortSubChapters(bool bSortQuestions);
	void sortQuestions();

	//! Anzahl der Fragen dieses Kapitels und aller Unterkapitel ermitteln
	/*! \return Anzahl der Fragen */
	int countSubQuestion() const;
	//! Anzahl der Fragen dieses Kapitels (ohne Unterkapitel) ermitteln
	/*! \return Anzahl der Fragen */
	inline int countQuestion() const					{ return m_listQuestion.size(); }
	inline const CQuestion* questionAt(int i) const		{ return m_listQuestion.at(i); }
	inline CQuestion* questionAt(int i)					{ return m_listQuestion.at(i); }
	inline void appendQuestion(CQuestion* q)			{ m_listQuestion.append(q); q->setParentChapter(this); }

	QList<CQuestion*> questionPool() const;
	QList<CQuestion*> questionPoolLevel(const unsigned uLevel) const;
	QList<CQuestion*> questionPoolDeepen() const;
	QList<CQuestion*> questionPoolRepeat(const QDate d=QDate::currentDate()) const;




	bool load (QDomElement elem);
	void save (QDomElement& parent, QDomDocument& doc);
	bool loadLearnStatistic (QDomElement elem);
	bool saveLearnStatistic (QDomElement& parent, QDomDocument& doc);

	QString checkForErrors() const;

	
	// Kapitelstatistik auslesen
	/* 
	\return Kapitelstatistik (Variable m_cs) 
	\sa m_cs 
	*/
//	inline CChapterStatistic statistic() const									{ return m_cs; }


	//! @name Funktionen zur Statistik
	//@{
	void updateStatistic();														//!< Kapitelstatistik aktualisieren
	//! Anzahl der Fragen einer bestimmten Abfragehäufigkeit
	inline unsigned countQuestion(const unsigned uLevel) const					{ return m_uLevelCount[uLevel]; }
	double levelAvg() const;													//!< Durchschnittlicher Lernfortschritt des Kapitels
	unsigned levelAvgRounded() const;											//!< Durchschnittlicher, gerundeter Lernfortschritt des Kapitels
	QString levelAvgText() const;												//!< Durchschnittlicher Lernfortschritt als Text
	QIcon levelAvgIcon() const;													//!< Icon zum durchschnittlichen Lernfortschritt
	QPixmap levelAvgPixmap() const;												//!< Pixmap zum durchschnittlichen Lernfortschritt
	//! Anzahl der noch nie abgefragen Fragen
	inline unsigned countNeverAsked() const										{ return m_uNeverAskedCount; }
	//!< Kapitel inkl. Unterkapitel enthalten Fragen, die gerade gelernt werden
	inline bool hasLearningNewQuestions() const									{ return m_bHasLearningNew; }
	inline bool hasKnownQuestions() const										{ return m_bHasKnownQuestions; }
	inline bool hasKnownQuestionsRepeatToday() const							{ return m_bHasKnownQuestionsRepeatToday; }

	CDayStatistic dayStatistic (const QDate& date) const;						//<! Statistik eines Tages
	CDayStatistic completeStatistic() const;									//<! Statistik des kompletten Zeitraums
	QDateTime firstAnswerClicked() const;										//<! Datum, zu dem zum allerersten Mal eine Frage beantwortet wurde
	//@}

	//! @name Funktionen zur Lernempfehlung
	//@{
	QDate repeatDate() const;													//!< Wiederhol-Datum des Kapitels
	//! Lernempfehlung für das Kapitel
	/*! \return Lernempfehlung (Variable m_recom) \sa m_recom */
	inline Recommendation recommendation() const								{ return m_recom; }
	QString recommendationIconName(const CCatalog *pCatalog) const;				//!< Name des Icons zur Lernempfehlung ermitteln
	//! Ermittelt Icon zur Lernempfehlung
	inline QIcon recommendationIcon (const CCatalog *pCatalog) const			{ return QIcon (recommendationIconName(pCatalog)); }
	QString recommendationText() const;											//!< Lernempfehlungstext (kurz) 
	QString recommendationTextExtended(const CCatalog *pCatalog) const;			//!< Lernempfehlungstext (ausführlich)
	QString recommendationToolTip() const;										//!< Lernempfehlungstext für Tooltip
	bool isRecommendedNow(const CCatalog *pCatalog) const;						//!< Kapitel sollte jetzt gelernt werden
	//! Anzahl der Unterkapitel mit einer bestimmten Lernempfehlung
	inline unsigned recommendationCount(const Recommendation r) const			{ return m_uRecomCount[r]; }
	//! Fragen, die beantwortet müssen, um die Lernempfehlung zu erfüllen
	inline QList<CQuestion*> recommendedQuestions() const						{ return m_listQuestionRecommended; }
	//! Kapitel hat Fragen, die beantwortet müssen, um die Lernempfehlung zu erfüllen
	inline bool hasRecommendedQuestions() const									{ return m_listQuestionRecommended.size() > 0; }
	//! Anzahl der Fragen, die beantwortet müssen, um die Lernempfehlung zu erfüllen
	inline int recommendedQuestionCount() const									{ return m_listQuestionRecommended.size(); }
	//! Alternative Lernempfehlung für das Kapitel
	inline Recommendation recommendation2() const								{ return m_recom2; }
	QString recommendationTextExtended2(const CCatalog *pCatalog) const;		//!< Alternativer Lernempfehlungstext (ausführlich)
	//@}

	//! @name Funktionen zu Prüfungen
	//@{
//	inline void setExam(const QString& strId, const unsigned uQuestionCount)	{ m_mapExam.insert(strId, uQuestionCount); }
//	inline unsigned examQuestionCount(const QString& strId) const				{ return m_mapExam[strId]; }
	//@}

	// static
	static QString tr (const char *sourceText, const char *comment=0);
	static QString recommendationText(const Recommendation r, const QDate dRepeat);
	static QString recommendationIconName(const Recommendation r, const CCatalog *pCatalog);

protected:
	void updateStatisticCount();				//!< Zählt alle Fragen des Kapitels (inkl. Unterkapitel) für die Statistik
	void updateRecommendation();				//!< Lernempfehlung für das Kapitel (inkl. Unterkapitel) aktualisieren
	void updateRecommendationStatistic();		//!< Statistik über Lernempfehlungen für das Kapitel (inkl. Unterkapitel) aktualisieren
	Recommendation recommendationIndividual() const;	//!< Empfehlung nur für dieses Kapitel, Unterkapitel und Elternkapitel werden ignoriert

protected:
	CChapter *m_pParentChapter;					//!< Übergeordnetes Kapitel
	QString m_strId;							//!< ID des Kapitels
	QString m_strText;							//!< Name des Kapitels
	QString m_strComment;						//!< Kommentar

	QList<CChapter*> m_listChapter;				//!< Liste mit Unterkapiteln
	QList<CQuestion*> m_listQuestion;			//!< Liste mit Fragen

	// Statistik
	unsigned m_uLevelCount[LEVEL_MAX+1];		//!< Anzahl der Fragen einer bestimmten Abfragehäufigkeit
	unsigned m_uNeverAskedCount;				//!< Anzahl der Fragen, die noch nie abgefragt wurden
	unsigned m_uRecomCount[RecommendationMax];	//!< Anzahl der Unterkapitel (inkl. selbst) mit bestimmter Lernempfehlung
	bool m_bHasLearningNew;						//!< Kapitel enthält Fragen, die gerade neu gelernt werden
	bool m_bHasKnownQuestions;					//!< Kapitel enthält Fragen, die seit mind. gestern bekannt sind
	bool m_bHasKnownQuestionsRepeatToday;		//!< Kapitel enthält Fragen, die seit mind. gestern bekannt sind und heute wiederholt werden sollten

	Recommendation m_recom;						//!< Lernempfehlung
	Recommendation m_recom2;					//!< Alternative Lernempfehlung

	//! Empfohlenes Datum zur Wiederholung der Fragen
	/*! Diese Variable wird mit der Funktion updateRecommendation() aktualisiert */
	QDate m_recomRepeatDate;
	//! Liste mit Fragen zum Erfüllen der Lernempfehlung
	/*! Diese Liste wird mit der Funktion updateRecommendation() aktualisiert */
	QList<CQuestion*> m_listQuestionRecommended;
};

#endif
