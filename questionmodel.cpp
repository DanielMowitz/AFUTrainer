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

#include "chapter.h"
#include "questionmodel.h"

#include <qtextdocument.h>

CQuestionModel::CQuestionModel(QObject *pParent) : QAbstractItemModel (pParent)
{
	m_pChapter=0;
}

CQuestionModel::~CQuestionModel()
{
}

void CQuestionModel::onLanguageChanged()
{
	headerDataChanged(Qt::Horizontal, 0, columnCount()-1);
}

void CQuestionModel::clear()
{
	m_pChapter=0;
	reset();
}

void CQuestionModel::setModelData (CChapter *pChapter)
{
	m_pChapter = pChapter;
	reset();
}

int CQuestionModel::columnCount (const QModelIndex & parent) const
{
	Q_UNUSED(parent)
	return 7;
}

QVariant CQuestionModel::data (const QModelIndex & index, int role) const
{
CQuestion *p;
QTextDocument t;

	if (!index.isValid()) return QVariant();
	p = (CQuestion*) index.internalPointer();
	if (p == 0) return QVariant();
	if (role == Qt::DisplayRole)
	{
		if (index.column() == 0)
			return p->id();
		else if (index.column() == 1)
			return p->firstTextLine();
			//return p->text();
		else if (index.column() == 2)
			return p->clickedCorrect();
		else if (index.column() == 3)
			return p->clickedWrong();
		else if (index.column() == 4)
			return p->levelText();
		else if (index.column() == 5)
		{
			t.setHtml(p->lastClickedText());
			return t.toPlainText();
		}
		else if (index.column() == 6)
		{
			t.setHtml(p->repeatDateText());
			return t.toPlainText();
		}
	}
	else if (role == Qt::DecorationRole)
	{
		if (index.column() == 4)
			//return QIcon (QString(":/icons/16x16/level%1.png").arg(p->level()));
			return p->levelIcon();
	}
	else if (role == Qt::ToolTipRole)
	{
		if (index.column() == 5)
			return p->lastClickedTextExtended();
		else if (index.column() == 6)
			return p->repeatDateTextExtended();
/*		if (index.column() == 4)
			return "<img src='" + p->levelIconName() + "'> " + p->levelText();
*/	}
/*	else if (role == Qt::WhatsThisRole)
	{
		if (index.column() == 4)
			return QString("Was ist das?");
	}
*/
	return QVariant();
}

bool CQuestionModel::hasChildren (const QModelIndex & parent) const
{
	if (parent.isValid()) return false;
	return (m_pChapter != 0);
}

QVariant CQuestionModel::headerData (int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical) return QVariant();
	if (role == Qt::DisplayRole)
	{
		if (section == 0)
			return tr("Kennung");
		else if (section == 1)
			return tr("Frage");
		else if (section == 2)
			return tr("R");
		else if (section == 3)
			return tr("F");	
		else if (section == 4)
			return tr("Lernfortschritt");
		else if (section == 5)
			return tr("letzte Abfrage");
		else if (section == 6)
			return tr("Wiederholung");
	}
/*	else if (role == Qt::DecorationRole)
	{
		if (section == m_iColBookmarkFlag) 
			return QIcon (":/icons/16x16/bookmark.png");
	}*/
	else if (role == Qt::ToolTipRole)
	{
		if (section == 2) 
			return tr("Anzahl der richtigen Antworten");
		else if (section == 3) 
			return tr("Anzahl der falschen Antworten");
	}
	return QVariant();
}

QModelIndex CQuestionModel::index (int row, int column, const QModelIndex & parent) const
{
CQuestion *p=0;

	if (m_pChapter == 0 || parent.isValid() || row >= m_pChapter->countQuestion()) return QModelIndex();
	p = m_pChapter->questionAt (row);
	return createIndex (row, column, (void*)p);
}

QModelIndex CQuestionModel::parent (const QModelIndex & index) const
{
	return QModelIndex();
}

int CQuestionModel::rowCount (const QModelIndex & parent) const
{
	if (m_pChapter == 0 || parent.isValid()) return 0;
	return m_pChapter->countQuestion();
}

