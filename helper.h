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

#ifndef HELPER_H
#define HELPER_H

#include <qstring.h>
#include <qdom.h>
#include <qdatetime.h>
#include <qstringlist.h>

class CCatalog;

class CHint
{
public:	
	CHint()													{ clear(); }
	~CHint()												{}

	void clear();

	inline void appendQuestion(const QString& id)			{ if (!hasQuestion(id)) m_strlQuestionId.append(id); }
	inline void removeQuestion(const QString& id)			{ m_strlQuestionId.removeAll(id); }
	inline bool hasQuestion(const QString& id) const		{ return m_strlQuestionId.contains(id); }

	inline QString author() const							{ return m_strAuthor; }
	inline void setAuthor(const QString& strAuthor)			{ m_strAuthor = strAuthor; }

	inline QDate date() const								{ return m_date; }
	inline QString dateString() const						{ return m_date.toString(Qt::LocalDate); }
	inline void setDate (const QDate& date)					{ m_date = date; }

	inline QString text() const								{ return m_strText; }
	inline void setText(const QString& strText)				{ m_strText = strText; }

	inline QString comment() const							{ return m_strComment; }
	inline void setComment (const QString& strComment)		{ m_strComment = strComment; }


	QString showText() const;

	bool load (QDomElement elem);
	void save (QDomElement& parent, QDomDocument& doc);

protected:
	QStringList m_strlQuestionId;
	QString m_strAuthor;
	QDate m_date;
	QString m_strText;
	QString m_strComment;
};

#endif
