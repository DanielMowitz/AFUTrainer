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

#include "dlglearnassistant.h"

#include <qheaderview.h>
#include <qmessagebox.h>

#include "catalog.h"

CDlgLearnAssistant::CDlgLearnAssistant (QWidget *pParent) : QDialog(pParent)
{
	m_pCatalog=0;
	setupUi(this);

	tvChapters->setModel (&m_modelChapter);
	tvChapters->header()->setStretchLastSection(false);
	tvChapters->header()->setResizeMode (0, QHeaderView::Stretch);
	tvChapters->header()->setResizeMode (1, QHeaderView::Interactive);
	tvChapters->header()->setResizeMode (3, QHeaderView::Interactive);
	tvChapters->header()->resizeSection(1, 50);
	tvChapters->header()->resizeSection(2, 140);
	tvChapters->header()->resizeSection(3, 20);
}


bool CDlgLearnAssistant::setup(CCatalog *pCatalog)
{
bool bEnableRepeat;
QList<CChapter*> listAll, listNow, listToday, listSelected;
CChapter *c;

	m_pCatalog = pCatalog;
	if (pCatalog->recommendationCount (CChapter::RecommendationRepeatToday) > 0)
	{
		bEnableRepeat = true;
		labRepeat->setText(tr("Es sind %1 Fragen vorhanden").arg(pCatalog->recommendedQuestions().count()));
		rbRepeat->setChecked(true);
	}
	else
	{
		bEnableRepeat = false;
		labRepeat->setText(tr("Keine Fragen vorhanden."));
		rbList->setChecked(true);
	}
	rbRepeat->setEnabled(bEnableRepeat);
	labRepeat->setEnabled(bEnableRepeat);

	// update list
	listAll = pCatalog->chapters();
	for (int i=0; i<listAll.size(); i++)
	{
		c = listAll.at(i);
		if (c->isRecommendedNow(pCatalog))
			listNow << c;
		else if (c->recommendation() == CChapter::RecommendationLearnNew)
			listToday << c;
	}

	listSelected << listNow << listToday;
	m_modelChapter.setModelData (pCatalog, listSelected);

	if (!bEnableRepeat && listSelected.isEmpty()) return false;

	updateEnable();

	return true;
}

void CDlgLearnAssistant::updateEnable()
{
	labRepeat->setEnabled(rbRepeat->isChecked());
	tvChapters->setEnabled(rbList->isChecked());
}


void CDlgLearnAssistant::on_buttonBox_accepted()
{
QModelIndexList list = tvChapters->selectionModel()->selectedIndexes();
	if (rbList->isChecked() && list.isEmpty())
	{
		QMessageBox::information(this, tr("Information"), tr("Bitte markieren Sie in der Liste ein Kapitel, das Sie lernen möchten!"));
		return;
	}
	accept();
}

CChapter* CDlgLearnAssistant::selectedChapter()
{
QModelIndexList list = tvChapters->selectionModel()->selectedIndexes();
	if (rbRepeat->isChecked())
		return m_pCatalog;
	if (rbList->isChecked())
	{
		if (list.isEmpty()) return 0;
		return (CChapter*)list.first().internalPointer();
	}
	return 0;
}
