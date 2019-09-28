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

#include "mainwindow.h"

#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qheaderview.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qdir.h>
#include <qsettings.h>

#include "dlglearn.h"
#include "dlglearnassistant.h"
#include "dlginformation.h"
#include "dlgexamselect.h"
#include "dlgexam.h"
#include "dlgexamstatistic.h"
#include "dlglearnstatistic.h"
#include "catalog.h"


CMainWindow::CMainWindow() : QMainWindow()
{
	setupUi(this);
	m_pSplitter = new QSplitter (this);
	setCentralWidget (m_pSplitter);
	m_pViewChapter = new QTreeView (m_pSplitter);
	m_pViewChapter->setModel(&m_modelCatalog);
	m_pViewChapter->header()->setStretchLastSection(false);
	m_pViewChapter->header()->setResizeMode (0, QHeaderView::Stretch);
	m_pViewChapter->header()->setResizeMode (1, QHeaderView::Interactive);
	m_pViewChapter->header()->setResizeMode (3, QHeaderView::Interactive);
	m_pViewChapter->header()->resizeSection(1, 45);
	m_pViewChapter->header()->resizeSection(2, 150);
	m_pViewChapter->header()->resizeSection(3, 20);
	//m_pViewChapter->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect (m_pViewChapter->selectionModel(), SIGNAL(selectionChanged (const QItemSelection& , const QItemSelection& )), this, SLOT(onCatalogSelectionChanged (const QItemSelection& , const QItemSelection& )));
	m_pSplitter2 = new QSplitter (m_pSplitter);
	m_pSplitter2->setOrientation(Qt::Vertical);

	m_pViewQuestions = new QTreeView (m_pSplitter2);
	m_pViewQuestions->setModel(&m_modelQuestion);
	m_pViewQuestions->header()->setStretchLastSection(false);
	m_pViewQuestions->header()->setResizeMode (0, QHeaderView::Interactive);
	m_pViewQuestions->header()->setResizeMode (1, QHeaderView::Stretch);
	m_pViewQuestions->header()->resizeSection(0, 60);
	m_pViewQuestions->header()->resizeSection(2, 20);
	m_pViewQuestions->header()->resizeSection(3, 20);
	m_pViewQuestions->header()->resizeSection(4, 100);
	m_pViewQuestions->header()->resizeSection(5, 80);
	m_pViewQuestions->header()->resizeSection(6, 80);
	m_pViewQuestions->setRootIsDecorated(false);
	connect (m_pViewQuestions->selectionModel(), SIGNAL(selectionChanged (const QItemSelection& , const QItemSelection& )), this, SLOT(onQuestionSelectionChanged (const QItemSelection& , const QItemSelection& )));

	m_pTextQuestion = new QTextBrowser (m_pSplitter2);
	m_pTextQuestion->setOpenExternalLinks(true);
	//m_pTextQuestion->setReadOnly(true);
	//connect (m_pTextQuestion, SIGNAL(anchorClicked (const QUrl &)), this, SLOT(onAnchorClicked (const QUrl &)));

	m_pCatalog = 0;

	connect (&m_rf, SIGNAL(loadFile(const QString&)), this, SLOT(onOpenFile(const QString&)));
	m_rf.create(QString(), 8);
	m_rf.insertToMenu(menuFile, actFileExit);
	m_rf.setShowNoEntry(false);
	m_rf.setShowSeperator(CRecentFiles::SeperatorBottom);

	// open last file
	onOpenFile(m_rf.recentFile(0));

#ifndef _DEBUG
	actFileSave->setVisible(false);
	actFileSaveAs->setVisible(false);
	actFileImport->setVisible(false);
	actFileImportUS->setVisible(false);
#endif

	updateWindowTitle();
}

CMainWindow::~CMainWindow()
{
	if (m_pCatalog) delete m_pCatalog;
}

void CMainWindow::updateWindowTitle()
{
QString str;
	str = tr("AFUTrainer 3.0");
	if (m_pCatalog)
	{
		str += " - ";
		if (m_pCatalog->name().isEmpty())
			str += tr("unbenannt");
		else
			str += m_pCatalog->name();

		if (!m_pCatalog->fileName().isEmpty())
			str += " [" + m_pCatalog->fileName() + "]";
	}
	setWindowTitle(str);
}

void CMainWindow::setCatalog (CCatalog *pCatalog)
{
	m_modelCatalog.setModelData(pCatalog);
	m_pViewChapter->expandAll();
	if (m_pCatalog) delete m_pCatalog;
	m_pCatalog = pCatalog;
	m_modelQuestion.setModelData(0);
	m_pTextQuestion->clear();
	updateWindowTitle();

	if (m_pCatalog == 0) return;

	QDate d = m_pCatalog->validUntil();
	if (d.isValid() && d < QDate::currentDate())
	{
		QString str = tr("<b>Dieser Fragenkatalog ist seit dem %1 nicht mehr gültig!</b>").arg(d.toString(Qt::LocalDate));
		if (!m_pCatalog->publisher().isEmpty())
			str += tr("<p>Weitere Informationen zur Gültigkeit erhalten Sie vom Herausgeber:<br>")+m_pCatalog->publisher();
		if (!m_pCatalog->contact().isEmpty())
			str += tr("<p>Einen aktuellen Fragenkatalog für den AFUTrainer erhalten Sie evt. von:<br>")+m_pCatalog->contact();

		QMessageBox::warning(this, tr("Warnung"), str);
	}
}

bool CMainWindow::checkForErrors()
{
QString str;
	
	if (m_pCatalog == 0) return false;
	str = m_pCatalog->checkForErrors();
	if (str.isEmpty()) return false;
	QMessageBox::critical(this, tr("Fehler im Fragenkatalog"), tr("Die gewünschte Funktion kann nicht ausgeführt werden, da der Fragenkatalog folgende Fehler enthält:<p>")+ str);
	return true;
}

bool CMainWindow::checkForHomeDir()
{
QString strDir = QDir::homePath() + "/.afutrainer/";
QDir dir;
	if (dir.exists(strDir)) return true;
	if (!dir.mkpath(strDir))
	{
		QMessageBox::critical(this, tr("Fehler"), tr("Konnte Verzeichnis %1 nicht anlegen!").arg(strDir));
		return false;
	}
	return true;
}

void CMainWindow::on_actFileNew_triggered()
{
	if (!checkForHomeDir()) return;
	setCatalog(0);
}

void CMainWindow::on_actFileOpen_triggered()
{
QString strFileName;
	if (!checkForHomeDir()) return;
	strFileName = QFileDialog::getOpenFileName(this, tr("Öffne Fragenkatalog"), QString(), tr("Amateur Radio Questionary Data Format (*.aqz)\nAlle Dateien (*.*)"));
	onOpenFile(strFileName);
}

void CMainWindow::onOpenFile(const QString& strFileName)
{
CCatalog *pCatalog=0;
	if (strFileName.isEmpty()) return;
	pCatalog = new CCatalog;
	if (pCatalog->load(strFileName, this))
	{
		setCatalog (pCatalog);
		m_rf.setRecentFile(strFileName);
	}
	else
	{
		m_rf.removeFile(strFileName);
		delete pCatalog;
	}
}

void CMainWindow::on_actFileInformation_triggered()
{
CDlgInformation dlg(this);
	if (!m_pCatalog)
	{
		QMessageBox::information(this, tr("Information"), tr("Kein Fragenkatalog geladen, zu dem Informationen angezeigt werden könnten."));
		return;
	}
	dlg.setup(m_pCatalog);
	dlg.exec();
}

void CMainWindow::on_actFileSave_triggered()
{
	if (!m_pCatalog) return;
	if (m_pCatalog->fileName().isEmpty())
		on_actFileSaveAs_triggered();
	else
	{
		m_pCatalog->save(m_pCatalog->fileName(), this);
		updateWindowTitle();
	}
}

void CMainWindow::on_actFileSaveAs_triggered()
{
QString strFileName;
	if (!m_pCatalog) return;
	strFileName = QFileDialog::getSaveFileName(this, tr("Speichere Fragenkatalog"), QString(), tr("XML-Dateien (*.xml)\nAlle Dateien (*.*)"));
	if (strFileName.isEmpty()) return;
	m_pCatalog->save(strFileName, this);
	updateWindowTitle();
}

bool ImportDE_isChapter(const QString& strLine)
{
	if (strLine.contains(QRegExp ("^[0-9](\\.[0-9]+)*\\s\\w")))
		return true;
	else
		return false;	
}

int ImportDE_chapterLevel(const QString& strLine)
{
QString str = strLine.left(strLine.indexOf(' '));
	return (str.count('.'));
}

bool ImportDE_isQuestion(const QString& strLine)
{
	return strLine.contains(QRegExp("^[A-Z]{2,2}[0-9]{3,3}"));
}

bool ImportDE_isAnswer(const QString& strLine)
{
	return strLine.contains(QRegExp("^[A-D]\\s")) || strLine.contains(QRegExp("^[A-D]$"));
}

void CMainWindow::on_actFileImport_triggered()
{
QString strFileName, strLine, strChapter;
CChapter *pChapter=0, *pTempChapter=0;
CQuestion *pQuestion=0;
int uCurrentLevel=-1, uLevel=-1;
enum LastAdded {None, Chapter, Question, Answer};
LastAdded la=None;
CCatalog *pCatalog=0;

strFileName = QFileDialog::getOpenFileName(this, tr("Öffne Datei zum Fragenkatalog-Import"), QString("D:/projekte/afutrainer/fragenkataloge/2007-02/Klasse A Technik/"), tr("TXT-Dateien (*.txt)\nAlle Dateien (*.*)"));
	if (strFileName.isEmpty()) return;

	QFile file(strFileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(this, tr("Fehler"), tr("Konnte Datei '%1' nicht zum Lesen öffnen!").arg(strFileName));
		return;
	}
	QTextStream in(&file);
	pCatalog = new CCatalog();

	while (!in.atEnd())
	{
		strLine = in.readLine().trimmed();
		if (strLine.isEmpty() || strLine.contains(QRegExp("^Prüfungsfragen")) || strLine.contains("^Bundesnetzagentur"))
		{
			la = None;
		}
		else if (ImportDE_isChapter(strLine)) // && (uCurrentLevel+1 == ImportDE_chapterLevel(strLine) || uCurrentLevel-1 == ImportDE_chapterLevel(strLine)))
		{
			uLevel = ImportDE_chapterLevel(strLine);
			while (uCurrentLevel >= uLevel && pChapter)
			{
				pChapter = pChapter->parentChapter();
				uCurrentLevel--;
			}
			pTempChapter = new CChapter();
			if (pChapter == 0)
				pCatalog->appendChapter(pTempChapter);
			else
				pChapter->appendChapter(pTempChapter);
			pTempChapter->setText(strLine.mid(strLine.indexOf(' ') + 1));

			pChapter = pTempChapter;
			uCurrentLevel++;
			strChapter += QString("%1 -- %2<br>").arg(uLevel).arg(strLine);
			qDebug ("%i -- %s", uLevel, qPrintable(strLine));
			la = Chapter;
		}
		else if (ImportDE_isQuestion(strLine))
		{
			pQuestion = new CQuestion();
			pQuestion->setId (strLine.left(5));
			pQuestion->setText (strLine.mid(6).trimmed());
			if (pChapter)
			{
				pChapter->appendQuestion(pQuestion);

				// update id of chapters
				pTempChapter = pChapter;
				uLevel = uCurrentLevel;
				while (pTempChapter && uLevel >= 0)
				{
					if (pTempChapter->id().isEmpty())
						pTempChapter->setId(pQuestion->id().at(uLevel));
					pTempChapter = pTempChapter->parentChapter();
					uLevel--;
				}
			}
			else 
				delete pQuestion;
			//qDebug (qPrintable(strLine));
			la = Question;
		}
		else if (ImportDE_isAnswer(strLine))
		{
			CAnswer a(strLine.mid(2).trimmed(), strLine[0] == QChar('A'));
			if (pQuestion)
				pQuestion->appendAnswer(a);
			//qDebug (qPrintable("\t"+strLine));
			la = Answer;
		}
		else
		{
			if (la == Chapter && pChapter)
			{
				if (pChapter->text().isEmpty())
					pChapter->appendText (strLine.trimmed());
				else
					pChapter->appendText (" " + strLine.trimmed());
			}
			else if (la == Question && pQuestion)
			{
				if (pQuestion->text().isEmpty() || pQuestion->text().right(1) == "-")
					pQuestion->appendText (strLine.trimmed());
				else
					pQuestion->appendText (" " + strLine.trimmed());
			}
			else if (la == Answer && pQuestion && pQuestion->countAnswer() > 0)
			{
				QString strText = pQuestion->answerAt(pQuestion->countAnswer()-1).text();
				if (strText.isEmpty() || strText.right(1) == "-")
					pQuestion->answerAt(pQuestion->countAnswer()-1).appendText(strLine.trimmed());
				else
					pQuestion->answerAt(pQuestion->countAnswer()-1).appendText(" " + strLine.trimmed());
			}
		}
	}

	// Bilder hinzufügen
	QList<CQuestion*> listPool = pCatalog->questionPool();
	QDir dirPath(strFileName.left(strFileName.lastIndexOf('/')));
	for (int i=0; i<listPool.size(); i++)
	{
		pQuestion = listPool.at(i);
		QStringList strlFiles = dirPath.entryList(QStringList() << pQuestion->id().toLower()+"*");
		for (int j=0; j<strlFiles.size(); j++)
		{
			QString strImgFile = strlFiles.at(j).toLower();
			// Überprüfen, ob Verweis
			if (strImgFile.contains("_"))
			{ // Ja, Verweis
				QString strImgFile1 = strImgFile.left(strImgFile.indexOf('_'));
				QString strImgFile2 = strImgFile.mid(strImgFile.indexOf('_') + 1);
				QStringList strlFiles2 = dirPath.entryList(QStringList() << strImgFile2 + "*");
				if (strlFiles2.size() == 0)
				{
					qDebug ("Verlinkte Datei %s von Frage %s existiert nicht. Link: %s", qPrintable(strImgFile2), qPrintable(strImgFile1), qPrintable(strImgFile));
				}
				else
				{
					qDebug ("Datei %s verlinkt auf reale Datei %s", qPrintable(strImgFile), qPrintable(strlFiles2.at(0)));
					strImgFile = strlFiles2.at(0);
				}

				/*int idx = strlFiles.indexOf(QRegExp(strImgFile2 + "\\..*"));
				if (idx < 0)
				{
					qDebug ("Verlinkte Datei %s von Frage %s existiert nicht. Link: %s", qPrintable(strImgFile2), qPrintable(strImgFile1), qPrintable(strImgFile));
				}
				else
				{
					qDebug ("Datei %s verlinkt auf reale Datei %s", qPrintable(strImgFile), qPrintable(strlFiles.at(idx)));
					strImgFile = strlFiles.at(idx);
				}*/

			}

			QString strLink = "<img src='" + strImgFile + "'>";

			if (strImgFile.contains(QRegExp("f\\d*\\.")))
			{ // Frage
				qDebug("Adding image %s to question %s", qPrintable(strImgFile), qPrintable(pQuestion->id()));
				pQuestion->appendText("<p>" + strLink + "</p>");
			}
			else if (strImgFile.contains("a.") && pQuestion->countAnswer() > 0)
			{ // Antwort A
				//qDebug("Adding image %s to answer A of question %s", qPrintable(strImgFile), qPrintable(pQuestion->id()));
				pQuestion->answerAt(0).appendText("<p>" + strLink + "</p>");
			}
			else if (strImgFile.contains("b.") && pQuestion->countAnswer() > 1)
			{ // Antwort B
				//qDebug("Adding image %s to answer B of question %s", qPrintable(strImgFile), qPrintable(pQuestion->id()));
				pQuestion->answerAt(1).appendText("<p>" + strLink + "</p>");
			}
			else if (strImgFile.contains("c.") && pQuestion->countAnswer() > 2)
			{ // Antwort C
				//qDebug("Adding image %s to answer C of question %s", qPrintable(strImgFile), qPrintable(pQuestion->id()));
				pQuestion->answerAt(2).appendText("<p>" + strLink + "</p>");
			}
			else if (strImgFile.contains("d.") && pQuestion->countAnswer() > 3)
			{ // Antwort D
				//qDebug("Adding image %s to answer D of question %s", qPrintable(strImgFile), qPrintable(pQuestion->id()));
				pQuestion->answerAt(3).appendText("<p>" + strLink + "</p>");
			}
			else
			{
				qDebug ("Konnte Grafik %s nicht zuordnen.\n\tMögliche Gründe: Frage oder Antwort existiert nicht. Dateiname der Grafik falsch geschrieben.", qPrintable(strImgFile));
			}
		}
	}

	pCatalog->setName("Importierter Katalog von " + strFileName);
	QMessageBox::information(this, tr("Information"), 
		"Bitte Debug-Ausgabe überprüfen, ob alle Grafik-Dateien zugeordnet wurden!<p>"
		"Bitte überprüfen, ob alle Kapitel korrekt erkannt wurden. "
		"Wenn nicht, muss die TXT-Datei von Hand kontrolliert werden!<hr>"+strChapter);

	qDebug("Weitere Vorgehensweise:\n\t1. Fragenkatalog als XML-Datei abspeichern.\n\t2. ggf. Korrekturen in der XML-Datei vornehmen\n\t3. XML-Datei und alle Grafik-Dateien in ein ZIP-Archiv packen und die Dateiänderung auf .atc ändern.\n\tFertig!");
	setCatalog (pCatalog);
}

bool ImportUS_isChapter(const QString& strLine)
{
	if (strLine.contains(QRegExp("^SUBELEMENT")) || strLine.contains(QRegExp("^[A-Z][0-9][A-Z]\\s")))
		return true;
	else
		return false;
}

bool ImportUS_isQuestion(const QString& strLine)
{
	return strLine.contains(QRegExp("^[A-Z][0-9][A-Z][0-9]{2,2}"));
}

bool ImportUS_isAnswer(const QString& strLine)
{
	return strLine.contains(QRegExp("^[A-D]\\.\\s"));
}

void CMainWindow::on_actFileImportUS_triggered()
{
QString strFileName, strLine, strChapter, str;
CChapter *pChapter=0, *pTempChapter=0;
CQuestion *pQuestion=0;
int uCurrentLevel=-1, uLevel=-1;
enum LastAdded {None, Chapter, Question, Answer};
LastAdded la=None;
CCatalog *pCatalog=0;

strFileName = QFileDialog::getOpenFileName(this, tr("Öffne Datei zum Fragenkatalog-Import"), QString("D:/projekte/afutrainer/fragenkataloge/US/"), tr("TXT-Dateien (*.txt)\nAlle Dateien (*.*)"));
	if (strFileName.isEmpty()) return;

	QFile file(strFileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(this, tr("Fehler"), tr("Konnte Datei '%1' nicht zum Lesen öffnen!").arg(strFileName));
		return;
	}
	QTextStream in(&file);
	pCatalog = new CCatalog();

	while (!in.atEnd())
	{
		strLine = in.readLine().trimmed();
		if (strLine.isEmpty())
		{
			//la = None;
		}
		else if (strLine == "~~")
		{
			la = None;
		}
		else if (ImportUS_isChapter(strLine))
		{
			pTempChapter = new CChapter();
			if (strLine.contains(QRegExp("^SUBELEMENT")))
			{
				uLevel = 0;
				strLine = strLine.mid(strLine.indexOf(' ')+1);
				pTempChapter->setId(strLine.left(2));
//				str = strLine.mid(strLine.indexOf(QRegExp("[A-Z]"), 2));
//				str.remove(QRegExp("\\[.*\\]"));
				pTempChapter->setText(strLine.mid(strLine.indexOf(QRegExp("[A-Z]"), 2)));
			}
			else
			{
				uLevel = 1;
				pTempChapter->setId(strLine.mid(2, 1));
//				str = strLine.mid(strLine.indexOf(QRegExp("[A-Z]"), 3));
//				str.remove(QRegExp("\\[.*\\]"));
				pTempChapter->setText(strLine.mid(strLine.indexOf(QRegExp("[A-Z]"), 3)));
			}
			while (uCurrentLevel >= uLevel && pChapter)
			{
				pChapter = pChapter->parentChapter();
				uCurrentLevel--;
			}
			if (!pChapter)
				pCatalog->appendChapter(pTempChapter);
			else
			{
//				pTempChapter->setExam("main", 1);
				pChapter->appendChapter(pTempChapter);
			}

			strChapter += QString("%1 -- %2 - %3<br>").arg(uLevel).arg(pTempChapter->id(), pTempChapter->text());
			pChapter = pTempChapter;
			uCurrentLevel++;
			la = Chapter;
		}
		else if (ImportUS_isQuestion(strLine))
		{
			pQuestion = new CQuestion();
			pQuestion->setId (strLine.left(5));
			pQuestion->setText (strLine.mid(6).trimmed());
			if (pChapter)
				pChapter->appendQuestion(pQuestion);
			else 
				delete pQuestion;
			//qDebug (qPrintable(strLine));
			la = Question;
		}
		else if (ImportUS_isAnswer(strLine))
		{
			CAnswer a(strLine.mid(3).trimmed(), false);
			if (pQuestion)
				pQuestion->appendAnswer(a);
			//qDebug (qPrintable("\t"+strLine));
			la = Answer;
		}
		else
		{
			if (la == Chapter && pChapter)
			{
				if (pChapter->text().isEmpty())
					pChapter->appendText (strLine.trimmed());
				else
					pChapter->appendText (" " + strLine.trimmed());
			}
			else if (la == Question && pQuestion)
			{
				if (pQuestion->text().isEmpty() || pQuestion->text().right(1) == "-")
					pQuestion->appendText (strLine.trimmed());
				else
					pQuestion->appendText (" " + strLine.trimmed());
			}
			else if (la == Answer && pQuestion && pQuestion->countAnswer() > 0)
			{
				QString strText = pQuestion->answerAt(pQuestion->countAnswer()-1).text();
				if (strText.isEmpty() || strText.right(1) == "-")
					pQuestion->answerAt(pQuestion->countAnswer()-1).appendText(strLine.trimmed());
				else
					pQuestion->answerAt(pQuestion->countAnswer()-1).appendText(" " + strLine.trimmed());
			}
		}
	}

	// Fragen durchgehen und richtige Antworten eintragen
	QList<CQuestion*> listPool = pCatalog->questionPool();
	for (int i=0; i<listPool.size(); i++)
	{
		pQuestion = listPool.at(i);
		QString strText = pQuestion->text();
		int idx = strText.indexOf(QRegExp("\\([A-Z]\\)"));
		if (idx == -1) continue;
		int iCorrect = pQuestion->text().at(idx+1).toAscii() - 'A';
		if (pQuestion->countAnswer() > iCorrect)
			pQuestion->answerAt(iCorrect).setCorrect(true);
		strText.remove(idx, 3);
		idx = strText.indexOf(QRegExp("\\[.*\\]"));
		if (idx >= 0)
		{
			int iLen = strText.indexOf(']', idx) - idx + 1;
			CHint hint;
			hint.appendQuestion(pQuestion->id());
			hint.setAuthor("FCC");
			hint.setText("See FCC rules part " + strText.mid(idx + 1,  iLen - 2));
			pCatalog->appendHint(hint);
			strText.remove(idx, iLen);
		}
		pQuestion->setText(strText.trimmed());
	}

	// Kapitelnamen korrigieren
	QList<CChapter*> listChapter = pCatalog->subChapters();
	for (int i=0; i<listChapter.size(); i++)
	{
		pChapter = listChapter.at(i);
		QString strText = pChapter->text();
		//strText = strText.left(strText.indexOf(" - "));
		strText.remove(QRegExp("\\[.*\\]"));
		pChapter->setText(strText.trimmed());
	}

	// Allgemeine Angaben:
	//pCatalog->setVersionText("");
	pCatalog->setPublisher("NCVEC (National Conference of Volunteer Examiner Coordinators)<br>Question Pool Committee<br>http://www.ncvec.org/");
	pCatalog->setContact("Oliver Saal, DM1OLI<br>http://www.oliver-saal.de/software/afutrainer/<br>Mail: osaal@gmx.de");

	pCatalog->setName("Importierter Katalog von " + strFileName);
	QMessageBox::information(this, tr("Information"), 
		"Bitte überprüfen, ob alle Kapitel korrekt erkannt wurden. "
		"Wenn nicht, muss die TXT-Datei von Hand kontrolliert werden!<hr>"+strChapter);

	qDebug("Weitere Vorgehensweise:\n\t1. Fragenkatalog als XML-Datei abspeichern.\n\t2. ggf. Korrekturen in der XML-Datei vornehmen\n\t3. XML-Datei und alle Grafik-Dateien in ein ZIP-Archiv packen und die Dateiänderung auf .atc ändern.\n\tFertig!");
	setCatalog (pCatalog);
}

void CMainWindow::on_actFileExit_triggered()
{
	qApp->quit();
}

void CMainWindow::on_actQuestionAssistant_triggered()
{
CDlgLearnAssistant dlg(this);
CDlgLearn dlgLearn(this);
CChapter *pChapter=0;

	if (m_pCatalog == 0) return;
	if (checkForErrors()) return;
	if (!dlg.setup(m_pCatalog))
	{
		QMessageBox::information(this, tr("Information"), tr("Derzeit gibt es keine Empfehlung des Lernassistentes."));
		return;
	}
	if (dlg.exec() != QDialog::Accepted) return;
	pChapter = dlg.selectedChapter();
	if (pChapter == 0) return;

	dlgLearn.go(m_pCatalog, pChapter);
	m_pCatalog->saveStatistic(this);
}

void CMainWindow::on_actQuestionsLearn_triggered()
{
CDlgLearn dlg(this);
QModelIndexList list = m_pViewChapter->selectionModel()->selectedIndexes();

	if (checkForErrors()) return;
	if (list.isEmpty())
	{
		QMessageBox::information(this, tr("Information"), tr("Bitte ein Kapitel zum Lernen auswählen!"));
		return;
	}
	CChapter *p = (CChapter*)list.first().internalPointer();
	Q_ASSERT(p != 0);
	dlg.go(m_pCatalog, p);
	m_pCatalog->saveStatistic(this);
}

void CMainWindow::on_actQuestionsLearnStatistics_triggered()
{
CDlgLearnStatistic dlg(this);
QModelIndexList list = m_pViewChapter->selectionModel()->selectedIndexes();
CChapter *p=0;

	if (checkForErrors()) return;
	if (list.isEmpty())
		p = m_pCatalog;
	else
		p = (CChapter*)list.first().internalPointer();

	dlg.go(p);
}

void CMainWindow::on_actQuestionsTest_triggered()
{
int iCount=0;
CExam exam;

	if (checkForErrors()) return;
	iCount = m_pCatalog->countExam();
	if (iCount == 0)
	{
		QMessageBox::information(this, tr("Information"), tr("Dieser Fragenkatalog enthält keine Prüfungen."));
		return;
	}
	else if (iCount == 1)
	{
		exam = m_pCatalog->examAt(0);
	}
	else if (iCount > 1)
	{
		CDlgExamSelect dlg1(this);
		dlg1.setup(m_pCatalog);
		dlg1.exec();
		if (dlg1.result() == QDialog::Rejected) return;
		exam = m_pCatalog->examAt(dlg1.selectedExam());
	}

	CDlgExam dlg2(m_pCatalog, this);
	if (dlg2.setup(exam))
		dlg2.exec();
}

void CMainWindow::on_actQuestionsTestStatistics_triggered()
{
CDlgExamStatistic dlg(this);
	if (checkForErrors()) return;
	if (m_pCatalog->countExam() == 0)
	{
		QMessageBox::information(this, tr("Information"), tr("Dieser Fragenkatalog enthält keine Prüfungen."));
		return;
	}
	dlg.go(m_pCatalog);
}

void CMainWindow::on_actViewToolbar_toggled(bool bChecked)
{
	if (bChecked)
		toolBar->show();
	else
		toolBar->hide();
}

void CMainWindow::on_actViewStatusbar_toggled(bool bChecked)
{
	if (bChecked)
		statusbar->show();
	else
		statusbar->hide();
}

void CMainWindow::on_actHelpWhatsThis_triggered()
{
	QWhatsThis::enterWhatsThisMode();
}

void CMainWindow::on_actHelpAbout_triggered()
{
QString str = tr("<b>AFUTrainer Version 3.0</b><br>"
				 "(c) 2003-2007 by Oliver Saal (DM1OLI)<br><hr>"
				 "EMail: osaal@gmx.de<br>"
				 "http://www.oliver-saal.de/software/afutrainer/<hr>"
				 "This program is free software; you can redistribute it and/or modify "
				 "it under the terms of the GNU General Public License as published by "
				 "the Free Software Foundation; either version 2 of the License, or "
				 "(at your option) any later version.<p>"
				 "This program is distributed in the hope that it will be useful, "
				 "but WITHOUT ANY WARRANTY; without even the implied warranty of "
				 "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
				 "GNU General Public License for more details.<p>"
				 "You should have received a copy of the GNU General Public License "
				 "along with this program; if not, write to the "
				 "Free Software Foundation, Inc., "
				 "59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.");
	QMessageBox::about(this,tr("Über..."), str);
}

void CMainWindow::on_actHelpAboutQt_triggered()
{
	qApp->aboutQt();
}

void CMainWindow::onCatalogSelectionChanged (const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected);
	m_pTextQuestion->clear();
	if (!selected.indexes().isEmpty())
	{
		CChapter *pChapter = (CChapter*)selected.indexes().first().internalPointer();
		if (pChapter)
			m_modelQuestion.setModelData(pChapter);
	}
	else
		m_modelQuestion.clear();
}

void CMainWindow::onQuestionSelectionChanged (const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected);
	m_pTextQuestion->clear();
	if (selected.indexes().isEmpty()) return;
	CQuestion *pQuestion = (CQuestion*)selected.indexes().first().internalPointer();
	if (pQuestion == 0) return;
	m_pTextQuestion->setHtml(pQuestion->showText(m_pCatalog));
}
/*
void CMainWindow::onAnchorClicked (const QUrl &link)
{
	QMessageBox::information(this, tr("Click"), link.toString());
	m_pTextQuestion->home();
}
*/
