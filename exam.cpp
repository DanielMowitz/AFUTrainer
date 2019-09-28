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

#include "exam.h"
#include "question.h"
#include "error.h"
#include "tools.h"

#include <qregexp.h>
#include <qvariant.h>

//#define DEBUGMSG

void CExamPart::clear()
{
	m_iQuestionCount = 0;
	m_strRegExp.clear();
}

bool CExamPart::load (QDomElement elem)
{
	if (elem.tagName() != QString ("exam_part")) return false;
	m_iQuestionCount = elem.attribute("count").toUInt();
	if (m_iQuestionCount == 0) return false;
	m_strGroup = elem.attribute("group");
	m_strRegExp = elem.text ();
	if (m_strRegExp.isEmpty()) return false;
	return true;
}

void CExamPart::save (QDomElement& parent, QDomDocument& doc) const
{
QDomElement elemRoot = doc.createElement("exam_part");
	
	elemRoot.setAttribute("count", QString("%1").arg(m_iQuestionCount));
	elemRoot.setAttribute("group", m_strGroup);
	elemRoot.appendChild(doc.createTextNode(m_strRegExp));

	parent.appendChild(elemRoot);
}

QList<CQuestion*> CExamPart::createQuestionPool(const QList<CQuestion*>& listAllQuestions) const
{
QList<CQuestion*> listRet, listSelected;
QRegExp regexp(m_strRegExp);
int i=0;
CQuestion *q=0;

	#ifdef DEBUGMSG
	qDebug ("\tPart Group = '%s' RegExp '%s' mit %i Fragen", qPrintable (m_strGroup), qPrintable(m_strRegExp), m_iQuestionCount);
	#endif

	// Alle Fragen raussuchen, die dem RegExp entsprechen
	for (i=0; i<listAllQuestions.size(); i++)
	{
		q = listAllQuestions.at(i);
		if (!m_strGroup.isEmpty() && !q->groups().contains(m_strGroup, Qt::CaseInsensitive)) 
			continue;

		if (regexp.exactMatch(q->id()))
		{
			#ifdef DEBUGMSG
			qDebug ("\t\tAdding Question '%s'", qPrintable (q->id()));
			#endif
			listSelected.append(q);
		}
	}

	if (listSelected.size() < questionCount())
	{
		throw CError (
			QString("Für den regulären Ausdruck '%1' wurden nur %2 Fragen gefunden - zu wenig, um daraus %3 Fragen für die Prüfung auszuwählen.")
			.arg(m_strRegExp).arg(listSelected.size()).arg(questionCount()), __PRETTY_FUNCTION__, __FILE__, __LINE__);
	}

	#ifdef DEBUGMSG
	qDebug ("\tStarte Auswahlverfahren...");
	#endif
	for (i=0; i < questionCount(); i++)
	{
		int rnd = afu_random (0, listSelected.size()-1);
		q = listSelected.at(rnd);
		#ifdef DEBUGMSG
		qDebug ("\t\tSelected question '%s'", qPrintable (q->id()));
		#endif
		listRet.append (q);
		listSelected.removeAt(rnd);
	}

	return listRet;
}



void CExam::clear()
{
	m_strId.clear();
	m_strName.clear();
	m_strComment.clear();
	m_uDuration = 0;
	m_uMaxErrorPoints = 0;
	m_listParts.clear();
}

bool CExam::load (QDomElement elem)
{
	if (elem.tagName() != QString ("exam")) return false;

	m_strId = elem.attribute("id");
	m_strName = elem.attribute("name");
	m_uDuration = elem.attribute("duration").toUInt();
	m_uMaxErrorPoints = elem.attribute("maxerrorpoints").toUInt();
	m_strComment = elem.attribute("comment");

	if (m_strName.isEmpty()) return false;
	if (m_uDuration == 0) return false;

	QDomNode n = elem.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			QDomElement e = n.toElement ();
			if (e.tagName() == QString ("exam_part"))
			{
				CExamPart ep;
				if (ep.load(e))	m_listParts.append(ep);
			}
		}
		n = n.nextSibling();
	}

	return true;
}

void CExam::save (QDomElement& parent, QDomDocument& doc) const
{
QDomElement elemRoot = doc.createElement("exam");
	
	elemRoot.setAttribute("id", m_strId);
	elemRoot.setAttribute("name", m_strName);
	elemRoot.setAttribute("duration", QString("%1").arg(m_uDuration));
	elemRoot.setAttribute("maxerrorpoints", QString("%1").arg(m_uMaxErrorPoints));
	elemRoot.setAttribute("comment", m_strComment);
	for (int i=0; i<m_listParts.size(); i++)
		m_listParts.at(i).save(elemRoot, doc);

	parent.appendChild(elemRoot);
}

unsigned CExam::questionCount() const
{
unsigned u=0;
	for (int i=0; i<m_listParts.size(); i++)
	{
		u += m_listParts.at(i).questionCount();
	}
	return u;
}

QList<CQuestion*> CExam::createQuestionPool(const QList<CQuestion*>& listAllQuestions) const
{
QList<CQuestion*> listRet;

	#ifdef DEBUGMSG
	qDebug ("Erzeuge Prüfungsfragen für '%s'", qPrintable(name()));
	#endif

	try
	{
		for (int i=0; i<m_listParts.size(); i++)
		{
			listRet << m_listParts.at(i).createQuestionPool(listAllQuestions);
		}
	}
	catch (CError e)
	{
		e.preText(QString("Konnte Prüfung '%1' nicht erzeugen.").arg(name()));
		throw;
	}

	#ifdef DEBUGMSG
	qDebug ("Insg. %i Prüfungsfragen ausgewählt", listRet.size());
	for (int i=0; i<listRet.size(); i++)
		qDebug ("\t%i. Frage ID '%s'", i+1, qPrintable(listRet.at(i)->id()));
	#endif

	return listRet;
}

CExamStat::CExamStat(const CExam& exam)
{
	clear();
	m_strId = exam.id();
}

void CExamStat::clear()
{
	m_strId.clear();
	m_dt = QDateTime::currentDateTime();
	m_uSecs = 0;

	m_strlQuestionId.clear();
	m_listAnswer.clear();

	m_uCorrect = 0;
	m_uWrong = 0;
	m_uErrorPoints = 0;
	m_bPassed = false;
}

void CExamStat::setQuestions(const QList<CQuestion*>& listQuestion, const QList<unsigned>& listAnswerMask)
{
	for (int i=0; i<listQuestion.size(); i++)
	{
		CQuestion *q = listQuestion.at(i);
		m_strlQuestionId.append(q->id());
		m_listAnswer.append(q->orderedAnswerMask(listAnswerMask.at(i)));
	}
}

void CExamStat::setResult (const unsigned uCorrect, const unsigned uWrong, const unsigned uErrorPoints, const bool bPassed)
{
	m_uCorrect = uCorrect;
	m_uWrong = uWrong;
	m_uErrorPoints = uErrorPoints;
	m_bPassed = bPassed;
}

bool CExamStat::load (QDomElement elem)
{
	if (elem.tagName() != QString ("exam")) return false;

	m_strId = elem.attribute("id");
	m_uSecs = elem.attribute("secs").toUInt();
	m_dt = QDateTime::fromString(elem.attribute("datetime"), Qt::ISODate);
	m_uCorrect = elem.attribute("correct").toUInt();
	m_uWrong = elem.attribute("wrong").toUInt();
	m_uErrorPoints = elem.attribute("errorpoints").toUInt();
	m_bPassed = QVariant(elem.attribute("passed")).toBool();

	if (m_strId.isEmpty()) return false;
	if (m_uSecs == 0) return false;

	QDomNode n = elem.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			QDomElement e = n.toElement ();
			if (e.tagName() == QString ("question"))
			{
				QString strId = e.attribute("id");
				if (!strId.isEmpty())
				{
					m_strlQuestionId.append (strId);
					m_listAnswer.append(e.attribute("answer").toUInt());
				}
			}
		}
		n = n.nextSibling();
	}

	return true;
}

void CExamStat::save (QDomElement& parent, QDomDocument& doc) const
{
QDomElement elemRoot = doc.createElement("exam");
	
	elemRoot.setAttribute("id", m_strId);
	elemRoot.setAttribute("secs", QString("%1").arg(m_uSecs));
	elemRoot.setAttribute("datetime", m_dt.toString(Qt::ISODate));
	elemRoot.setAttribute("correct", QString("%1").arg(m_uCorrect));
	elemRoot.setAttribute("wrong", QString("%1").arg(m_uWrong));
	elemRoot.setAttribute("errorpoints", QString("%1").arg(m_uErrorPoints));
	elemRoot.setAttribute("passed", QString("%1").arg(m_bPassed));
	for (int i=0; i<m_strlQuestionId.size(); i++)
	{
		QDomElement elemQuestion = doc.createElement("question");
		elemQuestion.setAttribute("id", m_strlQuestionId.at(i));
		elemQuestion.setAttribute("answer", m_listAnswer.at(i));
		elemRoot.appendChild(elemQuestion);
	}

	parent.appendChild(elemRoot);
}

