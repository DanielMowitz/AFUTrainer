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

#include "dlglearnstatistic.h"
#include "chapter.h"
#include "tools.h"
#include "plotwidget.h"

#include <qdatetime.h>

CDlgLearnStatistic::CDlgLearnStatistic(QWidget *pParent) : QDialog(pParent)
{


	m_pChapter = 0;
	setupUi(this);
	cbPeriod->addItem(tr("2 Wochen"), 14);
	cbPeriod->addItem(tr("1 Monat"), 30);
	cbPeriod->addItem(tr("3 Monate"), 90);
	cbPeriod->addItem(tr("6 Monate"), 30*6);
	cbPeriod->addItem(tr("1 Jahr"), 365);
	cbPeriod->setCurrentIndex(0);

	m_pPlotLevel = new CPlotWidget(groupLevel);
	m_pPlotLevel->setFrameShape(QFrame::Panel);
	m_pPlotLevel->setFrameShadow(QFrame::Sunken);
	groupLevel->layout()->addWidget(m_pPlotLevel);

	m_pPlotLevel->setBorderDistance(25, 5, 5, 25);
	m_pPlotLevel->setLimitY(0.0, 5.5);
	m_pPlotLevel->setBorderPen(QPen(Qt::black));
	m_pPlotLevel->setBorder(CPlotWidget::BorderLeft | CPlotWidget::BorderBottom);
	m_pPlotLevel->setTicY(1.0);

	QList<CPlotWidgetTic> listTics;
	for (int i=0; i<=LEVEL_MAX; i++)
	{
		CPlotWidgetTic tic (i, QPixmap(CQuestion::levelIconName(i)));
		listTics.append(tic);
	}
	m_pPlotLevel->setTicListY(listTics);

	m_pPlotClicked = new CPlotWidget(groupClicked);
	m_pPlotClicked->setFrameShape(QFrame::Panel);
	m_pPlotClicked->setFrameShadow(QFrame::Sunken);
	groupClicked->layout()->addWidget(m_pPlotClicked);

	m_pPlotClicked->setType(CPlotWidget::PlotBarsSum);
	m_pPlotClicked->setBarOffset(-0.5);
	m_pPlotClicked->setBorderDistance(25, 5, 5, 25);

	// Zeitaufwand
	cbPeriod2->addItem(tr("2 Wochen"), 14);
	cbPeriod2->addItem(tr("1 Monat"), 30);
	cbPeriod2->addItem(tr("3 Monate"), 90);
	cbPeriod2->addItem(tr("6 Monate"), 30*6);
	cbPeriod2->addItem(tr("1 Jahr"), 365);
	cbPeriod2->setCurrentIndex(0);

	m_pPlotTime = new CPlotWidget(groupTimeExpediture);
	m_pPlotTime->setFrameShape(QFrame::Panel);
	m_pPlotTime->setFrameShadow(QFrame::Sunken);
	groupTimeExpediture->layout()->addWidget(m_pPlotTime);

	m_pPlotTime->setType(CPlotWidget::PlotBarsSum);
	m_pPlotTime->setBarOffset(-0.5);
	m_pPlotTime->setBorderDistance(25, 5, 5, 25);
/*
	m_pPlotTimePerQuestion = new CPlotWidget(groupTimeExpediturePerQuestion);
	m_pPlotTimePerQuestion->setFrameShape(QFrame::Panel);
	m_pPlotTimePerQuestion->setFrameShadow(QFrame::Sunken);
	groupTimeExpediturePerQuestion->layout()->addWidget(m_pPlotTimePerQuestion);

	m_pPlotTimePerQuestion->setType(CPlotWidget::PlotLines);
	m_pPlotTimePerQuestion->setBorderDistance(25, 5, 5, 25);
*/
}

void CDlgLearnStatistic::go(CChapter *pChapter)
{
	m_pChapter = pChapter;
	updateTable();
	updateHistory();
	updateTimeExpediture();
	exec();
}


void CDlgLearnStatistic::updateTable()
{
const int w=60, h=16;
QList<CQuestion*> listQuestion = m_pChapter->questionPool();
CDayStatistic ds;

	// CHAPTER STATISTICS
	labChapter->setText (m_pChapter->text());
	labChapterCount->setText(QString("%1").arg(listQuestion.size()));
	labChapterVeryOften->setText(QString("%1").arg(m_pChapter->countQuestion(0)));
	labChapterOften->setText(QString("%1").arg(m_pChapter->countQuestion(1)));
	labChapterNormal->setText(QString("%1").arg(m_pChapter->countQuestion(2)));
	labChapterRare->setText(QString("%1").arg(m_pChapter->countQuestion(3)));
	labChapterVeryRare->setText(QString("%1").arg(m_pChapter->countQuestion(4)));
	labChapterExtremeRare->setText(QString("%1").arg(m_pChapter->countQuestion(5)));
	labChapterAvgText->setText(m_pChapter->levelAvgText());
	labChapterAvgIcon->setPixmap(m_pChapter->levelAvgPixmap());
	labChapterAvgIcon->setToolTip(QString("Kennzahl: %1").arg(m_pChapter->levelAvg(), 4, 'g', 2));

	double dQuestionCount = listQuestion.size(), dPercent=0.0;
	dPercent = (double)m_pChapter->countQuestion(0)/dQuestionCount;
	labChapterVeryOftenBar->setPixmap(createProgressBar(w, h, dPercent));
	labChapterVeryOftenBar->setToolTip(QString("%1 %").arg(dPercent * 100, 0, 'f', 1));
	dPercent = (double)m_pChapter->countQuestion(1)/dQuestionCount;
	labChapterOftenBar->setPixmap(createProgressBar(w, h, dPercent));
	labChapterOftenBar->setToolTip(QString("%1 %").arg(dPercent * 100, 0, 'f', 1));
	dPercent = (double)m_pChapter->countQuestion(2)/dQuestionCount;
	labChapterNormalBar->setPixmap(createProgressBar(w, h, dPercent));
	labChapterNormalBar->setToolTip(QString("%1 %").arg(dPercent * 100, 0, 'f', 1));
	dPercent = (double)m_pChapter->countQuestion(3)/dQuestionCount;
	labChapterRareBar->setPixmap(createProgressBar(w, h, dPercent));
	labChapterRareBar->setToolTip(QString("%1 %").arg(dPercent * 100, 0, 'f', 1));
	dPercent = (double)m_pChapter->countQuestion(4)/dQuestionCount;
	labChapterVeryRareBar->setPixmap(createProgressBar(w, h, dPercent));
	labChapterVeryRareBar->setToolTip(QString("%1 %").arg(dPercent * 100, 0, 'f', 1));
	dPercent = (double)m_pChapter->countQuestion(5)/dQuestionCount;
	labChapterExtremeRareBar->setPixmap(createProgressBar(w, h, dPercent));
	labChapterExtremeRareBar->setToolTip(QString("%1 %").arg(dPercent * 100, 0, 'f', 1));

	ds = m_pChapter->dayStatistic(QDate());
	labClickCount->setText(QString::number(ds.clickedCount()));
	labClickCorrect->setText(QString::number(ds.clickedCorrect()));
	labClickWrong->setText(QString::number(ds.clickedWrong()));
	labClickCorrectP->setText(ds.clickedCount() != 0 ? QString("%1 %").arg(100.0 * ds.clickedCorrect() / ds.clickedCount(), 0, 'f', 1) : "--");
	labClickWrongP->setText(ds.clickedCount() != 0 ? QString("%1 %").arg(100.0 * ds.clickedWrong() / ds.clickedCount(), 0, 'f', 1) : "--");
	labTimeExpediture->setText(QString("%1 h %2 m %3 s").arg(ds.timeExpediture() / 3600000).arg(ds.timeExpediture() / 60000 % 60, 2, 10, QChar('0')).arg(ds.timeExpediture()/1000 % 60, 2, 10, QChar('0')));
	labTimeExpeditureCorrect->setText(QString("%1 h %2 m %3 s").arg(ds.timeExpeditureCorrect() / 3600000).arg(ds.timeExpeditureCorrect() / 60000 % 60, 2, 10, QChar('0')).arg(ds.timeExpeditureCorrect()/1000 % 60, 2, 10, QChar('0')));
	labTimeExpeditureWrong->setText(QString("%1 h %2 m %3 s").arg(ds.timeExpeditureWrong() / 3600000).arg(ds.timeExpeditureWrong() / 60000 % 60, 2, 10, QChar('0')).arg(ds.timeExpeditureWrong()/1000 % 60, 2, 10, QChar('0')));
	labTimeExpeditureCorrectP->setText(ds.timeExpediture() != 0 ? QString("%1 %").arg(100.0 * ds.timeExpeditureCorrect() / ds.timeExpediture(), 0, 'f', 1) : "--");
	labTimeExpeditureWrongP->setText(ds.timeExpediture() != 0 ? QString("%1 %").arg(100.0 * ds.timeExpeditureWrong() / ds.timeExpediture(), 0, 'f', 1) : "--");
	labTimePerQuestion->setText(ds.clickedCount() != 0 ? QString("%2 m %3 s").arg(ds.timeExpediture() / ds.clickedCount() / 60000).arg(ds.timeExpediture() / 1000 / ds.clickedCount()  % 60, 2, 10, QChar('0')) : "--");
	labTimePerQuestionCorrect->setText(ds.clickedCorrect() != 0 ? QString("%2 m %3 s").arg(ds.timeExpeditureCorrect() / ds.clickedCorrect() / 60000).arg(ds.timeExpeditureCorrect() / 1000 / ds.clickedCorrect()  % 60, 2, 10, QChar('0')) : "--");
	labTimePerQuestionWrong->setText(ds.clickedWrong() != 0 ? QString("%2 m %3 s").arg(ds.timeExpeditureWrong() / ds.clickedWrong() / 60000).arg(ds.timeExpeditureWrong() / 1000 / ds.clickedWrong()  % 60, 2, 10, QChar('0')) : "--");
}

void CDlgLearnStatistic::on_cbPeriod_currentIndexChanged(int index)
{
	Q_UNUSED(index);
	updateHistory();
}

void CDlgLearnStatistic::updateHistory()
{
int iDays = cbPeriod->itemData(cbPeriod->currentIndex()).toInt(), idx=0;
CPlotWidgetCurve curveLevel, curveClickedCorrect, curveClickedWrong;
QDate date = QDate::currentDate();
CDayStatistic ds;
QList<CPlotWidgetTic> listTics;
CPlotWidgetTic tic;
bool bAddTic=false;
unsigned uMaxClicked=0;

	if (m_pChapter == 0) return;
	setCursor(Qt::WaitCursor);
	date = date.addDays(-iDays);

	tic.setLineType(CPlotWidgetTic::LineNone);
	tic.setFillType(CPlotWidgetTic::FillAll);
	tic.setPen(QPen(Qt::darkGray));

	while (date <= QDate::currentDate())
	{
		if (bAddTic)
		{
			bAddTic=false;
			tic.setPos(idx-1);
			if (listTics.size() % 2)
				tic.setBrush(QBrush(Qt::white));
			else
				tic.setBrush(QBrush(QColor(232,232,232)));
			listTics.append(tic);
		}

		ds = m_pChapter->dayStatistic(date);
		curveLevel.append(CPlotWidgetPoint(idx, ds.level()));
		curveClickedCorrect.append(CPlotWidgetPoint((double)idx, ds.clickedCorrect()));
		curveClickedWrong.append(CPlotWidgetPoint((double)idx, ds.clickedWrong()));
		if (ds.clickedCount() > uMaxClicked) uMaxClicked = ds.clickedCount();

		date = date.addDays(1);

		// X-Tics
		if (iDays <= 14)
		{ // daily tics
			tic.setText(date.toString("ddd"));
			tic.setWidth(1);
			if (date.dayOfWeek() == Qt::Monday)
				tic.setLineType(CPlotWidgetTic::LinePlot);
			else
				tic.setLineType(CPlotWidgetTic::LineNone);

			bAddTic=true;
		}
		else if (iDays <= 90 && date.dayOfWeek() == Qt::Monday)
		{
			tic.setText(QString("KW %1").arg(date.weekNumber()));
			tic.setWidth(7);
			bAddTic=true;
		}
		else if (iDays > 90 && date.day() == 1)
		{
			tic.setText(date.toString("MMM"));
			tic.setWidth(date.daysInMonth());
			if (date.month() == 1)
				tic.setLineType(CPlotWidgetTic::LinePlot);
			else
				tic.setLineType(CPlotWidgetTic::LineNone);
			bAddTic=true;
		}

		idx++;
	}

	m_pPlotLevel->setTicListX(listTics);
	m_pPlotClicked->setTicListX(listTics);


	curveLevel.setPen(QPen(Qt::darkBlue));
	curveClickedCorrect.setPen(QPen(Qt::green));
	curveClickedCorrect.setBrush(QBrush(Qt::green));
	curveClickedWrong.setPen(QPen(Qt::red));
	curveClickedWrong.setBrush(QBrush(Qt::red));


	m_pPlotLevel->clearCurves();
	m_pPlotLevel->appendCurve(curveLevel);
	m_pPlotLevel->update();

	m_pPlotClicked->clearCurves();
	m_pPlotClicked->appendCurve(curveClickedCorrect);
	m_pPlotClicked->appendCurve(curveClickedWrong);
	m_pPlotClicked->update();

	unsigned uDiffBase[] = {1, 2, 5, 10}, uDiffMulti=1, uDiff=0, uDiffCount=0;
	idx = 0;
	do
	{
		if (idx == 4) { idx = 0; uDiffMulti*=10; }
		uDiff = uDiffBase[idx] * uDiffMulti;
		uDiffCount = (uMaxClicked / uDiff) + 1;
		idx++;
	}
	while (uDiffCount > 9);

	m_pPlotClicked->setLimitY(0, uDiffCount * uDiff + uDiff/2);

	listTics.clear();
	tic.clear();
	tic.setTextFlags(Qt::AlignRight | Qt::AlignVCenter);
	for (unsigned u=0; u<=uDiffCount * uDiff; u+=uDiff)
	{
		tic.setPos(u);
		tic.setText(QString("%1").arg(u));
		listTics.append(tic);
	}
	m_pPlotClicked->setTicListY(listTics);

	setCursor(Qt::ArrowCursor);
}


void CDlgLearnStatistic::on_cbPeriod2_currentIndexChanged(int index)
{
	Q_UNUSED(index);
	updateTimeExpediture();
}

void CDlgLearnStatistic::updateTimeExpediture()
{
int iDays = cbPeriod2->itemData(cbPeriod2->currentIndex()).toInt(), idx=0;
CPlotWidgetCurve curveTimeWrong, curveTimeCorrect,curveTimePQWrong, curveTimePQCorrect; // PQ=per question
QDate date = QDate::currentDate();
CDayStatistic ds;
QList<CPlotWidgetTic> listTics;
CPlotWidgetTic tic;
bool bAddTic=false;
unsigned uMaxTime=0;

	if (m_pChapter == 0) return;
	setCursor(Qt::WaitCursor);
	date = date.addDays(-iDays);

	tic.setLineType(CPlotWidgetTic::LineNone);
	tic.setFillType(CPlotWidgetTic::FillAll);
	tic.setPen(QPen(Qt::darkGray));

	while (date <= QDate::currentDate())
	{
		if (bAddTic)
		{
			bAddTic=false;
			tic.setPos(idx-1);
			if (listTics.size() % 2)
				tic.setBrush(QBrush(Qt::white));
			else
				tic.setBrush(QBrush(QColor(232,232,232)));
			listTics.append(tic);
		}

		ds = m_pChapter->dayStatistic(date);
		curveTimeWrong.append(CPlotWidgetPoint(idx, (double)ds.timeExpeditureWrong()/1000.0/60.0));
		curveTimeCorrect.append(CPlotWidgetPoint(idx, (double)ds.timeExpeditureCorrect()/1000.0/60.0));
/*		if (ds.clickedWrong() > 0)
			curveTimePQWrong.append(CPlotWidgetPoint(idx, (double)ds.timeExpeditureWrong()/1000.0/60.0/(double)ds.clickedWrong()));
		else
			curveTimePQWrong.append(CPlotWidgetPoint(idx, 0));
		if (ds.clickedCorrect() > 0)
			curveTimePQCorrect.append(CPlotWidgetPoint(idx, (double)ds.timeExpeditureCorrect()/1000.0/60.0/(double)ds.clickedCorrect()));
		else
			curveTimePQWrong.append(CPlotWidgetPoint(idx, 0));*/
		if (ds.timeExpediture()/1000/60 > uMaxTime) uMaxTime = ds.timeExpediture()/1000/60;

		date = date.addDays(1);

		// X-Tics
		if (iDays <= 14)
		{ // daily tics
			tic.setText(date.toString("ddd"));
			tic.setWidth(1);
			if (date.dayOfWeek() == Qt::Monday)
				tic.setLineType(CPlotWidgetTic::LinePlot);
			else
				tic.setLineType(CPlotWidgetTic::LineNone);

			bAddTic=true;
		}
		else if (iDays <= 90 && date.dayOfWeek() == Qt::Monday)
		{
			tic.setText(QString("KW %1").arg(date.weekNumber()));
			tic.setWidth(7);
			bAddTic=true;
		}
		else if (iDays > 90 && date.day() == 1)
		{
			tic.setText(date.toString("MMM"));
			tic.setWidth(date.daysInMonth());
			if (date.month() == 1)
				tic.setLineType(CPlotWidgetTic::LinePlot);
			else
				tic.setLineType(CPlotWidgetTic::LineNone);
			bAddTic=true;
		}

		idx++;
	}

	m_pPlotTime->setTicListX(listTics);
	//m_pPlotTimePerQuestion->setTicListX(listTics);

	curveTimeCorrect.setPen(QPen(Qt::green));
	curveTimeCorrect.setBrush(QBrush(Qt::green));
	curveTimeWrong.setPen(QPen(Qt::red));
	curveTimeWrong.setBrush(QBrush(Qt::red));

	m_pPlotTime->clearCurves();
	m_pPlotTime->appendCurve(curveTimeCorrect);
	m_pPlotTime->appendCurve(curveTimeWrong);
	m_pPlotTime->update();
/*
	m_pPlotTimePerQuestion->clearCurves();
	m_pPlotTimePerQuestion->appendCurve(curveTimePQCorrect);
	m_pPlotTimePerQuestion->appendCurve(curveTimePQWrong);
	m_pPlotTimePerQuestion->setAutoLimitRoundY(0.1);
	m_pPlotTimePerQuestion->update();
*/
	unsigned uDiffBase[] = {1, 2, 5, 10}, uDiffMulti=1, uDiff=0, uDiffCount=0;
	idx = 0;
	do
	{
		if (idx == 4) { idx = 0; uDiffMulti*=10; }
		uDiff = uDiffBase[idx] * uDiffMulti;
		uDiffCount = (uMaxTime / uDiff) + 1;
		idx++;
	}
	while (uDiffCount > 9);

	m_pPlotTime->setLimitY(0, uDiffCount * uDiff + uDiff/2);

	listTics.clear();
	tic.clear();
	tic.setTextFlags(Qt::AlignRight | Qt::AlignVCenter);
	for (unsigned u=0; u<=uDiffCount * uDiff; u+=uDiff)
	{
		tic.setPos(u);
		tic.setText(QString("%1").arg(u));
		listTics.append(tic);
	}
	m_pPlotTime->setTicListY(listTics);

	setCursor(Qt::ArrowCursor);
}
