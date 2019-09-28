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

#ifndef DLGLEARNASSISTANT_H
#define DLGLEARNASSISTANT_H

#include <qdialog.h>
#include "ui_dlglearnassistant.h"

#include "chaptermodel.h"

class CDlgLearnAssistant : public QDialog, Ui::DlgLearnAssistant
{
	Q_OBJECT
public:
	CDlgLearnAssistant (QWidget *pParent=0);
	~CDlgLearnAssistant ()						{}

	bool setup(CCatalog *pCatalog);
	CChapter* selectedChapter();

protected slots:
	void on_buttonBox_accepted();
	inline void on_rbList_toggled()				{ updateEnable(); }
	inline void on_rbRepeat_toggled()			{ updateEnable(); }

protected:
	void updateEnable();

protected:
	CCatalog *m_pCatalog;
	CChapterModel m_modelChapter;

//	CRecommendation m_recomDeepen;
//	CRecommendation m_recomRepeat;
};

#endif
