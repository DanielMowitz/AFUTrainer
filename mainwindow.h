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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qsplitter.h>
#include <qtreeview.h>
#include <qtextbrowser.h>

#include "catalogmodel.h"
#include "questionmodel.h"
#include "recentfiles.h"

#include "ui_mainwindow.h"

class CMainWindow : public QMainWindow, protected Ui::MainWindow
{
	Q_OBJECT
public:
	CMainWindow();
	~CMainWindow();

protected:
	void setCatalog (CCatalog *pCatalog);
	void updateWindowTitle();
	bool checkForErrors();
	bool checkForHomeDir();

protected slots:
	void on_actFileNew_triggered();
	void on_actFileOpen_triggered();
	void on_actFileInformation_triggered();
	void on_actFileSave_triggered();
	void on_actFileSaveAs_triggered();
	void on_actFileImport_triggered();
	void on_actFileImportUS_triggered();
	void on_actFileExit_triggered();
	void on_actQuestionAssistant_triggered();
	void on_actQuestionsLearn_triggered();
	void on_actQuestionsLearnStatistics_triggered();
	void on_actQuestionsTest_triggered();
	void on_actQuestionsTestStatistics_triggered();
	void on_actViewToolbar_toggled(bool bChecked);
	void on_actViewStatusbar_toggled(bool bChecked);
	void on_actHelpWhatsThis_triggered();
	void on_actHelpAbout_triggered();
	void on_actHelpAboutQt_triggered();

	void onCatalogSelectionChanged (const QItemSelection& selected, const QItemSelection& deselected);
	void onQuestionSelectionChanged (const QItemSelection& selected, const QItemSelection& deselected);

	void onOpenFile(const QString& strFileName);

//	void onAnchorClicked (const QUrl &link );

protected:
	CRecentFiles m_rf;
	QSplitter *m_pSplitter;				// splitter widget
	QTreeView *m_pViewChapter;			// listview chapters
	QTreeView *m_pViewQuestions;		// listview questions
	QSplitter *m_pSplitter2;
	QTextBrowser *m_pTextQuestion;

	CCatalog *m_pCatalog;
	CCatalogModel m_modelCatalog;
	CQuestionModel m_modelQuestion;
};


#endif // MAINWINDOW_H

