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

#include "helper.h"
#include "catalog.h"

void CHint::clear()
{
	m_strlQuestionId.clear();
	m_strAuthor.clear();
	m_date = QDate();
	m_strText.clear();
	m_strComment.clear();
}

bool CHint::load (QDomElement elem)
{
QString str;

	if (elem.tagName() != QString ("hint")) return false;
	m_strAuthor = elem.attribute ("author", "AFUTrainer-Hilfe");
	m_date = QDate::fromString(elem.attribute ("date"), Qt::ISODate);
	str = elem.attribute("question");
	str = str.replace(' ', ';').replace(',', ';');
	m_strlQuestionId = str.split(";", QString::SkipEmptyParts);
	m_strComment = elem.attribute ("comment");
	m_strText = elem.text ();

	if (m_strText.isEmpty()) return false;
	return true;
}

void CHint::save (QDomElement& parent, QDomDocument& doc)
{
QDomElement elemRoot = doc.createElement("hint");
	if (!m_strAuthor.isEmpty()) elemRoot.setAttribute("author", m_strAuthor);
	if (m_date.isValid()) elemRoot.setAttribute("date", m_date.toString(Qt::ISODate));
	elemRoot.setAttribute("question", m_strlQuestionId.join(";"));
	if (!m_strComment.isEmpty()) elemRoot.setAttribute("comment", m_strComment);
	parent.appendChild(elemRoot);
	elemRoot.appendChild(doc.createTextNode(m_strText));
}

QString CHint::showText() const
{
QString str;

/*	if (text().isEmpty())
	{
		if (pCatalog)
			return pCatalog->hintText(id());
		else
			return QString();
	}
*/
/*	str = "<p><table cellspacing='0' border='0' width='100%'>";
	str += "<tr><td bgcolor='gray'><b>"+author()+"</b></td>";
	str += "<td bgcolor='gray' align='right'><b>"+dateString()+"</b></td></tr>";
	str += "<tr><td colspan='2'>"+text()+"</td></tr>";
	str += "<tr><td colspan='2'>&nbsp;</td></tr>";
	str += "</table></p>";
*/

	str += "<p>" + text() + "</p>"; //"<p><i><font size=-1>" + author() + "&nbsp;" + date() + "</font></i></p>";
	return (str);
}
