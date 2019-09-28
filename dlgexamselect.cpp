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

#include "dlgexamselect.h"

#include <qmessagebox.h>
#include <qheaderview.h>
#include "catalog.h"

CDlgExamSelect::CDlgExamSelect (QWidget *pParent) : QDialog (pParent)
{
	m_pCatalog = 0;
	m_iSelectedExam = -1;
	setupUi(this);

	twList->headerItem()->setText(0, tr("Name"));
	twList->headerItem()->setText(1, tr("Dauer (min)"));
	//twList->header()->setStretchLastSection(false);
	twList->header()->resizeSection(0, 370);
	//twList->header()->resizeSection(1, 70);

}

CDlgExamSelect::~CDlgExamSelect ()
{
}

void CDlgExamSelect::setup (const CCatalog *pCatalog)
{
	m_pCatalog = pCatalog;
	for (int i=0; i<m_pCatalog->countExam(); i++)
	{
		CExam exam = m_pCatalog->examAt(i);
		QTreeWidgetItem *pItem = new QTreeWidgetItem (twList);
		pItem->setText(0, exam.name());
		pItem->setText(1, QString("%1 min").arg(exam.duration()));
		pItem->setTextAlignment(1, Qt::AlignRight);
		pItem->setData(0, Qt::UserRole, i);
	}
}

void CDlgExamSelect::on_twList_currentItemChanged (QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Q_UNUSED(previous);
	if (current)
	{
		m_iSelectedExam = current->data(0, Qt::UserRole).toInt();
		CExam exam = m_pCatalog->examAt(m_iSelectedExam);
		labName->setText(exam.name());
		labComment->setText(exam.comment());
		labDuration->setText(QString("%1 min").arg(exam.duration()));
		labQuestions->setText(QString("%1").arg(exam.questionCount()));
		labError->setText(QString("%1").arg(exam.maxErrorPoints()));
	}
	else
	{
		m_iSelectedExam = -1;
		labName->clear();
		labComment->clear();
		labDuration->clear();
		labQuestions->clear();
		labError->clear();
	}
}

void CDlgExamSelect::on_buttonBox_accepted()
{
	if (m_iSelectedExam == -1)
	{
		QMessageBox::information(this, tr("Information"), tr("Bitte eine Prüfung auswählen!"));
		return;
	}
	accept();
}

