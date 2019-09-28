/***************************************************************************
 *   Copyright (C) 1999-2005 by Oliver Saal                                *
 *   http://www.oliver-saal.de/                                            *
 *   osaal@gmx.de                                                          *
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

#include "error.h"

#include <qobject.h>
#include <qstring.h>

#ifdef ERROR_USE_SQL
#include <qsqlerror.h>
#include <qmap.h>
//#include <qmapiterator.h>
#include <qvariant.h>
#endif

CError::CError ()
{
	m_uLine = 0;
}

CError::CError (const QString& strFunc, const QString& strFile, const unsigned int uLine)
{
	m_strFunction = strFunc;
	m_strFile = strFile;
	m_uLine = uLine;
}

CError::CError (const QString& strText, const QString& strFunc, const QString& strFile, const unsigned int uLine)
{
	m_strText = strText;
	m_strFunction = strFunc;
	m_strFile = strFile;
	m_uLine = uLine;
}

#ifdef ERROR_USE_SQL

CError::CError (const QString& strText, const QSqlDatabase* pDB, const QString& strFunc, const QString& strFile, const unsigned int uLine)
{
	m_strText = QString ("%1\n%3").arg (strText, pDB->lastError().text());
	m_strFunction = strFunc;
	m_strFile = strFile;
	m_uLine = uLine;	
}

CError::CError (const QString& strText, const QSqlQuery& query, const QString& strFunc, const QString& strFile, const unsigned int uLine)
{
QMap<QString, QVariant> map;
	m_strText = QString ("%1\nSQL: %2\n").arg (strText, query.lastQuery());
	map = query.boundValues();
	if (!map.isEmpty())
	{
		m_strText+="Bound values:\n";
		QMap<QString, QVariant>::const_iterator i = map.constBegin();
		while (i != map.constEnd()) 
		{
			m_strText += QString ("%1 / %2 ==> %4\n").arg (i.key(), i.value().typeName(), i.value().isNull() ? "NULL" : i.value().toString());
        	++i;
		}
	}
	
	m_strText += query.lastError().text();
	m_strFunction = strFunc;
	m_strFile = strFile;
	m_uLine = uLine;	
}
#endif

QString CError::toPlainText() const
{
	return QObject::tr("%1\n\nFunction: %2\nFile: %3 Line: %4\n").arg(m_strText, m_strFunction, m_strFile).arg(m_uLine);
}

QString CError::toHtml() const
{
QString str = m_strText;
	str.replace ('\n', "<br>");
	return QObject::tr("<p><font color='red'><b>%1</b></font><br>Function: %2<br>File: %3 Line: %4</p><br>\n").arg(str, m_strFunction, m_strFile).arg(m_uLine);
}

