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

#include "exam.h"

#include "ui_dlgexam.h"
#include <qdialog.h>
#include <qtimer.h>
#include <qdatetime.h>

class CCatalog;
class CQuestion;

class CDlgExam : public QDialog, Ui::DlgExam
{
Q_OBJECT
public:
	CDlgExam (CCatalog *pCatalog, QWidget *pParent=0);
	~CDlgExam ();

	bool setup (const CExam& exam);

protected:
	void init();
	void updateNavi();
	void updateProgressQuestion();
	void updateProgressTimer();
	void showQuestion();
	void onQuestionChanged();
	unsigned answeredQuestionCount();
	void saveCurrentAnswer();

protected slots:
	void onTimer();
	void on_pbFirst_clicked();
	void on_pbPrev_clicked();
	void on_pbNext_clicked();
	void on_pbLast_clicked();
	void on_spinBox_valueChanged(int i);
	void on_pbFinish_clicked();

	inline void on_rbA_toggled(bool bChecked)		{ Q_UNUSED(bChecked); saveCurrentAnswer(); updateProgressQuestion(); }
	inline void on_rbB_toggled(bool bChecked)		{ Q_UNUSED(bChecked); saveCurrentAnswer(); updateProgressQuestion(); }
	inline void on_rbC_toggled(bool bChecked)		{ Q_UNUSED(bChecked); saveCurrentAnswer(); updateProgressQuestion(); }
	inline void on_rbD_toggled(bool bChecked)		{ Q_UNUSED(bChecked); saveCurrentAnswer(); updateProgressQuestion(); }
	inline void on_rbNoIdea_toggled(bool bChecked)		{ Q_UNUSED(bChecked); saveCurrentAnswer(); updateProgressQuestion(); }

protected:
	CCatalog *m_pCatalog;
	CExam m_exam;

	QList<CQuestion*> m_listQuestion;
	QList<unsigned> m_listAnswerMask;
	QTimer m_timer;
	QDateTime m_dtStart;
	QDateTime m_dtStop;
	int m_iCurrentQuestion;
	bool m_bIsFinished;
	bool m_bTimeout;
};

