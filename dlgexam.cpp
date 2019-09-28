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

#include "dlgexam.h"
#include "question.h"
#include "error.h"
#include "catalog.h"

#include <qmessagebox.h>

CDlgExam::CDlgExam (CCatalog *pCatalog, QWidget *pParent) : QDialog (pParent, Qt::WindowMaximizeButtonHint)
{
	m_pCatalog = pCatalog;
	m_iCurrentQuestion = -1;
	m_bIsFinished = false;
	m_bTimeout = false;
	setupUi(this);
	connect (&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

CDlgExam::~CDlgExam ()
{
}

bool CDlgExam::setup (const CExam& exam)
{
	m_exam = exam;
	labHeader->setText(exam.name());

	try
	{
		m_listQuestion = exam.createQuestionPool(m_pCatalog->questionPool());
	}
	catch (CError e)
	{
		QMessageBox::critical(this, tr("Fehler"), e.toHtml());
		return false;
	}

	m_listAnswerMask.clear();
	while (m_listAnswerMask.size() < m_listQuestion.size())
		m_listAnswerMask.append(0);

	// Antworten bei Bedarf durchmischen
	if (m_pCatalog->useMixedAnswers())
	{
		for (int i=0; i<m_listQuestion.size(); i++)
			m_listQuestion.at(i)->mixAnswers();
	}
	init();
	return true;
}

void CDlgExam::init()
{
	// Zeitbalken & Zeitüberwachung
	m_timer.setInterval(1000);
	m_timer.start();
	m_dtStart = QDateTime::currentDateTime();
	pgTime->setMaximum (m_exam.duration() * 60);

	// Sonstiges
	pgQuestions->setMaximum (m_listQuestion.size());
	m_bIsFinished = false;
	m_bTimeout = false;
	labMaxWrong->setText(QString("%1").arg(m_exam.maxErrorPoints()));

	// Navigation
	spinBox->setRange (1, m_listQuestion.size());
	spinBox->setValue(1);
	m_iCurrentQuestion = 1;

	// Alles updaten
	updateNavi();
	showQuestion();
	updateProgressTimer();
	updateProgressQuestion();
}

void CDlgExam::onTimer()
{
	updateProgressTimer();
}

void CDlgExam::updateNavi()
{
	pbFirst->setEnabled (spinBox->value() > spinBox->minimum());
	pbPrev->setEnabled (spinBox->value() > spinBox->minimum());
	pbNext->setEnabled (spinBox->value() < spinBox->maximum());
	pbLast->setEnabled (spinBox->value() < spinBox->maximum());
}

unsigned CDlgExam::answeredQuestionCount()
{
unsigned u=0;
	for (int i=0; i<m_listAnswerMask.size(); i++)
	{
		if (m_listAnswerMask.at(i) != 0) u++;
	}
	return u;
}

void CDlgExam::updateProgressQuestion()
{
unsigned uAnswered = answeredQuestionCount();
	labProgressQuestions->setText(tr("%1 von %2").arg(uAnswered).arg(m_listAnswerMask.size()));
	pgQuestions->setValue(uAnswered);
}

void CDlgExam::updateProgressTimer()
{
QDateTime dt = QDateTime::currentDateTime();
unsigned uSecs = m_dtStart.secsTo(dt);
	
	labProgessTime->setText(tr("Abgelaufene Zeit: %1:%2 min von %3:00 min")
		.arg(uSecs / 60)
		.arg(uSecs % 60, 2, 10, QChar('0'))
		.arg(m_exam.duration()));

	pgTime->setValue (uSecs);

	if (uSecs > m_exam.duration()*60)
	{
		m_bTimeout = true;
		m_timer.stop();
		QMessageBox::information(this, tr("Information"), tr("Die Zeit ist abgelaufen.\nDie Prüfung wird deswegen beendet."));
		on_pbFinish_clicked();
	}
}

void CDlgExam::on_pbFirst_clicked()
{
	spinBox->setValue(1);
}

void CDlgExam::on_pbPrev_clicked()
{
int iNew = spinBox->value() - 1;
	if (iNew < 1) iNew = 1;
	spinBox->setValue(iNew);
}

void CDlgExam::on_pbNext_clicked()
{
int iNew = spinBox->value() + 1;
	if (iNew > m_listQuestion.size()) iNew = m_listQuestion.size();
	spinBox->setValue(iNew);
}

void CDlgExam::on_pbLast_clicked()
{
	spinBox->setValue(m_listQuestion.size());
}

void CDlgExam::on_spinBox_valueChanged(int i)
{
	Q_UNUSED(i);
	onQuestionChanged();
}

void CDlgExam::saveCurrentAnswer()
{
unsigned uAnswerMask=0;
	if (m_iCurrentQuestion >= 0)
	{ // Antwort zur aktuellen Frage abspeichern
		if (rbA->isChecked()) uAnswerMask |= 1;
		if (rbB->isChecked()) uAnswerMask |= 2;
		if (rbC->isChecked()) uAnswerMask |= 4;
		if (rbD->isChecked()) uAnswerMask |= 8;
		m_listAnswerMask[m_iCurrentQuestion] = uAnswerMask;
	}
}

void CDlgExam::onQuestionChanged()
{
	saveCurrentAnswer();
	updateNavi();
	updateProgressQuestion();
	showQuestion();					// neue Frage anzeigen
}

void CDlgExam::showQuestion()
{
CQuestion *q=0;
unsigned uAnswerMask;
QString str;

	m_iCurrentQuestion = spinBox->value()-1;
	q = m_listQuestion.at(m_iCurrentQuestion);
	uAnswerMask = m_listAnswerMask.at(m_iCurrentQuestion);
	str = q->learnText(m_pCatalog, m_bIsFinished, false);
	if (m_bIsFinished)
	{
		str += "<hr><p><b>";
		if (!q->isCorrectAnswer(uAnswerMask))
			str += "<font color='red'>";
		else
			str += "<font color='green'>";

		str += q->correctionText(uAnswerMask) + "</font></p>";
	}
	textBrowser->setHtml(str);

	rbA->setChecked(uAnswerMask & 1);
	rbB->setChecked(uAnswerMask & 2);
	rbC->setChecked(uAnswerMask & 4);
	rbD->setChecked(uAnswerMask & 8);
	rbNoIdea->setChecked(uAnswerMask == 0);
}

void CDlgExam::on_pbFinish_clicked()
{
unsigned uCorrect=0, uWrong=0, uAnsweredCount = answeredQuestionCount(), uErrorPoints=0;
bool bSaveStat=true, bPassed=false;
QDateTime dt = QDateTime::currentDateTime();
unsigned uSecs = m_dtStart.secsTo(dt);

	saveCurrentAnswer();
	if (uAnsweredCount < (unsigned)m_listQuestion.size() && !m_bTimeout)
	{
		if (QMessageBox::question (this, 
			tr("Sicherheitsabfrage"), 
			tr("Sie haben noch nicht alle Fragen beantwortet.\nWirklich abgeben?"), 
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return;
	}

	m_timer.stop();
	pbFinish->setEnabled(false);
	rbA->setEnabled(false);
	rbB->setEnabled(false);
	rbC->setEnabled(false);
	rbD->setEnabled(false);
	rbNoIdea->setEnabled(false);
	pbCancel->setText(tr("Beenden"));
	m_bIsFinished = true;
	showQuestion();

	for (int i=0; i<m_listQuestion.size(); i++)
	{
		CQuestion *q = m_listQuestion.at(i);
		unsigned uAnswerMask = m_listAnswerMask.at(i);
		if (q->isCorrectAnswer(uAnswerMask))
			uCorrect++;
		else
		{
			uWrong++;
			uErrorPoints+=q->errorPoints();
		}
	}
	labCorrect->setText(QString("%1").arg(uCorrect));
	labWrong->setText(QString("%1").arg(uWrong));

	if (uAnsweredCount < (unsigned)m_listQuestion.size()/2)
	{
		bPassed = false;
		bSaveStat = false;
		labResult->setText(tr("Ohne Wertung\n(Nicht bestanden)"));
		QMessageBox::information(this, tr("Information"), tr("Sie haben weniger als die Hälfte aller Fragen beantwortet.\nDiese Prüfung wird deswegen nicht gewertet."));
	}
	else if (uErrorPoints > m_exam.maxErrorPoints())
	{
		bPassed = false;
		labResult->setText(tr("<font color='red'>Nicht bestanden</font>"));
		QMessageBox::information(this, tr("Ergebnis"), tr("Sie haben die Prüfung leider nicht bestanden."));
	}
	else
	{
		bPassed = true;
		labResult->setText(tr("<font color='green'>Bestanden</font>"));
		QMessageBox::information(this, tr("Ergebnis"), tr("<b>Herzlichen Glückwunsch!</b><p>Sie haben die Prüfung bestanden!"));
	}

	if (bSaveStat)
	{ // Prüfungsstatistik speichern
		CExamStat es(m_exam);
		es.setSecs(uSecs);
		es.setQuestions(m_listQuestion, m_listAnswerMask);
		es.setResult(uCorrect, uWrong, uErrorPoints, bPassed);
		m_pCatalog->appendExamStat(es);
		m_pCatalog->saveStatistic(this);
	}
}
