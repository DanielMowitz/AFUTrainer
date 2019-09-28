/***************************************************************************
 *   Copyright (C) 1999-2007 by Oliver Saal                                *
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

#ifndef ERROR_H
#define ERROR_H

//#define ERROR_USE_SQL

#ifndef __PRETTY_FUNCTION__
#	if defined __FUNCSIG__
#		define __PRETTY_FUNCTION__ __FUNCSIG__
#	elif defined __func__
#		define __PRETTY_FUNCTION__ __func__
#	else
#		define __PRETTY_FUNCTION__ __FILE__
#	endif
#endif



#include <qstring.h>

#ifdef ERROR_USE_SQL
#include <qsqlquery.h>

#define THROW_TRANSACTION(x) throw CError (tr("Could not start database transaction."), x, __PRETTY_FUNCTION__, __FILE__, __LINE__);
#define THROW_COMMIT(x) throw CError (tr("Could not commit database transaction."), x, __PRETTY_FUNCTION__, __FILE__, __LINE__);
#endif

class CError
{
public:
	CError ();
	CError (const QString& strFunc, const QString& strFile, const unsigned int uLine);
	CError (const QString& strText, const QString& strFunc, const QString& strFile, const unsigned int uLine);
#ifdef ERROR_USE_SQL
	CError (const QString& strText, const QSqlDatabase* pDB, const QString& strFunc, const QString& strFile, const unsigned int uLine);
	CError (const QString& strText, const QSqlQuery& query, const QString& strFunc, const QString& strFile, const unsigned int uLine);
#endif

	inline void preText (const QString& str)		{ m_strText = str + m_strText; }
	inline void postText (const QString& str)		{ m_strText += str; }
	
	inline QString text() const						{ return m_strText; }
	inline QString function() const					{ return m_strFunction; }
	inline QString file() const						{ return m_strFile; }
	inline unsigned line() const					{ return m_uLine; }

	QString toPlainText() const;
	QString toHtml() const;
	
protected:
	QString m_strText;
	QString m_strFunction;
	QString m_strFile;
	unsigned int m_uLine;
};


#endif // ERROR_H
