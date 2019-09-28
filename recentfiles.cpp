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

#include "recentfiles.h"

#include <qmenu.h>
#include <qstringlist.h>
#include <qsettings.h>

CRecentFiles::CRecentFiles(QObject *pParent) : QObject(pParent)
{
	m_actNoEntry = new QAction(this);
	m_actSepTop = new QAction(this);
	m_actSepTop->setSeparator(true);
	m_actSepTop->setVisible(false);
	m_actSepBottom = new QAction(this);
	m_actSepBottom->setSeparator(true);
	m_actSepBottom->setVisible(false);
	m_showSeperator = SeperatorNone;

	m_actNoEntry->setEnabled(false);
	m_bShowNoEntry = true;
	onLanguageChanged();
}

CRecentFiles::~CRecentFiles()
{
	qDeleteAll(m_listActions);
}

void CRecentFiles::clear()
{
	qDeleteAll(m_listActions);
	m_listActions.clear();
}

void CRecentFiles::onLanguageChanged()
{
	m_actNoEntry->setText(tr("No recent files"));
}

void CRecentFiles::create (const QString& strName, const unsigned uMaxCount)
{
QAction *pAct=0;
	qDeleteAll(m_listActions);
	m_listActions.clear();
	m_strName = strName;
	m_uMaxCount = uMaxCount;

	for (unsigned u=0; u<uMaxCount; u++)
	{
		pAct = new QAction(this);
		pAct->setVisible(false);
		connect(pAct, SIGNAL(triggered()), this, SLOT(onRecentFile()));
		m_listActions.append(pAct);
     }
	updateActions();
}

void CRecentFiles::insertToMenu(QMenu *pMenu, QAction *pBefore)
{
	pMenu->insertAction(pBefore, m_actSepTop);
	pMenu->insertAction(pBefore, m_actNoEntry);
	pMenu->insertActions(pBefore, m_listActions);
	pMenu->insertAction(pBefore, m_actSepBottom);
}

QString CRecentFiles::settingName() const
{
QString str = "RecentFileList";
	if (!m_strName.isEmpty()) str += "_" + m_strName;
	return str;
}

void CRecentFiles::onRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
		loadFile(action->data().toString());
}

void CRecentFiles::setRecentFile(const QString& strFileName)
{
QSettings set;
QStringList strl = set.value(settingName()).toStringList();

	strl.removeAll(strFileName);
	strl.prepend(strFileName);
	while ((unsigned)strl.size() > m_uMaxCount)
		strl.removeLast();

	set.setValue(settingName(), strl);

	updateActions();
}

void CRecentFiles::removeFile(const QString& strFileName)
{
QSettings set;
QStringList strl = set.value(settingName()).toStringList();
	strl.removeAll(strFileName);
	set.setValue(settingName(), strl);
	updateActions();
}

void CRecentFiles::removeAll()
{
QSettings set;
	set.setValue(settingName(), QStringList());
	updateActions();
}

void CRecentFiles::updateActions()
{
QSettings set;
QStringList strl = set.value(settingName()).toStringList();
unsigned uNumRecentFiles = qMin((unsigned)strl.size(), m_uMaxCount);
QString str;

	m_actNoEntry->setVisible(m_bShowNoEntry && uNumRecentFiles == 0);
	m_actSepTop->setVisible((m_showSeperator == SeperatorTop || m_showSeperator == SeperatorBoth) && uNumRecentFiles != 0);
	m_actSepBottom->setVisible((m_showSeperator == SeperatorBottom || m_showSeperator == SeperatorBoth) && uNumRecentFiles != 0);

	for (unsigned i=0; i<uNumRecentFiles; ++i)
	{
		str = tr("&%1 %2").arg(i + 1).arg(strl[i]);
		m_listActions[i]->setText(str);
		m_listActions[i]->setData(strl[i]);
		m_listActions[i]->setVisible(true);
     }
     for (unsigned j=uNumRecentFiles; j<m_uMaxCount; ++j)
         m_listActions[j]->setVisible(false);
}

QString CRecentFiles::recentFile(const int idx) const
{
QSettings set;
QStringList strl = set.value(settingName()).toStringList();
	if (idx >= strl.size()) return QString();
	return strl.at(idx);
}

void CRecentFiles::setShowNoEntry (const bool bShow)
{
QSettings set;
QStringList strl = set.value(settingName()).toStringList();
unsigned uNumRecentFiles = qMin((unsigned)strl.size(), m_uMaxCount);
	m_bShowNoEntry = bShow;
	m_actNoEntry->setVisible(m_bShowNoEntry && uNumRecentFiles == 0);
}

void CRecentFiles::setShowSeperator(const ShowSeperator show)
{
QSettings set;
QStringList strl = set.value(settingName()).toStringList();
unsigned uNumRecentFiles = qMin((unsigned)strl.size(), m_uMaxCount);
	m_showSeperator = show;
	m_actSepTop->setVisible((m_showSeperator == SeperatorTop || m_showSeperator == SeperatorBoth) && uNumRecentFiles != 0);
	m_actSepBottom->setVisible((m_showSeperator == SeperatorBottom || m_showSeperator == SeperatorBoth) && uNumRecentFiles != 0);
}
