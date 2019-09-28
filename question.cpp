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

#include "question.h"
#include "catalog.h"
#include "tools.h"

#include <qtextdocument.h>
#include <qcoreapplication.h>
#include <qfileinfo.h>
#include <qvariant.h>


void CDayStatistic::clear()
{
	m_date = QDate();
	m_uClickedWrong = 0;
	m_uClickedCorrect = 0;
	m_uTimeExpeditureCorrect = 0;
	m_uTimeExpeditureWrong = 0;
	m_dLevel = 0.0;
}

CDayStatistic& CDayStatistic::operator += (const CDayStatistic& ds)
{
	m_date = ds.m_date;
	m_uClickedWrong += ds.m_uClickedWrong;
	m_uClickedCorrect += ds.m_uClickedCorrect;
	m_uTimeExpeditureCorrect += ds.m_uTimeExpeditureCorrect;
	m_uTimeExpeditureWrong += ds.m_uTimeExpeditureWrong;
	m_dLevel += ds.m_dLevel;
	return *this;
}

void CDayStatistic::debug() const
{
	qDebug("-- Day statistic %s --", qPrintable(m_date.toString(Qt::LocalDate)));
	if (m_uClickedCorrect + m_uClickedWrong == 0)
	{
		qDebug ("   No clicks");
		return;
	}
	qDebug("  %i correct + %i wrong = %i", m_uClickedCorrect, m_uClickedWrong, m_uClickedCorrect + m_uClickedWrong);
	qDebug("  %i ms + %i ms = %i ms", m_uTimeExpeditureCorrect, m_uTimeExpeditureWrong, m_uTimeExpeditureCorrect + m_uTimeExpeditureWrong);
	qDebug("  Level = %lf", m_dLevel);
}

QString CQuestion::tr (const char *sourceText, const char *comment)
{
	return QCoreApplication::translate("CQuestion", sourceText, comment);
}

unsigned CQuestion::waitDaysForRepeat (const unsigned uLevel)
{
	switch (uLevel)
	{
	case LEVEL_VERYOFTEN:	
		return 0;
	case LEVEL_OFTEN:
		return 1;
	case LEVEL_NORMAL:
		return 2;
	case LEVEL_RARE:
		return 4;
	case LEVEL_VERYRARE:
		return 8;
	default:
	case LEVEL_EXTREMERARE:
		return 16;
	}
}


void CQuestion::clear()
{
	m_pParentChapter = 0;
	m_strId.clear();
	m_strText.clear();
	m_listAnswer.clear();
	m_strlGroups.clear();
	m_uErrorPoints = 1;

	m_uCorrectAnswers=0;
	m_uCorrectSuccessive=0;
	m_uWrongAnswers=0;
	m_uWrongSuccessive=0;
	m_uLevel=0;
}

QString CQuestion::plainText() const
{
QTextDocument doc;
	doc.setHtml(m_strText);
	return doc.toPlainText();
}

QString CQuestion::firstTextLine() const
{
QString str = plainText();
int i = str.indexOf('\n');
	return str.left (i);
}

QString CQuestion::showText(CCatalog *pCatalog) const
{
QString str, strHints;
	str +="<h2>Frage " + id() + "</h2>";
	str += "<p>" + text() + "</p><br>";

	str += "<p><table cellspacing='3'>";
	for (int i=0; i<m_listAnswer.size(); i++)
	{
		str += "<tr>";
		// Icon
		str += "<td valign='top' width='30'><img src=':/icons/16x16/button_";
		if (m_listAnswer[i].isCorrect())
			str +="ok";
		else
			str +="cancel";
		str +=".png'></td>";
		// Answer number
		str += QString("<td valign='top' >&nbsp;<b>%1</b>&nbsp;</td>").arg(QChar('A' + i));
		// Text
		str += "<td valign='top'>" + m_listAnswer[i].text() + "</td>";
		str += "</tr>"; 
	}
	str += "</table></p>";
	strHints = pCatalog->hintText(id());
	if (!strHints.isEmpty())
	{
		str += "<h2>Hilfestellung</h2>" + strHints;
	}
	str += "<h2>Abfrageverlauf</h2>";
	str += lastClickedTextExtended();
	str += " " + repeatDateTextExtended();
	if (!m_listAnswerClicked.isEmpty())
	{
		str += "<br><br><table><tr><th>Datum/Uhrzeit&nbsp;</th><th>Antwort&nbsp;</th><th>Richtig?&nbsp;</th><th>Benötigte Zeit</th></tr>";
		for (int i=0; i<m_listAnswerClicked.size(); i++)
		{
			CAnswerClicked ac = m_listAnswerClicked.at(i);
			str += "<tr><td>" + ac.dateTime().toString(Qt::LocalDate) + "&nbsp;</td><td>" + ac.answerText() + "</td><td>";
			if (ac.isCorrect(m_listAnswer))
				str += tr("richtig");
			else
				str += tr("falsch");
			str += "</td><td>" + QString ("%1").arg(ac.neededTimeText()) + "</td></tr>";
		}
		str += "</table>";
	}
	//str +="<h2>Statistik</h2>";
	return str;
}

QString CQuestion::learnText(CCatalog *pCatalog, const bool bShowId, const bool bShowHints)
{
QString str;

	if (bShowId) str += "<b>" + id() + "</b> ";
	str += text() + "<br>";

	str += "<p><table cellspacing='3'>";
	for (int i=0; i<m_listAnswer.size(); i++)
	{
		str += "<tr>";
		// Answer number
		str += QString("<td valign='top' >&nbsp;<b>%1</b>&nbsp;</td>").arg(QChar('A' + i));
		// Text
		str += "<td valign='top'>";
		//CHEAT: if (mixedAnswerAt(i).isCorrect()) str += "<i>OK</i> ";
		str += learnAnswerAt(i).text();
		str += "</td></tr>"; 
	}
	str += "</table></p>";

	if (bShowHints)
	{
		QString strHints = pCatalog->hintText(id());
		if (!strHints.isEmpty()) str += "<hr>" + strHints;
	}
	return str;
}

bool CQuestion::load (QDomElement elem)
{
QString str;
	if (elem.tagName () != "question") return false;
	if (!elem.hasAttribute ("id")) return false;

	clear ();
	m_strId = elem.attribute ("id");
	m_uErrorPoints = elem.attribute("errorpoints", "1").toUInt();
	str = elem.attribute("groups");
	str = str.replace(' ', ';').replace(',', ';');
	m_strlGroups = str.split(";", QString::SkipEmptyParts);

	if (m_uErrorPoints == 0) return false;

	QDomNode n = elem.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			QDomElement e = n.toElement ();
			str = e.text();
			if (e.tagName () == "textquestion")
				m_strText = str;
			else if (e.tagName () == "textanswer")
				m_listAnswer.append(CAnswer(str, QVariant(e.attribute("correct", "false")).toBool()));
		}
		n = n.nextSibling();
	}
	return true;
}


QString CQuestion::removeTempPath(const QString& strText)
{
QString str=strText, strFileName, strBaseName;
int idxStart=-1, idxEnd=0;
	while ((idxStart = str.indexOf(QRegExp("src\\s*=\\s*\""), idxEnd)) != -1)
	{
		idxStart = str.indexOf('"', idxStart)+1;
		idxEnd = str.indexOf('"', idxStart);
		strFileName = str.mid(idxStart, idxEnd - idxStart);
		if (strFileName.isEmpty()) continue;
		QFileInfo fi(strFileName);
		strBaseName = fi.fileName();
		strBaseName = strBaseName.left(strBaseName.lastIndexOf('.'));
		str.replace(strFileName, strBaseName);
	}
	return str;
}

void CQuestion::save (QDomElement& parent, QDomDocument& doc)
{
QDomElement elemRoot = doc.createElement("question");
	elemRoot.setAttribute("id", id());
	if (m_uErrorPoints > 1)
		elemRoot.setAttribute("errorpoints", QString("%1").arg(m_uErrorPoints));
	if (!m_strlGroups.isEmpty())
		elemRoot.setAttribute("groups", m_strlGroups.join("; "));

	parent.appendChild(elemRoot);

	QDomElement elemQuestion = doc.createElement("textquestion");
	QDomText textQuestion = doc.createTextNode(removeTempPath(text()));
	elemQuestion.appendChild(textQuestion);
	elemRoot.appendChild(elemQuestion);

	// save answers
	for (int i=0; i<m_listAnswer.size(); i++)
	{
		QDomElement elemAnswer = doc.createElement(QString("textanswer"));
		if (m_listAnswer[i].isCorrect())
			elemAnswer.setAttribute("correct", m_listAnswer[i].isCorrect() ? "true" : "false");
		QDomText textAnswer = doc.createTextNode(removeTempPath(m_listAnswer[i].text()));
		elemAnswer.appendChild(textAnswer);
		elemRoot.appendChild(elemAnswer);
	}
}

bool CQuestion::loadLearnStatistic (QDomElement elem)
{
	if (elem.tagName () != "question") return false;
	if (elem.attribute ("id") != id()) return false;

	m_uCorrectAnswers		= elem.attribute("c").toUInt();
	m_uWrongAnswers			= elem.attribute ("w").toUInt();
	m_uCorrectSuccessive	= elem.attribute ("cs").toUInt();
	m_uWrongSuccessive		= elem.attribute ("ws").toUInt();

	m_listAnswerClicked.clear();
	QDomNode n = elem.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			QDomElement e = n.toElement ();
			if (e.tagName () == "answer_clicked")
			{
				CAnswerClicked ac;
				if (ac.load(e))
					m_listAnswerClicked.append(ac);
			}
		}
		n = n.nextSibling();
	}
	qSort(m_listAnswerClicked);
	m_uLevel = levelAtEndOfDay(QDate::currentDate());
	return true;
}

bool CQuestion::saveLearnStatistic (QDomElement& parent, QDomDocument& doc)
{
QDomElement elemQuestion;

	if (clickedWrong() == 0 && clickedCorrect() == 0) return true;
	elemQuestion = doc.createElement ("question");
	elemQuestion.setAttribute ("id", id());
	elemQuestion.setAttribute ("c", QString ("%1").arg(clickedCorrect()));
	elemQuestion.setAttribute ("w", QString ("%1").arg(clickedWrong()));
	elemQuestion.setAttribute ("cs", QString ("%1").arg(clickedCorrectSuccessive()));
	elemQuestion.setAttribute ("ws", QString ("%1").arg(clickedWrongSuccessive()));
	parent.appendChild (elemQuestion);

	for (int i=0; i<m_listAnswerClicked.size(); i++)
	{
		m_listAnswerClicked.at(i).save(elemQuestion, doc);
	}
	return true;
}


QString CQuestion::checkForErrors() const
{
QString str;
int iAnswerCount = 0;

	iAnswerCount = countAnswer();
	if (iAnswerCount != 4)
		str += QObject::tr("Die Frage %1 hat nicht exakt vier Antworten, sondern %2 Stück.").arg(id()).arg(iAnswerCount) + "<br>";
	iAnswerCount = countCorrectAnswer();
	if (iAnswerCount != 1)
		str += QObject::tr("Die Frage %1 hat nicht exakt eine richtige Antwort, sondern %2 Stück.").arg(id()).arg(iAnswerCount) + "<br>";
	for (int j=0; j<countAnswer(); j++)
	{
		if (answerAt(j).text().isEmpty())
			str += QObject::tr("Die Antwort %1 der Frage %2 enthält keinen Text.").arg(QChar('A' + j)).arg(id()) + "<br>";
	}
	if (text().isEmpty())
		str += QObject::tr("Die Frage %1 enthält keinen Text.").arg(id()) + "<br>";

	return str;
}

void CQuestion::mixAnswers(const bool bMix)
{
unsigned u1=0, u2=0;
	m_listMixedAnswer.clear();
	if (!bMix) return;
	for (int i=0; i<m_listAnswer.size(); i++)
		m_listMixedAnswer.append(i);

	for (int i=0; i<m_listMixedAnswer.size()*2; i++)
	{
		u1 = afu_random(0, m_listMixedAnswer.size()-1);
		u2 = afu_random(0, m_listMixedAnswer.size()-1);
		if (u1 == u2) continue;
		m_listMixedAnswer.swap(u1, u2);
	}
}

const CAnswer& CQuestion::learnAnswerAt(int i) const
{
	if (m_listMixedAnswer.size() != m_listAnswer.size()) 
		return m_listAnswer.at(i);
	else
		return m_listAnswer.at(m_listMixedAnswer.at(i));
}

unsigned CQuestion::correctAnswerMask() const
{
unsigned u=0;
	for (int i=0; i<m_listAnswer.size(); i++)
	{
		if (learnAnswerAt(i).isCorrect())
			u |= (1<<i);
	}
	return u;
}

QString CQuestion::answerMaskToString(const unsigned u)
{
QString str;
	for (int i=0; i<(int)sizeof(unsigned); i++)
	{
		if (u & (1<<i))
		{
			if (!str.isEmpty()) str += ", ";
			str += QChar('A' + i);
		}
	}
	return str;
}

QString CQuestion::correctionText(const unsigned uAnswerMask)
{
	if (uAnswerMask == 0)
	{
		return QObject::tr("Die richtige Antwort lautet %1.")
			.arg(answerMaskToString(correctAnswerMask()));
	}
	else if (uAnswerMask == correctAnswerMask())
	{
		return QObject::tr("Ihre Antwort %1 war richtig!").arg(answerMaskToString(uAnswerMask));
	}
	else
	{
		return QObject::tr("Ihre Antwort %1 war leider falsch. Die richtige Antwort wäre %2 gewesen.")
			.arg(answerMaskToString(uAnswerMask)).arg(answerMaskToString(correctAnswerMask()));
	}
}

int CQuestion::countCorrectAnswer() const
{
int iRet=0;
	for (int i=0; i<m_listAnswer.size(); i++)
	{
		if (m_listAnswer.at(i).isCorrect()) 
			iRet++;
	}
	return iRet;
}

unsigned CQuestion::orderedAnswerMask(const unsigned uMixedAnswerMask)
{
unsigned u=0;
	if (m_listMixedAnswer.isEmpty()) return uMixedAnswerMask;

	for (int i=0; i<m_listMixedAnswer.size(); i++)
	{
		u |= ((uMixedAnswerMask>>i) & 0x0001) << m_listMixedAnswer.at(i);
	}
	return u;
}

void CQuestion::registerAnswerClicked (const unsigned uAnswerMask, const unsigned uNeededTime)
{
	m_listAnswerClicked.append(CAnswerClicked(orderedAnswerMask(uAnswerMask), uNeededTime));
	if (correctAnswerMask() == uAnswerMask)
	{
		m_uCorrectAnswers++;
		m_uCorrectSuccessive++;
		m_uWrongSuccessive = 0;
		//if (m_uLevel > 0 && m_uLevel < LEVEL_MAX || m_uLevel == 0 && m_uCorrectSuccessive == 2)
		if (m_uLevel < LEVEL_MAX)
			m_uLevel++;
	}
	else
	{
		m_uWrongAnswers++;
		m_uCorrectSuccessive = 0;
		m_uWrongSuccessive++;
		if (m_uLevel > 0)
			m_uLevel--;
	}
/*	if (m_pParentChapter)
		m_pParentChapter->updateStatistic(true);
*/
}

QString CQuestion::levelIconName(const unsigned uLevel)
{
//	if (uLevel <= LEVEL_VERYRARE)
		return QString(":/icons/16x16/level%1.png").arg(uLevel);
//	else
//		return ":/icons/16x16/button_ok.png";
}

QIcon CQuestion::levelIcon(const unsigned uLevel) 
{
	return QIcon (levelIconName(uLevel));
}

QString CQuestion::levelText(const unsigned uLevel) 
{
	switch (uLevel)
	{
	case LEVEL_VERYOFTEN:
		return QObject::tr("Ahnungslos");
	case LEVEL_OFTEN:
		return QObject::tr("Anfänger");
	case LEVEL_NORMAL:
		return QObject::tr("Fortgeschritten");
	case LEVEL_RARE:
		return QObject::tr("Experte");
	case LEVEL_VERYRARE:
		return QObject::tr("Freak");
	case LEVEL_EXTREMERARE:
		return QObject::tr("Professor");
	default:
		return QObject::tr("unbekannt");
	}
}

QDateTime CQuestion::firstClicked() const
{
	// Vorraussetzung: m_listAnswerClicked ist nach Datum aufsteigend sortiert
	if (m_listAnswerClicked.size() > 0)
		return m_listAnswerClicked.at(0).dateTime();
	return QDateTime();
}

QDateTime CQuestion::lastClicked() const
{
	// Vorraussetzung: m_listAnswerClicked ist nach Datum aufsteigend sortiert
	if (m_listAnswerClicked.size() > 0)
		return m_listAnswerClicked.at(m_listAnswerClicked.size()-1).dateTime();
	return QDateTime();
}

QString CQuestion::lastClickedText() const
{
QDateTime dtLastClicked = lastClicked();
unsigned uSecs = dtLastClicked.secsTo (QDateTime::currentDateTime());
unsigned uDays = dtLastClicked.daysTo (QDateTime::currentDateTime());

	if (!dtLastClicked.isValid())
		return QString();

	if (uSecs < 60)
		return tr("vor &lt; 1 min");
	else if (uSecs < 3600)
		return (tr("vor %1 min").arg(uSecs / 60));
	else if (uDays == 1)
		return (tr("gestern"));
	else if (uDays > 1)
		return (tr("vor %1 Tagen").arg(uDays));
	else
		return (tr("vor %1 h").arg(uSecs / 3600));
}

QString CQuestion::lastClickedTextExtended() const
{
QDateTime dtLastClicked = lastClicked();
QString strRet;
unsigned uLevelOld=0, uLevelNew=0;
	if (!dtLastClicked.isValid())
		return tr("Die Frage wurde noch nie beantwortet.");

	strRet = tr("Die Frage wurde %1 (%2) zuletzt beantwortet. ", "%1=gestern, vor x Tagen, ... / %2 = exaktes Datum m. Uhrzeit").arg(lastClickedText(), dtLastClicked.toString(Qt::LocalDate));

	uLevelOld = levelAtEndOfDay(dtLastClicked.date().addDays(-1));
	uLevelNew = levelAtEndOfDay(dtLastClicked.date());

	if (uLevelOld == uLevelNew)
		strRet += tr("Die Einstufung des Lernfortschritts änderte sich an diesem Tag nicht.");
	else
		strRet += tr("Der Lernfortschritt änderte sich an diesem Tag von '%1' auf '%2'.").arg(levelText(uLevelOld), levelText(uLevelNew));

	return strRet;
}

/*!
\return Datum, an dem die Frage wiederholt werden soll. 
Gibt es kein solches Datum, so wird ein ungültiges Datum (QDate::isValid()) zurückgegeben. 
Hinweis: Das Datum kann auch in der Vergangenheit liegen!
*/

QDate CQuestion::repeatDate() const
{
QDate dLastClicked = lastClicked().date();
unsigned uLevelOld=0, uLevelNew=0;

	if (!dLastClicked.isValid()) return QDate();

	uLevelOld = levelAtEndOfDay(dLastClicked.addDays(-1));
	uLevelNew = levelAtEndOfDay(dLastClicked);

	if (uLevelNew <= uLevelOld && uLevelNew != LEVEL_MAX) 
		return dLastClicked;

	return dLastClicked.addDays(waitDaysForRepeat(m_uLevel));
}

bool CQuestion::isRepeatToday() const
{
QDate d = repeatDate();

	if (d.isValid() && d <= QDate::currentDate()) return true;
	return false;
}

QString CQuestion::repeatDateText() const
{
QDate d = repeatDate();
	if (!d.isValid()) return QString();
	if (d <= QDate::currentDate())
		return tr("heute");
	else if (QDate::currentDate().addDays(1) == d)
		return tr("morgen");
	else
		return tr("in %1 Tagen").arg(QDate::currentDate().daysTo(d));
}

QString CQuestion::repeatDateTextExtended() const
{
QString strRet;
unsigned uLevelOld=0, uLevelNew=0, uLevelTarget=0;
QDate dLastClicked = lastClicked().date();

	if (!dLastClicked.isValid()) return QString();
	uLevelOld = levelAtEndOfDay(dLastClicked.addDays(-1));
	uLevelNew = levelAtEndOfDay(dLastClicked);
	if (uLevelNew > uLevelOld)
		uLevelTarget = uLevelNew + 1;
	else
		uLevelTarget = uLevelOld + 1;
	if (uLevelTarget > LEVEL_MAX) uLevelTarget = LEVEL_MAX;
	if (uLevelNew == LEVEL_MAX)
		strRet = tr("Es wird empfohlen, die Frage %1 zu wiederholen.").arg(repeatDateText());
	else
		strRet = tr("Es wird empfohlen, die Frage %1 zu wiederholen, um den Lernfortschritt '%2' zu erreichen.")
			.arg(repeatDateText()).arg(levelText(uLevelTarget));
	return strRet;
}

unsigned CQuestion::levelAtEndOfDay(const QDate& date) const
{
unsigned uLevel = 0, uCorrectSuccessive=0;
CAnswerClicked ac;
	// Vorraussetzung: m_listAnswerClicked ist nach Datum aufsteigend sortiert
	for (int i=0; i<m_listAnswerClicked.size(); i++)
	{
		ac = m_listAnswerClicked.at(i);
		if (ac.dateTime().date() > date) return uLevel;
		if (ac.isCorrect(m_listAnswer))
		{
			uCorrectSuccessive++;
			if (uLevel < LEVEL_MAX)
			{
				//if (uLevel == LEVEL_VERYOFTEN && uCorrectSuccessive == 2 || uLevel > LEVEL_VERYOFTEN) 
					uLevel++;
			}
		}
		else
		{
			uCorrectSuccessive = 0;
			if (uLevel > LEVEL_VERYOFTEN) uLevel--;
		}
	}
	return uLevel;
}

CDayStatistic CQuestion::dayStatistic(const QDate& date) const
{
CAnswerClicked ac;
CDayStatistic ds;
	ds.m_date = date;
	if (date.isNull())
		ds.m_dLevel = levelAtEndOfDay(QDate::currentDate());
	else
		ds.m_dLevel = levelAtEndOfDay(date);

	// Vorraussetzung: m_listAnswerClicked ist nach Datum aufsteigend sortiert
	for (int i=0; i<m_listAnswerClicked.size(); i++)
	{
		ac = m_listAnswerClicked.at(i);
		if (!date.isNull())
		{
			if (ac.dateTime().date() > date) break;
			if (ac.dateTime().date() < date) continue;
			//if (ac.dateTime().date() != date) continue;
		}
		if (ac.isCorrect(m_listAnswer)) 
		{
			ds.m_uClickedCorrect++;
			ds.m_uTimeExpeditureCorrect += ac.neededTime();
		}
		else
		{
			ds.m_uClickedWrong++;
			ds.m_uTimeExpeditureWrong += ac.neededTime();
		}
	}

	return ds;
}


/*!
\return true: Wenn die Frage heute zum ersten Mal beantwortet wurde und noch in der Kategorie LEVEL_VERYOFTEN ist
*/
bool CQuestion::isLearningNew() const
{
	if (m_uLevel != LEVEL_VERYOFTEN) return false;
	if (m_listAnswerClicked.size() > 0 
		&& m_listAnswerClicked.at(0).dateTime().date() != QDate::currentDate()) return false;
	return true;
}

/*!
\return true: Die Frage wurde gestern oder früher zum ersten Mal beantwortet
*/
bool CQuestion::isKnownQuestion() const
{
	if (m_listAnswerClicked.size() > 0 
		&& m_listAnswerClicked.at(0).dateTime().date() < QDate::currentDate()) return true;
	return false;
}
