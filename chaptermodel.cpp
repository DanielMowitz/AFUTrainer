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

#include "chaptermodel.h"
#include "catalog.h"

#include <qtextdocument.h>

#define COL_CHAPTER 0
#define COL_QUESTION 1
#define COL_ASSIST 2
#define COL_AVG 3

CChapterModel::CChapterModel(QObject *pParent) : QAbstractItemModel (pParent)
{
	m_pCatalog = 0;
}

CChapterModel::~CChapterModel()
{
}

void CChapterModel::onLanguageChanged()
{
	headerDataChanged(Qt::Horizontal, 0, columnCount()-1);
}

void CChapterModel::clear()
{
	m_listChapter.clear();
	m_pCatalog = 0;
	reset();
}

void CChapterModel::setModelData (CCatalog *pCatalog, QList<CChapter*> listChapter)
{
	m_listChapter = listChapter;
	m_pCatalog = pCatalog;
	reset();
}

int CChapterModel::columnCount (const QModelIndex & parent) const
{
	Q_UNUSED(parent)
	return 4;
}

QVariant CChapterModel::data (const QModelIndex & index, int role) const
{
CChapter *p = (CChapter*) index.internalPointer();

	if (m_pCatalog == 0 || !index.isValid()) return QVariant();

	if (role == Qt::TextAlignmentRole && index.column() == COL_QUESTION) return Qt::AlignRight;

	if (p == 0) return QVariant();

	if (role == Qt::DisplayRole)
	{
		if (index.column() == COL_CHAPTER)
		{
			if (p->id().isEmpty())
				return p->text();
			else
				return p->idWithParents() + " - " + p->text();
		}
		else if (index.column() == COL_QUESTION)
			return QString("%1").arg(p->countSubQuestion());
		else if (index.column() == COL_ASSIST)
		{
			return p->recommendationText();
/*			CRecommendation r(p);
			return r.textShort();
*/
		}
	}
	else if (role == Qt::DecorationRole)
	{
		if (index.column() == COL_CHAPTER)
			return QIcon(":/icons/16x16/folder.png");
		else if (index.column() == COL_AVG)
		{
			return p->levelAvgIcon();
		}
		else if (index.column() == COL_ASSIST)
		{
			return p->recommendationIcon(m_pCatalog);
//TODO			return p->recommendation().icon(m_pCatalog);
		}
	}
	else if (role == Qt::ToolTipRole)
	{
		if (index.column() == COL_AVG)
		{
			return QString ("%1 - Kennzahl: %2").arg(p->/*statistic().*/levelAvgText()).arg(p->/*statistic().*/levelAvg(), 4, 'f', 2);
		}
		else if (index.column() == COL_ASSIST)
		{
			return p->recommendationToolTip();
//TODO			return p->recommendation().recommendationText(m_pCatalog);
		}
	}

	return QVariant();
}

bool CChapterModel::hasChildren (const QModelIndex & parent) const
{
	if (parent.isValid()) return false;
	return (!m_listChapter.isEmpty());
}

QVariant CChapterModel::headerData (int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical) return QVariant();

	if (role == Qt::DisplayRole)
	{
		if (section == COL_CHAPTER)
			return tr("Kapitel");
		else if (section == COL_QUESTION)
			return tr("Fragen");
		else if (section == COL_AVG)
			return tr("Ø");
		else if (section == COL_ASSIST)
			return tr("Lernassistent");
	}
	else if (role == Qt::ToolTipRole)
	{
		if (section == COL_CHAPTER)
			return tr("Kapitelname\nBitte markieren Sie das Kapitel, das Sie lernen möchten.");
		else if (section == COL_QUESTION)
			return tr("Anzahl d. Fragen inkl. Unterkapitel");
		else if (section == COL_AVG)
			return tr("Durchschnittlicher Lernfortschritt");
		else if (section == COL_ASSIST)
			return tr("Empfehlung des Lernassistentes");
	}
	else if (role == Qt::WhatsThisRole)
	{

	}
	return QVariant();
}

QModelIndex CChapterModel::index (int row, int column, const QModelIndex & parent) const
{
CChapter *p=0;

	if (parent.isValid() || row >= m_listChapter.size()) return QModelIndex();
	p = m_listChapter.at (row);
	return createIndex (row, column, (void*)p);
}

QModelIndex CChapterModel::parent (const QModelIndex & index) const
{
	Q_UNUSED(index);
	return QModelIndex();
}

int CChapterModel::rowCount (const QModelIndex & parent) const
{
	if (parent.isValid()) return 0;
	return m_listChapter.size();
}

