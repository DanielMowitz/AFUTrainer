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
#include "osziparchive.h"
#include "tools.h"

#include <qdir.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qvariant.h>

CCatalog::~CCatalog()
{  
	qDeleteAll(m_listFiles);
}

void CCatalog::clear()
{
	CChapter::clear();
	m_bMixAnswers = true;
	m_bSort = false;
	m_strFileName.clear();
	m_listHint.clear();
	m_strUniqueName.clear();
	qDeleteAll(m_listFiles);
	m_strPublisher.clear();
	m_strContact.clear();
	m_dateValidFrom = QDate();
	m_dateValidUntil = QDate();
	m_dateCreated = QDate();
	m_datePublished = QDate();
	m_strVersion.clear();
	m_listExam.clear();
	m_listExamStat.clear();
}

bool CCatalog::isEmpty()
{
	return m_strText.isEmpty();
}

bool CCatalog::isValid() const
{
	if (m_dateValidFrom.isValid() && QDate::currentDate() < m_dateValidFrom) return false;
	if (m_dateValidUntil.isValid() && QDate::currentDate() > m_dateValidUntil) return false;
	return true;
}

bool CCatalog::hasHints (const QString& strQuestionId) const
{
	for (int i=0; i<m_listHint.size(); i++)
	{
		if (m_listHint.at(i).hasQuestion(strQuestionId)) return true;
	}
	return false;
}

QString CCatalog::hintText (const QString& strQuestionId) const
{
QString str;
	if (strQuestionId.isEmpty()) return QString();
	for (int i=0; i<m_listHint.size(); i++)
	{
		if (m_listHint.at(i).hasQuestion(strQuestionId)) 
			str += m_listHint[i].showText();
	}
	return str;
}

bool CCatalog::load (const QString& strFileName, QWidget *pParent)
{
QDomDocument doc;
QDomElement elemRoot, e;
QDomNode n;
QString str, strXML;
int iErrLine, iErrCol;
double dVersion=0.0;
CChapter *pChapter=0;

	if (strFileName.right(3).toLower() != "aqz") return false;

	CZipArchive zip;
	if (!zip.open(strFileName, CZipArchive::OpenReadOnly))
	{
		QMessageBox::critical(pParent, pParent->tr("Datei-Fehler"), pParent->tr("Konnte folgende Datei nicht zum Lesen öffnen:\n%1").arg(strFileName));
		return false;
	}

	// Fragen entpacken
	CZipFile *pzfQuestions = zip.findFile("questions.xml");
	if (pzfQuestions == 0)
	{
		QMessageBox::critical(pParent, pParent->tr("Datei-Fehler"), pParent->tr("Konnte in der Datei '%1' keine Fragen finden.").arg(strFileName));
		return false;
	}
	strXML = QString::fromUtf8(pzfQuestions->deflateToByteArray());

	clear();

	// Alle Grafiken in temp. Dateien entpacken
//	pParent->setCursor(Qt::WaitCursor);
	for (int i=0; i<zip.fileCount(); i++)
	{
		CZipFile *pzf = zip.fileAt(i);
		str = pzf->fileName().right(3);
		if (str == "png" || str == "jpg" || str == "gif" || str == "bmp")
		{ // Datei entpacken und Pfade im XML anpassen
			str = QDir::temp().absoluteFilePath(pzf->fileName()+".XXXXXX");
			QTemporaryFile *ptf = new QTemporaryFile(str);
			ptf->open();
			str = ptf->fileName();
			pzf->deflateToFile(*ptf);
			ptf->close();
			m_listFiles.append(ptf);
			strXML.replace(pzf->fileName(), str);
			//qDebug ("Deflating %s to %s", qPrintable(pzf->fileName()), qPrintable(str));
		}
	}
//	pParent->setCursor(Qt::ArrowCursor);

	if (!doc.setContent(strXML, true, &str, &iErrLine, &iErrCol))
	{
		QMessageBox::critical(pParent, pParent->tr("XML-Fehler"), pParent->tr("Fragenkatalog: ") + strFileName + "\n" + str + "\n" + QString (pParent->tr("Zeile: %1 Spalte %2")).arg(iErrLine).arg(iErrCol));
		return false;
	}

	elemRoot = doc.documentElement ();
	if (elemRoot.tagName() != "aqdf")
	{
		QMessageBox::critical(pParent, pParent->tr("Datei-Fehler"), pParent->tr("Unbekanntes XML-Datenformat '%1'.").arg(elemRoot.tagName()));
		return false;
	}
	//m_strText = elemRoot.attribute ("name");
	m_dateCreated = QDate::fromString(elemRoot.attribute("created"), Qt::ISODate);
	dVersion = elemRoot.attribute("version").toDouble();
	if (dVersion != 1.0)
	{
		QMessageBox::information(pParent, pParent->tr("Information"), pParent->tr("Kann die Dateiversion %1 des Fragenkatalogs '%2' nicht lesen.").arg(dVersion).arg(strFileName));
		return false;
	}


	n = elemRoot.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			e = n.toElement ();
			if (e.tagName() == QString ("chapter"))
			{
				pChapter = new CChapter();
				if (pChapter->load (e))
					appendChapter(pChapter);
				else
					delete pChapter;
			}
			else if (e.tagName() == QString ("exam"))
			{
				CExam exam;
				if (exam.load(e)) m_listExam.append(exam);
			}
			else if (e.tagName () == "hint")
			{
				CHint hint;
				if (hint.load (e))
					m_listHint.append (hint);
			}
			else if (e.tagName () == "title")
			{
				m_strText = e.text();
				m_strUniqueName = e.attribute("unique");
			}
			else if (e.tagName () == "comment")
				m_strComment = e.text();
			else if (e.tagName () == "contact")
				m_strContact = e.text();
			else if (e.tagName () == "publisher")
				m_strPublisher = e.text();
			else if (e.tagName() == "valid")
			{
				m_dateValidFrom = QDate::fromString(e.attribute("from"), Qt::ISODate);
				m_dateValidUntil = QDate::fromString(e.attribute("until"), Qt::ISODate);
			}
			else if (e.tagName() == "version")
			{
				m_datePublished = QDate::fromString(e.attribute("published"), Qt::ISODate);
				m_strVersion = e.text();
			}
			else if (e.tagName() == "options")
			{
				m_bMixAnswers = QVariant(e.attribute("mixanswers", "true")).toBool();
				m_bSort = QVariant(e.attribute("sort", "false")).toBool();
			}
		}
		n = n.nextSibling();
	}

	if (m_strUniqueName.isEmpty())
		m_strUniqueName = QDir(strFileName).dirName();

	if (m_bSort) sortAll();

	loadStatistic (pParent);
	updateStatistic();

	m_strFileName = strFileName;
	return true;
}

bool CCatalog::save (const QString& strFileName, QWidget *pParent)
{
QFile file (strFileName);

	if (strFileName.isEmpty()) return false;
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(pParent, QString(pParent->tr("Datei-Fehler")), pParent->tr("Konnte folgende Datei nicht zum Schreiben öffnen:\n")+strFileName);
		return false;
	}
	QTextStream out(&file);

    QDomDocument doc("afutrainer");
    QDomElement elemRoot = doc.createElement("afutrainer");
	//elemRoot.setAttribute("name", name());
	elemRoot.setAttribute("version", 3.0);
	elemRoot.setAttribute("created", QDate::currentDate().toString(Qt::ISODate));
    doc.appendChild(elemRoot);


	// save unique name
	QDomElement elemTitle = createXmlTextElement("title", name(), doc);
	elemTitle.setAttribute("unique", m_strUniqueName);
	elemRoot.appendChild (elemTitle);

	// save comment
	if (!m_strComment.isEmpty())
		elemRoot.appendChild (createXmlTextElement("comment", m_strComment, doc));

	// save contact
	if (!m_strContact.isEmpty())
		elemRoot.appendChild (createXmlTextElement("contact", m_strContact, doc));

	// save publisher
	if (!m_strPublisher.isEmpty())
		elemRoot.appendChild (createXmlTextElement("publisher", m_strPublisher, doc));

	// save version
	QDomElement elemVersion = createXmlTextElement("version", m_strVersion, doc);
	elemVersion.setAttribute("published", m_datePublished.toString(Qt::ISODate));
	elemRoot.appendChild(elemVersion);

	// save dates
	if (m_dateValidFrom.isValid() || m_dateValidUntil.isValid())
	{
		QDomElement elemValid = doc.createElement("valid");
		elemValid.setAttribute("from", m_dateValidFrom.toString(Qt::ISODate));
		elemValid.setAttribute("until", m_dateValidUntil.toString(Qt::ISODate));
		elemRoot.appendChild(elemValid);
	}

	// TODO: save tests


	// save chapters
	for (int i=0; i<m_listChapter.size(); i++)
		m_listChapter[i]->save(elemRoot, doc);

	// save helpers
	for (int i=0; i<m_listHint.size(); i++)
		m_listHint[i].save(elemRoot, doc);

	out << doc.toString();

	m_strFileName = strFileName;
	return true;
}

QString CCatalog::statisticFileName() const
{
QDir dir;
QString str = dir.homePath()+QString("/.afutrainer/") + m_strUniqueName + QString(".stat.xml");
	Q_ASSERT(!m_strUniqueName.isEmpty());
	return (str);
}

bool CCatalog::loadStatistic(QWidget *pParent)
{
QDomDocument doc;
QDomElement elemRoot, e;
QDomNode n;
QFile file (statisticFileName());
QString strVerzeichnis, str, strXML;
int iErrLine, iErrCol;

	if (!file.exists()) return true;
	if (!file.open (QIODevice::ReadOnly))
	{
		QMessageBox::critical(pParent, QString(pParent->tr("Datei-Fehler")), pParent->tr("Konnte folgende Datei nicht zum Lesen öffnen:\n")+statisticFileName());
		return false;
	}
	QTextStream in (&file);
	strXML = in.readAll();

	if (!doc.setContent(strXML, true, &str, &iErrLine, &iErrCol))
	{
		QMessageBox::critical(pParent, pParent->tr("XML-Fehler"), pParent->tr("Statistik zum Fragenkatalog: ") + statisticFileName() + "\n" + str + "\n" + QString (pParent->tr("Zeile: %1 Spalte %2")).arg(iErrLine).arg(iErrCol));
		return false;
	}
	file.close ();

	elemRoot = doc.documentElement ();
	if (doc.doctype().name() != "AFUTrainerStatistics") return false;
	if (elemRoot.tagName() != "statistic") return false;

	n = elemRoot.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			e = n.toElement ();
			if (e.tagName() == QString ("learning"))
			{
				loadLearnStatistic(e);
			}
			else if (e.tagName() == QString("exams"))
				loadExamStatistic(e);
		}
		n = n.nextSibling();
	}

	return true;
}

bool CCatalog::saveStatistic(QWidget *pParent)
{
QDomDocument doc ("AFUTrainerStatistics");
QString strFileName = statisticFileName();
QFile file (strFileName);

	if (m_strUniqueName.isEmpty()) return false;
	QDomElement elemRoot = doc.createElement ("statistic");
	elemRoot.setAttribute ("name", name());
	elemRoot.setAttribute("version", 2);
	elemRoot.setAttribute("date", QDate::currentDate().toString(Qt::ISODate));
	doc.appendChild (elemRoot);

	QDomElement elemLearn = doc.createElement ("learning");
	elemRoot.appendChild (elemLearn);

	saveLearnStatistic(elemLearn, doc);

	QDomElement elemExams = doc.createElement ("exams");
	elemRoot.appendChild (elemExams);
	saveExamStatistic(elemExams, doc);

	if (!file.open (QIODevice::WriteOnly))
	{
		QMessageBox::critical (pParent, pParent->tr("Fehler"), pParent->tr("Konnte folgende Datei nicht zum Schreiben öffnen.\n")+strFileName);
		return false;
	}
	QTextStream out(&file);
	out << doc.toString ();
	return true;
}

bool CCatalog::loadExamStatistic (QDomElement& elemRoot)
{
QDomNode n;
QDomElement e;

	n = elemRoot.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			e = n.toElement ();
			if (e.tagName() == QString ("exam"))
			{
				CExamStat es;
				if (es.load(e)) m_listExamStat.append(es);
			}
		}
		n = n.nextSibling();
	}
	return true;
}

void CCatalog::saveExamStatistic (QDomElement& parent, QDomDocument& doc)
{
	for (int i=0; i<m_listExamStat.size(); i++)
	{
		m_listExamStat.at(i).save (parent, doc);
	}
}


QList<CChapter*> CCatalog::chapters()
{
QList<CChapter*> list;
	list.append(this);
	list << subChapters();
	return list;
}

CExam CCatalog::examById(const QString& strId) const
{
	for (int i=0; i<m_listExam.size(); i++)
	{
		if (m_listExam.at(i).id() == strId)
			return m_listExam.at(i);
	}
	return CExam();
}
