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

#pragma once

#include <qlist.h>
#include <qaction.h>
#include <qstring.h>

class QMenu;

class CRecentFiles : public QObject
{
Q_OBJECT
public:
	enum ShowSeperator { SeperatorNone, SeperatorTop, SeperatorBottom, SeperatorBoth };

	CRecentFiles(QObject *pParent=0);
	~CRecentFiles();

	void clear();

	void create (const QString& strName, const unsigned uMaxCount=4);
	void insertToMenu(QMenu *pMenu, QAction *pBefore=0);

	inline unsigned maxCount() const								{ return m_uMaxCount; }
	inline QString name() const										{ return m_strName; }

	void setRecentFile(const QString& strFileName);
	void removeFile(const QString& strFileName);
	void removeAll();

	QString recentFile(const int idx) const;

	void setShowNoEntry (const bool bShow);
	inline bool showNoEntry() const									{ return m_bShowNoEntry; }

	void setShowSeperator(const ShowSeperator show);
	inline ShowSeperator showSeperator() const						{ return m_showSeperator; }

public slots:
	void onLanguageChanged();

protected slots:
	void onRecentFile();

signals:
	void loadFile(QString strFile);

protected:
	QString settingName() const;
	void updateActions();

protected:
	QString m_strName;
	unsigned m_uMaxCount;

	QList<QAction*> m_listActions;
	QAction *m_actNoEntry;
	QAction *m_actSepTop;
	QAction *m_actSepBottom;
	bool m_bShowNoEntry;
	ShowSeperator m_showSeperator;
};
