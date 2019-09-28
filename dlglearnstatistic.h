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

#include <qdialog.h>
#include "ui_dlglearnstatistic.h"

class CChapter;
class CPlotWidget;

class CDlgLearnStatistic : public QDialog, Ui::DlgLearnStatistic
{
Q_OBJECT
public:
	CDlgLearnStatistic(QWidget *pParent=0);
	~CDlgLearnStatistic()							{}

	void go(CChapter *pChapter);

protected slots:
	void on_cbPeriod_currentIndexChanged(int index);
	void on_cbPeriod2_currentIndexChanged(int index);

protected:
	void updateTable();
	void updateHistory();
	void updateTimeExpediture();

protected:
	CChapter *m_pChapter;
	CPlotWidget *m_pPlotLevel;
	CPlotWidget *m_pPlotClicked;
	CPlotWidget *m_pPlotTime;
//	CPlotWidget *m_pPlotTimePerQuestion;
};
