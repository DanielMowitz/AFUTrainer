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
#include "ui_dlgexamstatistic.h"

class CCatalog;

class CDlgExamStatistic : public QDialog, Ui::DlgExamStatistic
{
Q_OBJECT
public:
	CDlgExamStatistic(QWidget *pParent=0);
	~CDlgExamStatistic()							{}

	void go (CCatalog *pCatalog);

protected slots:
	void on_cbExam_currentIndexChanged (int index);

protected:
	CCatalog *m_pCatalog;
};
