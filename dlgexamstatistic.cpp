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

#include "dlgexamstatistic.h"
#include "catalog.h"

#include <qmessagebox.h>
#include <qheaderview.h>

CDlgExamStatistic::CDlgExamStatistic(QWidget *pParent) : QDialog(pParent)
{
	m_pCatalog = 0;
	setupUi(this);

	twExamStat->headerItem()->setText(0, "Datum/Uhrzeit");
	twExamStat->headerItem()->setText(1, "Ergebnis");
	twExamStat->headerItem()->setText(2, "Fehler");
	twExamStat->headerItem()->setText(3, "Dauer");
	twExamStat->headerItem()->setText(4, "Ø pro Frage");

	twExamStat->header()->resizeSection(0, 130);
	twExamStat->header()->resizeSection(2, 50);
	twExamStat->header()->resizeSection(3, 75);
}

void CDlgExamStatistic::go (CCatalog *pCatalog)
{
	m_pCatalog = pCatalog;

	if (pCatalog->countExamStat() == 0)
	{
		QMessageBox::information(parentWidget(), tr("Information"), tr("Es wurden bisher noch keine Prüfungen durchgeführt."));
		return;
	}

	cbExam->clear();
	for (int i=0; i<pCatalog->countExam(); i++)
	{
		cbExam->addItem(pCatalog->examAt(i).name(), pCatalog->examAt(i).id());
	}

	exec();
}

void CDlgExamStatistic::on_cbExam_currentIndexChanged (int index)
{
QString strExamId = cbExam->itemData(index).toString();
CExam exam = m_pCatalog->examById(strExamId);
unsigned uExamCount=0, uPassedCount=0, uFailedCount=0, uErrorCount=0, uDuration=0, uQuestions=0;
QTreeWidgetItem *pItem=0;

	twExamStat->clear();

	labExamQCount->setText(QString::number(exam.questionCount()));
	labExamTime->setText(QString("%1 min").arg(exam.duration()));
	labExamWrong->setText(QString::number(exam.maxErrorPoints()));

	for (int i=0; i<m_pCatalog->countExamStat(); i++)
	{
		CExamStat es = m_pCatalog->examStatAt(i);
		if (es.id() != strExamId) continue;
		uExamCount++;
		if (es.passed()) uPassedCount++; else uFailedCount++;
		uErrorCount += es.errorPoints();
		uDuration += es.duration();
		uQuestions += es.correctAnswers() + es.wrongAnswers();
		pItem = new QTreeWidgetItem (twExamStat);
		pItem->setText(0, es.datetime().toString(Qt::LocalDate));
		pItem->setText(1, es.passed() ? tr("Bestanden") : tr("Nicht bestanden"));
		pItem->setText(2, QString::number(es.errorPoints()));
		pItem->setTextAlignment(2, Qt::AlignRight);
		pItem->setText(3, QString("%1 m %2 s").arg(es.duration() / 60).arg(es.duration() % 60, 2, 10, QChar('0')));
		pItem->setTextAlignment(3, Qt::AlignRight);
		unsigned uQuestionCount = es.correctAnswers() + es.wrongAnswers();
		if (uQuestionCount != 0)
			pItem->setText(4, QString("%1 m %2 s").arg(es.duration() / uQuestionCount / 60).arg(es.duration() / uQuestionCount % 60, 2, 10, QChar('0')));
		else
			pItem->setText(4, "--");
		pItem->setTextAlignment(4, Qt::AlignRight);
	}

	labExamCount->setText(QString::number(uExamCount));
	labExamPassed->setText(uExamCount != 0 ? QString::number(uPassedCount) : "--");
	labExamFailed->setText(uExamCount != 0 ? QString::number(uFailedCount) : "--");
	labExamPassedP->setText(uExamCount != 0 ? QString("%1 %").arg((double)uPassedCount / uExamCount * 100.0, 0, 'f', 1) : "--");
	labExamFailedP->setText(uExamCount != 0 ? QString("%1 %").arg((double)uFailedCount / uExamCount * 100.0, 0, 'f', 1) : "--");
	labAvgWrong->setText(uExamCount != 0 ? QString::number(uErrorCount / uExamCount) : "--");
	labTimeExam->setText(uExamCount != 0 ? QString("%1 m %2 s").arg(uDuration / uExamCount / 60).arg(uDuration / uExamCount % 60, 2, 10, QChar('0')) : "--");
	labTimeQuestion->setText(uQuestions != 0 ? QString("%1 m %2 s").arg(uDuration / uQuestions / 60).arg(uDuration / uQuestions % 60, 2, 10, QChar('0')) : "--");
}

