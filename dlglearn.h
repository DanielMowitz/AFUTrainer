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

#ifndef DLGLEARN_H
#define DLGLEARN_H

#include <qdialog.h>
#include <qlist.h>
#include <qdatetime.h>
#include "question.h"

#include "ui_dlglearn.h"

class CChapter;
class CCatalog;

class CDlgLearn : public QDialog, Ui::DlgLearn
{
	Q_OBJECT
public:
	CDlgLearn (QWidget *pParent=0);
	~CDlgLearn();

	void go (CCatalog *pCatalog, CChapter *pChapter);

protected:
	void setNewChapter(CChapter *pChapter);
	void updateStatistic();
	void updateLearnAssistant();
	void nextQuestion();
	CQuestion *findNextQuestion();
	CQuestion *findNextPoolQuestion();
	CQuestion *findNextTargetQuestion();

	void handleAnswer(const int i);

protected slots:
	void on_pbAnswerA_clicked();
	void on_pbAnswerB_clicked();
	void on_pbAnswerC_clicked();
	void on_pbAnswerD_clicked();
	void on_pbShowHelper_clicked();
	void on_pbSkip_clicked();
	void on_pbLastQuestion_clicked();
	void on_pbQuit_clicked();
	void on_pbLearnAssistant_clicked();
	void onUpdateDS();

protected:
	CCatalog *m_pCatalog;
	CChapter *m_pChapter;
	QList<CQuestion*> m_listQuestion;
	CQuestion *m_pQuestion;
	CQuestion *m_pLastQuestion;
	unsigned m_uLastAnswerMask;
	bool m_bHintsUsed;
	QTime m_timeElapsed;
	unsigned m_uElapsedBeforeBreak;	// if the user makes a break
	bool m_bMsgBoxTargetReachedShowed;
	CDayStatistic m_ds;

	bool m_bAssistantEnable;
	bool m_bCheatEnable;
};

#endif
