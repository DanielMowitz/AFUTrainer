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

#ifndef ANSWER_H
#define ANSWER_H

#include <qstring.h>
#include <qdom.h>
#include <qlist.h>
#include <qdatetime.h>

//! Die Klasse CAnswer speichert eine einzelne Antwort auf eine Frage
/*!
Das Schreiben und Lesen von XML-Daten wird von der Klasse CQuestion übernommen
*/

class CAnswer
{
public:
	//! Standard-Konstruktor
	/*! Initialisiert die Klasse, indem die Funktion clear() aufgerufen wird. */
	CAnswer()										{ clear(); }
	
	//! Konstruktor inkl. setzen eines Textes und ob die Antwort richtig ist
	/*! 
	\param strText Antworttext
	\param bCorrect true: Die Antwort ist korrekt
	\sa m_strText, m_bIsCorrect
	*/
	CAnswer(const QString& strText, const bool bCorrect);

	//! Standard-Destruktor
	/*! In der Standard-Implementierung tut der Destruktor nichts. */
	~CAnswer()										{}

	//! Zurücksetzen aller Werte
	/*! Es werden alle Daten der Antwort gelöscht. */
	void clear();
	//! Überprüfen, ob die Klasse eine Antwort enthält 
	/*! \return true: Die Klasse ist leer und enthält keine Antwort */
	inline bool isEmpty() const						{ return m_strText.isEmpty(); }

	//! Auslesen, ob diese Antwort richtig ist
	/*! \return true: Die Antwort ist richtig */
	inline bool isCorrect() const					{ return m_bIsCorrect; }
	//! Auslesen des Antworttextes
	/*! \return Antworttext */
	inline QString text() const						{ return m_strText; }
	//! Setzen, ob diese Antwort richtig ist
	/*! 
	\param bCorrect true: Die Antwort ist korrekt
	\sa m_bIsCorrect
	*/
	inline void setCorrect (const bool bCorrect)	{ m_bIsCorrect = bCorrect; }
	//! Setzen des Antworttextes
	/*! 
	\param strText Antworttext
	\sa m_strText
	*/
	inline void setText(const QString& strText)		{ m_strText = strText; }

	//! Anhängen eines Textes an den Antworttext
	/*! 
	\param strText Anzuhängender Text
	\sa m_strText
	*/
	inline void appendText(const QString& strText)	{ m_strText += strText; }

protected:
	//! Antworttext
	/*! 
	Der Antworttext darf HTML-Code und HTML-Verweise auf Grafiken/Bilder enthalten. Bei Verweisen ist zu achten, dass kein Verzeichnis angegeben werden darf!

	Default: leer 
	*/
	QString m_strText;	
	//! Richtige oder falsche Antwort
	/*! Default: false */
	bool m_bIsCorrect;	
};

//! Die Klasse CAnswerClicked speichert eine einzelne Beantwortung des Benutzers
/*!
*/

class CAnswerClicked
{
public:
	//! Standard-Konstruktor
	/*! Initialisiert die Klasse, indem die Funktion clear() aufgerufen wird. */
	CAnswerClicked ()								{ clear(); }
	CAnswerClicked (const unsigned uAnswer, const unsigned uNeededTime)			
													{ m_uAnswer = uAnswer; m_dt = QDateTime::currentDateTime(); m_uNeededTime = uNeededTime; }
	//! Standard-Destruktor
	/*! In der Standard-Implementierung tut der Destruktor nichts. */
	~CAnswerClicked ()								{}

	//! Zurücksetzen aller Werte
	/*! Es werden alle Daten auf Default-Werte zurückgesetzt. */
	void clear();

	inline QDateTime dateTime() const				{ return m_dt; }
	inline unsigned neededTime() const				{ return m_uNeededTime; }
	inline unsigned answer() const					{ return m_uAnswer; }
	static QString answerText(const unsigned uAnswer);
	QString answerText() const;
	QString neededTimeText() const;

	bool isCorrect(const QList<CAnswer>& listAnswer) const;

	bool load (QDomElement elem);
	void save (QDomElement& parent, QDomDocument& doc) const;

	static QString tr (const char *sourceText, const char *comment=0);

	bool operator < (const CAnswerClicked& ac) const;

protected:
	QDateTime m_dt;				//!< Zeitstempel der Antwort
	unsigned m_uAnswer;			//!< Antwort-Bitmaske
	unsigned m_uNeededTime;		//!< Benötigte Zeit in ms
};

#endif
