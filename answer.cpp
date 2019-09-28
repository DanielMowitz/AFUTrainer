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

#include "answer.h"

#include <qcoreapplication.h>


CAnswer::CAnswer(const QString& strText, const bool bCorrect)
{
	m_strText = strText;
	m_bIsCorrect = bCorrect;
}

void CAnswer::clear()
{
	m_strText.clear();
	m_bIsCorrect = false;
}

QString CAnswerClicked::tr (const char *sourceText, const char *comment)
{
	return QCoreApplication::translate("CAnswerClicked", sourceText, comment);
}

void CAnswerClicked::clear()
{
	m_dt = QDateTime::currentDateTime();
	m_uAnswer=0;
	m_uNeededTime=0;
}

bool CAnswerClicked::operator < (const CAnswerClicked& ac) const
{
	return m_dt < ac.m_dt;
}

QString CAnswerClicked::answerText(const unsigned uAnswer)
{
QString strRet;
	for (int i=0; i<32; i++)
	{
		if (uAnswer & (1<<i))
		{
			if (!strRet.isEmpty())
				strRet += ", ";
			strRet += QChar('A' + i);
		}
	}
	return strRet;
}

QString CAnswerClicked::answerText() const
{
	return answerText(m_uAnswer);
}

QString CAnswerClicked::neededTimeText() const
{
	if (m_uNeededTime < 1000)
		return tr("&lt; 1 sec");
	else if (m_uNeededTime < 60000)
		return tr("%1 sec").arg(m_uNeededTime / 1000);
	else
		return tr("%1:%2 min").arg(m_uNeededTime / 60000).arg((m_uNeededTime / 1000) % 60, 2, 10, QChar('0'));
}

bool CAnswerClicked::isCorrect(const QList<CAnswer>& listAnswer) const
{
unsigned uMask=0;
	for (int i=0; i<listAnswer.size(); i++)
	{
		if (listAnswer.at(i).isCorrect())
			uMask |= (1<<i);
	}
	return (m_uAnswer == uMask);
}

bool CAnswerClicked::load (QDomElement elem)
{
bool bOk=false;
	if (elem.tagName() != "answer_clicked") return false;
	m_dt = QDateTime::fromString (elem.attribute("datetime"), Qt::ISODate);
	if (!m_dt.isValid()) return false;
	m_uAnswer = elem.attribute("answer_code").toUInt(&bOk);
	if (!bOk) return false;
	m_uNeededTime = elem.attribute("needed_time").toUInt(&bOk);
	if (!bOk) return false;
	return true;
}

void CAnswerClicked::save (QDomElement& parent, QDomDocument& doc) const
{
QDomElement elem = doc.createElement("answer_clicked");

	elem.setAttribute("datetime", m_dt.toString(Qt::ISODate));
	elem.setAttribute("answer_code", QString("%1").arg(m_uAnswer));
	elem.setAttribute("needed_time", QString("%1").arg(m_uNeededTime));
	parent.appendChild(elem);
}

