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

#include "catalog.h"
#include "dlglearn.h"
#include "dlgviewquestion.h"
#include "dlglearnassistant.h"
#include "tools.h"

#include <qmessagebox.h>

CDlgLearn::CDlgLearn (QWidget *pParent) : QDialog (pParent, Qt::WindowMaximizeButtonHint)
{
	m_pCatalog = 0;
	m_pChapter = 0;
	m_pQuestion=0;
	m_pLastQuestion=0;
	m_uLastAnswerMask=0;
	m_bHintsUsed=false;
	m_uElapsedBeforeBreak=0;
#ifdef _DEBUG
	m_bCheatEnable=true;
#else
	m_bCheatEnable=false;
#endif
	m_bAssistantEnable=true;
	setupUi(this);

	if (!m_bAssistantEnable)
		gbAssistant->hide();
}

CDlgLearn::~CDlgLearn()
{
}

void CDlgLearn::go (CCatalog *pCatalog, CChapter *pChapter)
{
	Q_ASSERT (pChapter != 0);
	m_pCatalog = pCatalog;
	setNewChapter(pChapter);

	m_ds = pCatalog->dayStatistic(QDate::currentDate());
	onUpdateDS();

	exec();
}

void CDlgLearn::setNewChapter(CChapter *pChapter)
{
	m_pQuestion=0;
	m_bHintsUsed=false;
	m_pChapter = pChapter;
	m_listQuestion = m_pChapter->questionPool();
	nextQuestion();
}

void CDlgLearn::updateStatistic()
{
const int w=60, h=16;

	// CHAPTER STATISTICS
	labChapter->setText (m_pChapter->text());
	labChapterCount->setText(QString("%1").arg(m_listQuestion.size()));
	labChapterVeryOften->setText(QString("%1").arg(m_pChapter->countQuestion(0)));
	labChapterOften->setText(QString("%1").arg(m_pChapter->countQuestion(1)));
	labChapterNormal->setText(QString("%1").arg(m_pChapter->countQuestion(2)));
	labChapterRare->setText(QString("%1").arg(m_pChapter->countQuestion(3)));
	labChapterVeryRare->setText(QString("%1").arg(m_pChapter->countQuestion(4)));
	labChapterExtremeRare->setText(QString("%1").arg(m_pChapter->countQuestion(5)));
	labChapterAvgText->setText(m_pChapter->levelAvgText());
	labChapterAvgIcon->setPixmap(m_pChapter->levelAvgPixmap());
	labChapterAvgIcon->setToolTip(QString("Kennzahl: %1").arg(m_pChapter->levelAvg(), 4, 'g', 2));

	double dQuestionCount = m_listQuestion.size(), dPercent;
//	dPercent = (double)m_listQuestion.size()/(double)m_pCatalog->countSubQuestion();
//	labChapterCountBar->setPixmap(createProgressBar(w, h, dPercent));
//	labChapterCountBar->setToolTip(QString("%1 %").arg(dPercent * 100, 0, 'f', 1));
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

	// QUESTION STATISTICS
	labQuestion->setText (tr("Frage %1").arg(m_pQuestion->id()));
	labQuestionLevelIcon->setPixmap(m_pQuestion->levelPixmap());
	labQuestionLevelText->setText(m_pQuestion->levelText());
	if (m_pQuestion->clickedCorrectSuccessive() != 0)
		labQuestionSuccessive->setText(tr("%1x richtig").arg(m_pQuestion->clickedCorrectSuccessive()));
	else if (m_pQuestion->clickedWrongSuccessive() != 0)
		labQuestionSuccessive->setText(tr("%1x falsch").arg(m_pQuestion->clickedWrongSuccessive()));
	else
		labQuestionSuccessive->setText(tr("--"));

	labQuestionCount->setText(QString("%1").arg(m_pQuestion->clickedCount()));
	labQuestionCorrect->setText(QString("%1").arg(m_pQuestion->clickedCorrect()));
	labQuestionWrong->setText(QString("%1").arg(m_pQuestion->clickedWrong()));
	QDateTime dtLastClicked = m_pQuestion->lastClicked();
	if (!dtLastClicked.isValid())
		labQuestionDate->setText("--");
	else
	{
		labQuestionDate->setText(m_pQuestion->lastClickedText());
	}

	onUpdateDS();
}

void CDlgLearn::updateLearnAssistant()
{
QString strRecom1, strRecom2, str;
QPixmap pixRecom1, pixRecom2, pix;

	// first recommendation
	strRecom1 = m_pChapter->recommendationTextExtended(m_pCatalog);
	if (m_pChapter->isRecommendedNow(m_pCatalog))
		strRecom1 = "<font color='green'>" + strRecom1 + "</font>";
	else
		strRecom1 = "<font color='red'>" + strRecom1 + "</font>";
	pixRecom1 = QPixmap (m_pChapter->recommendationIconName (m_pCatalog));

	// second recommendation
	strRecom2 =  m_pChapter->recommendationTextExtended2(m_pCatalog);
	pixRecom2 = QPixmap (m_pChapter->recommendationIconName (m_pChapter->recommendation2(), m_pCatalog));


	if (m_pChapter->recommendation() == CChapter::RecommendationSubChapter && m_pChapter->recommendation2() == CChapter::RecommendationRepeatToday)
	{
		labRecommendationIcon->setPixmap (pixRecom2);
		labRecommendation->setText("<font color='green'>" + strRecom2 + "</font>");
	}
	else
	{
		str = strRecom1;
		if (!strRecom2.isEmpty()) 
			str += "<p><font color='green'>" + strRecom2 + "</font>";
		labRecommendationIcon->setPixmap (pixRecom1);
		labRecommendation->setText(str);
	}
}

CQuestion *CDlgLearn::findNextQuestion()
{
CQuestion *p = 0;

	if (m_bAssistantEnable && m_pChapter->hasRecommendedQuestions())
	{
//		if (afu_random (0, 99) >= 0)	// hier kann ggf. das Verhältnis zwischen RecommendedQuestion und Zufalls-Question ausgewählt werden
//		{
			p = findNextTargetQuestion();
			// Wichtig: p kann hier immernoch 0 sein! Dies bedeutet, dass keine passende Frage im TargetPool gefunden wurde.
//		}
	}

	if (p == 0)
		p = findNextPoolQuestion();

	return p;
}

CQuestion *CDlgLearn::findNextTargetQuestion()
{
CQuestion *pQuestion=0;
QList<CQuestion*> list = m_pChapter->recommendedQuestions();
int iSize = list.size();
unsigned uRnd=0;

	if (iSize == 0) return 0;
	if (iSize == 1)
	{
		if (list.at(0) == m_pQuestion)
			// Regel: Niemals die gleiche Frage mehrfach hintereinander 
			// -> kann hier nicht erfüllt werden -> 0 zurückgeben -> hole Frage aus allg. Pool (siehe findNextQuestion)
			return 0;
		else
			return list.at(0);
	}

	do
	{
		uRnd = afu_random (0, iSize-1);
		pQuestion = list.at(uRnd);
	}
	while (m_pQuestion == pQuestion);

	return pQuestion;
}

CQuestion *CDlgLearn::findNextPoolQuestion()
{
CQuestion *pQuestion=0;
unsigned uCountQuestion[LEVEL_MAX+1];
unsigned uRnd=0, uLevel=0;
int i=0;
unsigned uGewichtung[LEVEL_MAX+1], uEdge[LEVEL_MAX+1];
unsigned uRndMax = 0;

	memset (uCountQuestion, 0, sizeof(unsigned)*(LEVEL_MAX+1));
	for (i=0;i<m_listQuestion.size(); i++) 
	{
		uLevel = m_listQuestion[i]->level();
		Q_ASSERT(uLevel < LEVEL_MAX+1);
		if (uLevel > LEVEL_MAX) continue;
		uCountQuestion[uLevel]++;
	}

	uGewichtung[LEVEL_MAX] = 1;
	uRndMax = 1;
	for (i=LEVEL_MAX-1; i>=0; i--) 
	{
		uRndMax += uGewichtung[i+1];
		uGewichtung[i] = uRndMax;
	}

	for (i=0; i<=LEVEL_MAX; i++) 
		uGewichtung[i] *= uCountQuestion[i];

	uRndMax=0;
	for (i=0; i<=LEVEL_MAX; i++)
	{
		uLevel = LEVEL_MAX-i;
		uRndMax += uGewichtung[uLevel];
		uEdge[uLevel] = uRndMax;
	}

	do /* diese Schleife verhindert, dass eine Frage zweimal hintereinander kommt */
	{
		do /* diese Schleife wählt die Abfragehäufigkeit aus */
		{
			uRnd = afu_random (1, uRndMax);
			uLevel = LEVEL_MAX;
			while (uLevel != LEVEL_VERYOFTEN && uRnd > uEdge[uLevel]) 
				uLevel--;

			Q_ASSERT(uCountQuestion[uLevel] != 0); // Wenn hier assertion failed => algorithmus falsch
		}
		while (uCountQuestion[uLevel] == 0);

		// Zufällige Frage bestimmen
		uRnd = afu_random (1, uCountQuestion[uLevel]);
		for (i=0; i<m_listQuestion.size()-1; i++)
			if (m_listQuestion[i]->level() == uLevel && --uRnd == 0) break;
	}
	while (m_pQuestion == m_listQuestion[i] && m_listQuestion.size() > 1);
	pQuestion = m_listQuestion[i];
	return pQuestion;
}

void CDlgLearn::nextQuestion()
{
QString str, strCheat;

	if (m_listQuestion.size() == 0)
	{
		QMessageBox::critical (this, "Fehler", "Es gibt keine Fragen, die gelernt werden könnten!");
		accept();
		return;
	}

	if (m_pQuestion)
	{	// Save current question as new last question
		pbLastQuestion->setEnabled(true);
		m_pLastQuestion = m_pQuestion;
	}

	m_pQuestion = findNextQuestion();

	// show answer
	m_pQuestion->mixAnswers();

	str = m_pQuestion->learnText(m_pCatalog, true, false);
	if (m_bCheatEnable)
	{
		strCheat = "<p><font size=-1><i><b>Schummel-Modus</b><br>Richtige Antwort: " + CAnswerClicked::answerText (m_pQuestion->correctAnswerMask()) + "</i>";
		strCheat += "<br>Empfohlene Wiederholung: " + m_pQuestion->repeatDateText();
		strCheat += QString("<br>isLearningNew(): %1").arg(m_pQuestion->isLearningNew());

		strCheat += "</font><p>";
	}
	teQuestion->setHtml(strCheat + str);

	pbShowHelper->setEnabled (m_pCatalog->hasHints(m_pQuestion->id()));
	m_bHintsUsed = false;
	m_timeElapsed.restart();
	updateStatistic();	
	updateLearnAssistant();
}

void CDlgLearn::on_pbAnswerA_clicked()
{
	handleAnswer(0);
}

void CDlgLearn::on_pbAnswerB_clicked()
{
	handleAnswer(1);
}

void CDlgLearn::on_pbAnswerC_clicked()
{
	handleAnswer(2);
}

void CDlgLearn::on_pbAnswerD_clicked()
{
	handleAnswer(3);
}

void CDlgLearn::handleAnswer(const int i)
{
unsigned uAnswerMask = 1<<i;
	
	if (!m_pQuestion->isCorrectAnswer(uAnswerMask))
	{
		QMessageBox::information(this, tr("Hinweis"), m_pQuestion->correctionText(uAnswerMask));
	}
	m_uLastAnswerMask=uAnswerMask;

	if (!m_bHintsUsed) 
	{
		m_pQuestion->registerAnswerClicked(uAnswerMask, m_timeElapsed.elapsed() + m_uElapsedBeforeBreak);

		bool bIsRecommendedOld = m_pChapter->isRecommendedNow(m_pCatalog) ;

		m_pCatalog->updateStatistic();
		m_ds = m_pCatalog->dayStatistic(QDate::currentDate());

		if (bIsRecommendedOld && !m_pChapter->isRecommendedNow(m_pCatalog))
		{
			QMessageBox msgBox(this);
			QPushButton *pbAssistant = msgBox.addButton(tr("Assistent"), QMessageBox::AcceptRole);
			QPushButton *pbIgnore = msgBox.addButton(QMessageBox::Ignore);
			QPushButton *pbExit = msgBox.addButton(QMessageBox::Cancel);
			pbAssistant->setIcon(QIcon(":/icons/16x16/idea_info.png"));
			pbAssistant->setToolTip(tr("Lern-Assistent aufrufen"));
			pbIgnore->setToolTip(tr("Meldung ignorieren, dieses Kapitel weiterlernen"));
			pbExit->setToolTip(tr("Lernmodus beenden"));
			msgBox.setText(tr("<b>Herzlichen Glückwunsch!</b><p>Sie haben das heutige Lernziel für dieses Kapitel erreicht.<br>Bitte folgen Sie den weiteren Empfehlungen des Lernassistents."));
			msgBox.setWindowTitle(tr("Ziel erreicht"));

			msgBox.exec();
			if (msgBox.clickedButton() == pbExit)
			{
				reject();
				return;
			}
			else if (msgBox.clickedButton() == pbAssistant)
			{
				on_pbLearnAssistant_clicked();
				return;
			}
		}
	}
	nextQuestion();
}

void CDlgLearn::on_pbShowHelper_clicked()
{
	m_bHintsUsed = true;
	QString str = m_pQuestion->learnText(m_pCatalog, true, true);
	teQuestion->setHtml(str);
}

void CDlgLearn::on_pbLastQuestion_clicked()
{
CDlgViewQuestion dlg(this);
	dlg.go(m_pCatalog, m_pLastQuestion, m_uLastAnswerMask);
}

void CDlgLearn::on_pbSkip_clicked()
{
	m_uLastAnswerMask=0;
	nextQuestion();
}

void CDlgLearn::on_pbQuit_clicked()
{
	m_pCatalog->updateStatistic();
	accept();
}

void CDlgLearn::on_pbLearnAssistant_clicked()
{
CDlgLearnAssistant dlg(this);
CChapter *pChapter=0;
	if (!dlg.setup(m_pCatalog))
	{
		QMessageBox::information(this, tr("Information"), tr("Derzeit gibt es keine Empfehlung des Lernassistentes."));
		return;
	}
	if (dlg.exec() != QDialog::Accepted) return;
	pChapter = dlg.selectedChapter();
	if (pChapter == 0) return;
	setNewChapter(pChapter);
}

void CDlgLearn::onUpdateDS()
{
	labDSTime->setText(QString("%1 h %2 m").arg(m_ds.timeExpediture()/1000/3600).arg(m_ds.timeExpediture()/1000/60 % 60, 2, 10, QChar('0')));
	labDSCount->setText(QString::number(m_ds.clickedCount()));
	labDSCorrect->setText(QString::number(m_ds.clickedCorrect()));
	labDSWrong->setText(QString::number(m_ds.clickedWrong()));
}
