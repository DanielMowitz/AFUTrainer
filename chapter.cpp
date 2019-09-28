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
#include <qcoreapplication.h>


QString CChapter::tr (const char *sourceText, const char *comment)
{
	return QCoreApplication::translate("CChapter", sourceText, comment);
}

void CChapter::clear()
{
	m_pParentChapter=0;
	m_strId.clear();
	m_strText.clear();
	m_strComment.clear();
	m_recom = RecommendationNone;
	m_recom2 = RecommendationNone;
	qDeleteAll(m_listChapter);
	qDeleteAll(m_listQuestion);
	m_bHasLearningNew = false;
	m_bHasKnownQuestions = false;
	m_bHasKnownQuestionsRepeatToday = false;
	m_uNeverAskedCount = 0;
	for (int i=0; i<=LEVEL_MAX; i++) m_uLevelCount[i] = 0;
	for (int i=0; i<=RecommendationMax; i++) m_uRecomCount[i] = 0;
//	m_mapExam.clear();
}

int CChapter::countSubQuestion() const
{
int i=0, iRet=0;
	for (i=0; i<m_listChapter.size(); i++)
		iRet += m_listChapter.at(i)->countSubQuestion();
	return (iRet + m_listQuestion.size());
}

bool CChapter::load (QDomElement elem)
{
CChapter *pChapter=0;

	if (elem.tagName () != QString ("chapter")) return false;
	if (!elem.hasAttribute ("name")) return false;
//	if (pParent != NULL && !elem.hasAttribute ("id")) return false;
	m_strId = elem.attribute ("id");
	m_strText = elem.attribute ("name");
	QDomNode n = elem.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			QDomElement e = n.toElement ();
			if (e.tagName() == "comment")
				m_strComment = e.text();
			else if (e.tagName() == QString ("chapter"))
			{
				pChapter = new CChapter();
				if (pChapter->load (e))
					appendChapter(pChapter);
				else
					delete pChapter;
			}
			else if (e.tagName() == QString ("question"))
			{
				CQuestion *pQuestion = new CQuestion();
				if (pQuestion->load (e))
					appendQuestion(pQuestion);
				else
					delete pQuestion;
			}
		}
		n = n.nextSibling();
	}
	updateStatistic();
	return true;
}

void CChapter::save (QDomElement& parent, QDomDocument& doc)
{
QDomElement elem = doc.createElement("chapter");

	elem.setAttribute("name", text());
	elem.setAttribute("id", id());

	if (!m_strComment.isEmpty())
	{
		QDomElement elemComment = doc.createElement("comment");
		QDomText textComment = doc.createTextNode(text());
		elemComment.appendChild(textComment);
		elem.appendChild(elemComment);
	}

	parent.appendChild(elem);

	// save exams
/*	QStringList strl = m_mapExam.keys();
	for (int i=0; i<strl.size(); i++)
	{
		QDomElement e = doc.createElement("exam");
		e.setAttribute("id", strl.at(i));
		e.setAttribute("questions", QString("%1").arg(m_mapExam[strl.at(i)]));
		elem.appendChild(e);
	}
*/
	// save chapters
	for (int i=0; i<m_listChapter.size(); i++)
	{
		m_listChapter[i]->save(elem, doc);
	}

	// save questions
	for (int i=0; i<m_listQuestion.size(); i++)
	{
		m_listQuestion[i]->save(elem, doc);
	}
}

bool CChapter::loadLearnStatistic (QDomElement elem)
{
QList<CQuestion*> listQuestionPool = questionPool();

	if (elem.tagName () != QString ("learning")) return false;

	QDomNode n = elem.firstChild();
	while (!n.isNull())
	{
		if (n.isElement ())
		{
			QDomElement e = n.toElement ();
			if (e.tagName() == QString ("question"))
			{
				QString strId = e.attribute("id");
				for (int i=0; i<listQuestionPool.size(); i++)
				{
					if (listQuestionPool.at(i)->id() == strId)
						listQuestionPool[i]->loadLearnStatistic(e);
				}
			}
		}
		n = n.nextSibling();
	}
	updateRecommendation();
	return true;
}

bool CChapter::saveLearnStatistic (QDomElement& parent, QDomDocument& doc)
{
	// questions from sub-chapters
	for (int i=0; i<m_listChapter.size(); i++)
	{
		m_listChapter[i]->saveLearnStatistic(parent, doc);
	}

	// save questions
	for (int i=0; i<m_listQuestion.size(); i++)
	{
		m_listQuestion[i]->saveLearnStatistic(parent, doc);
	}
	return true;
}

QString CChapter::checkForErrors() const
{
QString str;

	// check chapters
	for (int i=0; i<m_listChapter.size(); i++)
	{
		str += m_listChapter[i]->checkForErrors();
	}

	// check questions
	for (int i=0; i<m_listQuestion.size(); i++)
	{
		str += m_listQuestion[i]->checkForErrors();
	}
	return str;
}

QList<CChapter*> CChapter::subChapters() const
{
QList<CChapter*> list;

	for (int i=0; i<m_listChapter.size(); i++)
	{
		list << m_listChapter.at(i);
		list << m_listChapter[i]->subChapters();
	}
	return list;
}

QList<CQuestion*> CChapter::questionPool() const
{
QList<CQuestion*> list;
	for (int i=0; i<m_listChapter.size(); i++)
	{
		list << m_listChapter[i]->questionPool();
	}
	list << m_listQuestion;
	return list;
}

QList<CQuestion*> CChapter::questionPoolLevel(const unsigned uLevel) const
{
QList<CQuestion*> list;
int i;
	for (i=0; i<m_listChapter.size(); i++)
		list << m_listChapter[i]->questionPoolLevel(uLevel);
	for (i=0; i<m_listQuestion.size(); i++)
	{
		if (m_listQuestion.at(i)->level() == uLevel)
			list.append (m_listQuestion.at(i));
	}
	return list;
}

QList<CQuestion*> CChapter::questionPoolDeepen() const
{
QList<CQuestion*> list;
int i;
	for (i=0; i<m_listChapter.size(); i++)
		list << m_listChapter[i]->questionPoolDeepen();
	for (i=0; i<m_listQuestion.size(); i++)
	{
		CQuestion *q = m_listQuestion.at(i);
		if (q->level() == LEVEL_VERYOFTEN && !q->isNeverAsked())
			list.append (q);
	}
	return list;
}

QList<CQuestion*> CChapter::questionPoolRepeat(const QDate d) const
{
QList<CQuestion*> list;
int i;
	for (i=0; i<m_listChapter.size(); i++)
		list << m_listChapter[i]->questionPoolRepeat(d);
	for (i=0; i<m_listQuestion.size(); i++)
	{
		CQuestion *q = m_listQuestion.at(i);
		if (q->repeatDate() <= d)
			list.append (q);
	}
	return list;
}


void CChapter::updateStatisticCount()
{
/*		for (int i=0; i<m_listQuestion.size(); i++)
		if (m_listQuestion.at(i)->isLearningNew()) return true;

	for (int i=0; i<m_listChapter.size(); i++)
		if (m_listChapter.at(i)->isLearningNew()) return true;

	return false;
*/
	// Alle Statistiken zurücksetzen
	m_uNeverAskedCount = 0;
	m_bHasLearningNew = false;
	m_bHasKnownQuestions = false;
	m_bHasKnownQuestionsRepeatToday = false;
	for (int i=0; i<=LEVEL_MAX; i++) m_uLevelCount[i] = 0;

	// Zuerst Statistiken für Unterkapitel berechnen und integrieren
	for (int i=0; i<m_listChapter.size(); i++)
	{
		CChapter *p = m_listChapter.at(i);
		p->updateStatisticCount();
		for (int j=0; j<=LEVEL_MAX; j++)
			m_uLevelCount[j] += p->m_uLevelCount[j];
		m_uNeverAskedCount += p->m_uNeverAskedCount;
		if (p->hasLearningNewQuestions()) m_bHasLearningNew = true;
		if (p->hasKnownQuestions()) m_bHasKnownQuestions = true;
		if (p->hasKnownQuestionsRepeatToday()) m_bHasKnownQuestionsRepeatToday = true;
	}

	// Anschließend Statistiken für eigene Fragen neu berechnen und hinzufügen
	for (int i=0; i<m_listQuestion.size(); i++)
	{
		CQuestion *q = m_listQuestion.at(i);
		if (q->isNeverAsked()) m_uNeverAskedCount++;
		m_uLevelCount[q->level()]++;
		if (q->isLearningNew()) m_bHasLearningNew = true;
		if (q->isKnownQuestion()) m_bHasKnownQuestions = true;
		if (q->isKnownQuestion() && q->isRepeatToday()) m_bHasKnownQuestionsRepeatToday = true;
	}
}

void CChapter::updateRecommendationStatistic()
{
	for (int i=0; i<RecommendationMax; i++) m_uRecomCount[i] = 0;
	m_uRecomCount[recommendation()]=1;

	for (int i=0; i<m_listChapter.size(); i++)
	{
		CChapter *p = m_listChapter.at(i);
		p->updateRecommendationStatistic();
		for (int j=0; j<RecommendationMax; j++)
			m_uRecomCount[j] += p->m_uRecomCount[j];
	}
}

/*
Berechnet die Statistik für das Kapitel inkl. aller Unterkapitel neu.
*/
void CChapter::updateStatistic()
{
	updateStatisticCount();
	updateRecommendation();
	updateRecommendationStatistic();
}

/*
CRecommendation CChapter::recommendation() const
{
CRecommendation r;
	r.create(this);
	return r;
}
*/
/*
CRecommendationStatistic CChapter::recommendationStatistic() const
{
CRecommendationStatistic rs;

	for (int i=0; i<m_listChapter.size(); i++)
	{
		rs.append(m_listChapter.at(i)->recommendationStatistic());
	}
	//TODO
//	rs.append(recommendation());
	return rs;
}
*/

QString CChapter::idWithParents() const
{
CChapter *pParent=m_pParentChapter;
QString str;
	str = id();
	while (pParent != 0)
	{
		str = pParent->id() + str;
		pParent = pParent->m_pParentChapter;
	}

	return str;
}

/*!
Das empfohlene Wiederholdatum eines Kapitels das frühestete Datum, 
an dem eine Frage des Kapitels oder Unterkapitels wiederholt werden sollte. 
\return Datum, wann das Kapitel wiederholt werden soll. 
Gibt es kein solches Datum, so wird ein ungültiges Datum (QDate::isValid()) zurückgegeben.
Hinweis: Das Datum kann auch in der Vergangenheit liegen!
*/
QDate CChapter::repeatDate() const
{
QDate d;
QList<CQuestion*> list = questionPool();

	for (int i=0; i<list.size(); i++)
	{
		QDate dq = list.at(i)->repeatDate();
		if (!dq.isValid()) continue;
		if (!d.isValid() || dq < d) d = dq;
	}
	return d;
}

/*!	
Die Variable m_recomRepeatDate muss up to date sein!

\return Lernempfehlung dieses Kapitels ohne Berücksichtigung evt. vorhandener ünter- oder übergeordneter Kapitel.

\sa m_recomRepeatDate	
*/
CChapter::Recommendation CChapter::recommendationIndividual() const
{
	if (!m_recomRepeatDate.isValid())
		return RecommendationLearnNew;
	else if (m_recomRepeatDate <= QDate::currentDate())
	{
		if (hasLearningNewQuestions() && !hasKnownQuestionsRepeatToday())
			return RecommendationLearnNew;
		else
			return RecommendationRepeatToday;
	}
	else
	{
		if (countNeverAsked() > 0)
			return RecommendationLearnNew;
		else
			return RecommendationRepeatLater;
	}
}

/*!	
Aktualisiert die Lernempfehlung für dieses Kapitel und alle Unterkapitel
\sa m_recom	
*/
void CChapter::updateRecommendation()
{
	m_recom = RecommendationNone;
	m_recom2 = RecommendationNone;

	m_recomRepeatDate = repeatDate(); /*QDate();*/
	if (m_listChapter.isEmpty() && m_listQuestion.isEmpty()) return;

	if (m_pParentChapter && m_pParentChapter->recommendation() != RecommendationSubChapter)
		m_recom = RecommendationParentChapter;
	else
	{
		if (countSubQuestion() > 20)
		{ // Nur wenn in den Unterkapiteln zusammen mehr als x Fragen sind, zuerst die Unterkapitel einzeln lernen lassen
			for (int i=0; i<m_listChapter.size(); i++)
			{
				CChapter *p = m_listChapter.at(i);
				if (p->levelAvg() < LEVEL_NORMAL || p->countNeverAsked() > 0)
					m_recom = RecommendationSubChapter;
			}
		}

		if (m_recom == RecommendationNone)
		{
			//m_recomRepeatDate = repeatDate();
			m_recom = recommendationIndividual();
		}
	}

	// Alternative Empfehlung
	if (m_recom == RecommendationParentChapter || m_recom == RecommendationSubChapter)
	{
		m_recom2 = recommendationIndividual();
		if (m_recom2 != RecommendationLearnNew && m_recom2 != RecommendationRepeatToday)
			m_recom2 = RecommendationNone;
	}

	// find recommended questions
	m_listQuestionRecommended.clear();
	QList<CQuestion*> list = questionPool();
	for (int i=0; i<list.size(); i++)
	{
		CQuestion *q = list.at(i);
		if (m_recom == RecommendationLearnNew || m_recom2 == RecommendationLearnNew)
		{
			if (q->isNeverAsked() || q->isLearningNew())
				m_listQuestionRecommended.append(q);
		}
		else if (m_recom == RecommendationRepeatToday || m_recom2 == RecommendationRepeatToday)
		{
			if (q->isRepeatToday())
				m_listQuestionRecommended.append(q);
		}
	}

	// update child chapters
	for (int i=0; i<m_listChapter.size(); i++)
	{
		CChapter *p = m_listChapter.at(i);
		p->updateRecommendation();
		//m_listQuestionRecommended << p->m_listQuestionRecommended;
	}
}
/*
bool CChapter::hasRecommendedQuestions() const
{
	return (m_recom == RecommendationLearnNew || m_recom == RecommendationRepeatToday);
}

QList<CQuestion*> CChapter::recommendedQuestions() const
{
QList<CQuestion*> listRet, list = questionPool();

	if (m_recom != RecommendationLearnNew && m_recom != RecommendationRepeatToday) return listRet;

	for (int i=0; i<list.size(); i++)
	{
		CQuestion *q = list.at(i);
		switch (m_recom)
		{
		case RecommendationLearnNew:
			if (q->isNeverAsked() || q->isLearningNew())
				listRet.append(q);
			break;
		case RecommendationRepeatToday:
			if (q->isRepeatToday())
				listRet.append(q);
			break;
		}
	}
	return listRet;
}
*/

QString CChapter::recommendationText(const Recommendation r, const QDate dRepeat)
{
unsigned uDays=0;
	switch (r)
	{
	default:
	case RecommendationNone:
		return tr("Keine Lernempfehlung");
	case RecommendationSubChapter:
		return tr("Zuerst Unterkapitel lernen");
	case RecommendationParentChapter:
		return tr("Übergeordnetes Kapitel lernen");
	case RecommendationLearnNew:
		return tr("Neue Fragen lernen");
	case RecommendationRepeatToday:
		return tr("Heute wiederholen");
	case RecommendationRepeatLater:
		uDays = QDate::currentDate().daysTo(dRepeat);
		if (uDays == 1)
			return tr("Morgen wiederholen");
		else
			return tr("In %1 Tagen wiederholen").arg(uDays);
	}
	return QString();
}

QString CChapter::recommendationText() const
{
	return recommendationText(m_recom, m_recomRepeatDate);
}

QString CChapter::recommendationToolTip() const
{
QString str = recommendationText();
unsigned uCount = recommendedQuestionCount();
	if (hasRecommendedQuestions() && m_recom2 == RecommendationNone)
	{
		str += " ";
		if (uCount == 1)
			str += tr("(1 Frage)");
		else
			str += tr("(%1 Fragen)").arg(uCount);
	}
	if (m_recom2 != RecommendationNone)
	{
		str += "\n" + tr("Alternativ: ");
		if (m_recom2 == RecommendationLearnNew)
		{
			if (uCount == 1)
				str += tr("1 neue Frage lernen");
			else
				str += tr("%1 neue Fragen lernen").arg(uCount);
		}
		else if (m_recom2 == RecommendationRepeatToday)
		{
			if (uCount == 1)
				str += tr("1 Frage wiederholen");
			else
				str += tr("%1 Fragen wiederholen").arg(uCount);
		}
		else
			str += recommendationText (m_recom2, m_recomRepeatDate);
	}
	return str;
}

QString CChapter::recommendationTextExtended(const CCatalog *pCatalog) const
{
unsigned uDays=0;
QString str;

	switch (m_recom)
	{
	default:
	case RecommendationNone:
		str = tr("Keine Lernempfehlung");
		break;
	case RecommendationSubChapter:
		str = tr("Dieses Kapitel enthält Unterkapitel, dessen Fragen Sie noch nicht ausreichend gelernt haben.\nEs wird empohlen in kleinen Etappen zu lernen und damit zuerst die Unterkapitel zu vertiefen.");
		break;
	case RecommendationParentChapter:
		if (levelAvgRounded() >= LEVEL_NORMAL)
			str = tr("Sie können die Fragen dieses Kapitels gut beantworten.\n");
		str += tr("Es wird empfohlen, alle Fragen des übergeordneten Kapitels gemischt zusammen zu lernen.");
		break;
	case RecommendationLearnNew:
		if (!isRecommendedNow(pCatalog))
			str = tr("Es gibt andere Kapitel, deren Fragen heute wiederholt werden sollten. Bitte lernen Sie diese Kapitel zuerst.");
		else
			str = tr("Bitte beantworten Sie alle neuen Fragen mindestens einmal richtig.");
		break;
	case RecommendationRepeatToday:
		str = tr("Bitte lernen Sie alle heute zu wiederholenden Fragen, bis sie eine Lernfortschritts-Stufe höher eingestuft sind.");
		break;
	case RecommendationRepeatLater:
		uDays = QDate::currentDate().daysTo(m_recomRepeatDate);
		if (uDays > 1)
			str = tr("Die Wiederholung dieses Kapitels ist erst in %1 Tagen geplant.\n").arg(uDays);
		else
			str = tr("Die Wiederholung dieses Kapitels ist erst für morgen geplant.\n");
		if (pCatalog->m_uRecomCount[RecommendationRepeatToday] > 0)
			str += tr("Es gibt andere Kapitel, deren Fragen heute wiederholt werden müssen. Bitte lernen Sie diese Kapitel zuerst.");
		else if (pCatalog->m_uRecomCount[RecommendationLearnNew] > 0)
			str += tr("Bitte lernen Sie zuerst Kapitel mit neuen Fragen.");
		break;
	}

	if (hasRecommendedQuestions() && isRecommendedNow(pCatalog))
		str += tr("<p>Dafür sind noch %1 Fragen zu lernen.").arg(recommendedQuestionCount());

	return str;
}

QString CChapter::recommendationTextExtended2(const CCatalog *pCatalog) const
{
QString str;

	if (m_recom2 == RecommendationLearnNew || (m_recom == RecommendationLearnNew && !isRecommendedNow(pCatalog)))
	{
		//str = tr("Bitte beantworten Sie alle neuen Fragen mindestens einmal richtig.");
		str = tr("Alternativ können Sie jetzt die neuen Fragen dieses Kapitels lernen (%1 Fragen).").arg(recommendedQuestionCount());
	}
	else if (m_recom2 == RecommendationRepeatToday)
	{
		if (m_recom == RecommendationSubChapter)
			str = tr("Bitte lernen Sie alle heute zu wiederholenden Fragen, bis sie eine Lernfortschritts-Stufe höher eingestuft sind (%1 Fragen).").arg(recommendedQuestionCount());
		else
			str = tr("Alternativ können Sie jetzt die heute zu wiederholenden Fragen dieses Kapitels lernen (%1 Fragen).").arg(recommendedQuestionCount());
	}

	return str;
}

QString CChapter::recommendationIconName(const Recommendation r, const CCatalog *pCatalog)
{
	switch (r)
	{
	case RecommendationSubChapter:
		return QString(":/icons/16x16/button_cancel.png");
	case RecommendationParentChapter:
		return QString(":/icons/16x16/button_ok.png");
	case RecommendationLearnNew:
		if (pCatalog->m_uRecomCount[RecommendationRepeatToday] > 0)
			return QString(":/icons/16x16/idea_gray.png");
		else
			return QString(":/icons/16x16/idea.png");
	case RecommendationRepeatToday:
		return QString(":/icons/16x16/idea.png");
	case RecommendationRepeatLater:
		return QString(":/icons/16x16/idea_gray.png");
	default:
		return QString();
	}
}

QString CChapter::recommendationIconName(const CCatalog *pCatalog) const
{
	return recommendationIconName(m_recom, pCatalog);
}

/*!
\return true: Kapitel kann jetzt gelernt werden, 
false: Kapitel sollte überhaupt nicht oder erst später gelernt werden
*/

bool CChapter::isRecommendedNow(const CCatalog *pCatalog) const
{
	switch (m_recom)
	{
	case RecommendationSubChapter:
		if (pCatalog->m_uRecomCount[RecommendationRepeatToday] > 0)
			return true;
		break;
	case RecommendationRepeatToday:
		return true;
	case RecommendationLearnNew:
		if (pCatalog->m_uRecomCount[RecommendationRepeatToday] == 0)
			return true;
		break;
	default:
		break;
	}
	return false;
}

/*
\return true: Das Kapitel enthält noch neue Fragen, die gerade (="heute") gelernt werden
bool CChapter::isLearningNew() const
{
	for (int i=0; i<m_listQuestion.size(); i++)
		if (m_listQuestion.at(i)->isLearningNew()) return true;

	for (int i=0; i<m_listChapter.size(); i++)
		if (m_listChapter.at(i)->isLearningNew()) return true;

	return false;
}
*/

CDayStatistic CChapter::dayStatistic (const QDate& date) const
{
CDayStatistic dsRet, ds;
QList<CQuestion*> listPool = questionPool();

	for (int i=0; i<listPool.size(); i++)
	{
		ds = listPool.at(i)->dayStatistic(date);
		dsRet += ds;
	}
	if (listPool.size() != 0) dsRet.m_dLevel /= listPool.size();
	return dsRet;
}

CDayStatistic CChapter::completeStatistic() const
{
	return dayStatistic(QDate());
}

QDateTime CChapter::firstAnswerClicked() const
{
QList<CQuestion*> listPool = questionPool();
QDateTime dtRet, dt;

	for (int i=0; i<listPool.size(); i++)
	{
		dt = listPool.at(i)->firstClicked();
		if (dt.isNull()) continue;
		if (dtRet.isNull() || dt < dtRet) dtRet = dt;
	}
	return dtRet;
}

double CChapter::levelAvg() const
{
double d=0.0;
double dCount=0.0;

	for (int i=0; i<=LEVEL_MAX; i++)
	{
		d += m_uLevelCount[i] * i;
		dCount += m_uLevelCount[i];
	}
	if (dCount != 0.0) d /= dCount;
	return d;
}

unsigned CChapter::levelAvgRounded() const
{
	return ((unsigned) (levelAvg()+0.5));
}

QString CChapter::levelAvgText() const
{
	return QString("%1").arg(CQuestion::levelText(levelAvgRounded()));
}

QIcon CChapter::levelAvgIcon() const
{
	return QIcon(CQuestion::levelIconName(levelAvgRounded()));
}

QPixmap CChapter::levelAvgPixmap() const
{
	return QPixmap(CQuestion::levelIconName(levelAvgRounded()));
}

static bool chapterLessThan(const CChapter *c1, const CChapter *c2)
{
     return c1->id() < c2->id();
}

void CChapter::sortSubChapters(bool bSortQuestions)
{
	if (bSortQuestions) sortQuestions();
	qSort (m_listChapter.begin(), m_listChapter.end(), chapterLessThan);

	for (int i=0; i<m_listChapter.size(); i++)
	{
		m_listChapter.at(i)->sortSubChapters(bSortQuestions);
	}
}

static bool questionLessThan(const CQuestion *q1, const CQuestion *q2)
{
     return q1->id() < q2->id();
}

void CChapter::sortQuestions()
{
	qSort (m_listQuestion.begin(), m_listQuestion.end(), questionLessThan);
}

