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

#ifndef CATALOGMODEL_H
#define CATALOGMODEL_H

#include <qabstractitemmodel.h>

class CCatalog;

class CCatalogModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	CCatalogModel (QObject *pParent=0);
	~CCatalogModel();

	void setModelData (CCatalog *pCatalog);

public:
	virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual bool hasChildren (const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual QModelIndex index (int row, int column, const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex parent ( const QModelIndex & index ) const;
	virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

public slots:
	void onLanguageChanged();

protected:
	CCatalog *m_pCatalog;
};

#endif

