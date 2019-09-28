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

#include "catalogmodel.h"

#include "catalog.h"

#define COL_CHAPTER 0
#define COL_QUESTION 1
#define COL_ASSIST 2
#define COL_AVG 3

CCatalogModel::CCatalogModel (QObject *pParent) : QAbstractItemModel (pParent)
{
	m_pCatalog = 0;
}

CCatalogModel::~CCatalogModel()
{
}


void CCatalogModel::onLanguageChanged()
{
	headerDataChanged(Qt::Horizontal, 0, columnCount()-1);
}

void CCatalogModel::setModelData (CCatalog *pCatalog)
{
	reset();
	m_pCatalog = pCatalog;
	reset();
}

int CCatalogModel::columnCount (const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 4;
}

QVariant CCatalogModel::headerData (int section, Qt::Orientation orientation, int role) const
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

QVariant CCatalogModel::data (const QModelIndex& index, int role) const
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
				return p->id() + " - " + p->text();
		}
		else if (index.column() == COL_QUESTION)
			return QString("%1").arg(p->countSubQuestion());
		else if (index.column() == COL_ASSIST)
		{
			return p->recommendationText();
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
		}
	}

	return QVariant();
}

bool CCatalogModel::hasChildren (const QModelIndex& parent) const
{
CChapter *p = (CChapter*) parent.internalPointer();

	if (m_pCatalog == 0) return false;

	if (parent.isValid() && p)
		return (p->countChapter() > 0);
	else // root item
		return true;
}

QModelIndex CCatalogModel::index (int row, int column, const QModelIndex& parent) const
{
CChapter *pParent = (CChapter*) parent.internalPointer();

	if (!parent.isValid())
	{	// root item
		return createIndex (row, column, (void*) m_pCatalog);
	}
	else
	{
		// take entry from parent item
		if (row > pParent->countChapter()) 
			return QModelIndex();
		return createIndex (row, column, (void*)pParent->chapterAt(row));
	}
	return QModelIndex();
}

QModelIndex CCatalogModel::parent (const QModelIndex& index) const
{
CChapter *pIndex = (CChapter*) index.internalPointer();
int iRow=0;

	if (pIndex == 0 || pIndex->parentChapter() == 0) // root item
		return QModelIndex(); // has no parent

	CChapter *pParent = pIndex->parentChapter();
	if (pParent->parentChapter())
		iRow = pParent->parentChapter()->indexOfChapter(pParent);
	else // parent is the root item (parent of the parent doesn't exist)
		iRow = 0;

	return createIndex (iRow, 0 /*index.column()*/, pParent);
}

int CCatalogModel::rowCount (const QModelIndex& parent) const
{
CChapter *pParent = (CChapter*) parent.internalPointer();

	if (m_pCatalog == 0 || m_pCatalog->isEmpty()) return 0;

	if (!parent.isValid()) // root item
		return 1;

	return pParent->countChapter();
}

