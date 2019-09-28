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

#include "plotwidget.h"
#include <qpainter.h>

void CPlotWidgetPoint::clear()
{
}


void CPlotWidgetCurve::clear()
{
	QList<CPlotWidgetPoint>::clear();
}


QRectF CPlotWidgetCurve::boundaries() const
{
QRectF rect;

	for (int i=0; i<size(); i++)
	{
		QPointF p = at(i);
		if (i == 0)
		{
			rect.setBottomLeft(p);
			rect.setTopRight(p);
		}
		else
		{
			if (p.x() < rect.left())
				rect.setLeft(p.x());
			else if (p.x() > rect.right())
				rect.setRight(p.x());

			if (p.y() > rect.bottom())
				rect.setBottom(p.y());
			else if (p.y() < rect.top())
				rect.setTop(p.y());
		}
	}
	return rect;
}

CPlotWidgetTic::CPlotWidgetTic(const double dPos, const QString& strText)
{
	clear();
	m_dPos = dPos;
	m_strText = strText;
}

CPlotWidgetTic::CPlotWidgetTic(const double dPos, const double dWidth, const QString& strText)
{
	clear();
	m_dPos = dPos;
	m_dWidth = dWidth;
	m_strText = strText;
}

CPlotWidgetTic::CPlotWidgetTic(const double dPos, const QPixmap& pixmap)
{
	clear();
	m_dPos = dPos;
	m_pixmap = pixmap;
}

void CPlotWidgetTic::clear()
{
	m_dPos = 0.0;
	m_dWidth = 0.0;
	m_pen = QPen(Qt::DashLine);
	m_pen.setColor(Qt::darkGray);
	m_penFont = QPen(Qt::black);
	m_pixmap = QPixmap();
	m_strText.clear();
	m_fillType = FillNone;
	m_lineType = LinePlot;
	m_iTextFlags = Qt::AlignHCenter | Qt::AlignTop;
}

void CPlotWidgetTic::paintX (QPainter *pPainter, CPlotWidget *pWidget) const
{
QPoint ptTL, ptBR; // TopLeft, BottomRight
QRect rectText;

	if (m_fillType != FillNone && m_dWidth > 0.0)
	{
		ptTL = pWidget->mapToPlot(QPointF(m_dPos, pWidget->m_rectData.top()));
		ptBR = pWidget->mapToPlot(QPointF(m_dPos+m_dWidth, pWidget->m_rectData.bottom()));

		if (m_fillType == FillAll)
		{
			ptTL.setY(pWidget->m_rectPlot.top());
			ptBR.setY(pWidget->m_rectPlot.bottom());
		}
		
		if (ptTL.x() < pWidget->m_rectPlot.left())
			ptTL.setX(pWidget->m_rectPlot.left());

		if (ptBR.x() > pWidget->m_rectPlot.right())
			ptBR.setX(pWidget->m_rectPlot.right());

		pPainter->fillRect(QRect(ptTL, ptBR), m_brush);

		rectText = QRect(
			QPoint(ptTL.x(), pWidget->m_rectPlot.bottom() + 5), 
			QPoint(ptBR.x(), pWidget->rect().bottom()));
	}
	else
	{
		rectText = QRect();
	}

	if (m_lineType != LineNone)
	{
		pPainter->setPen(m_pen);
		pPainter->drawLine(
			pWidget->mapToPlot(QPointF(m_dPos, pWidget->m_rectData.top())), 
			pWidget->mapToPlot(QPointF(m_dPos, pWidget->m_rectData.bottom())));
	}

	if (!m_strText.isEmpty())
	{
		pPainter->setPen(m_penFont);
		pPainter->drawText(rectText, m_iTextFlags, m_strText);
	}
}

void CPlotWidgetTic::paintY (QPainter *pPainter, CPlotWidget *pWidget) const
{
QPoint ptTL, ptBR; // TopLeft, BottomRight
QRect rectText;

	if (m_fillType != FillNone && m_dWidth > 0.0)
	{
		ptTL = pWidget->mapToPlot(QPointF(pWidget->m_rectData.left(), m_dPos));
		ptBR = pWidget->mapToPlot(QPointF(pWidget->m_rectData.right(), m_dPos+m_dWidth));

		if (m_fillType == FillAll)
		{
			ptTL.setX(pWidget->m_rectPlot.left());
			ptBR.setX(pWidget->m_rectPlot.right());
		}
		
		if (ptTL.y() < pWidget->m_rectPlot.top())
			ptTL.setY(pWidget->m_rectPlot.top());

		if (ptBR.y() > pWidget->m_rectPlot.bottom())
			ptBR.setY(pWidget->m_rectPlot.bottom());

		pPainter->fillRect(QRect(ptTL, ptBR), m_brush);

		rectText = QRect(
			QPoint(3, ptTL.y()), 
			QPoint(pWidget->m_rectPlot.left()-3, ptBR.y()));
	}
	else
	{
		QFontMetrics fm(m_font);
		QPoint pt = pWidget->mapToPlot(QPointF(pWidget->m_rectData.left(), m_dPos));
		rectText = QRect(
			QPoint(3, pt.y()-fm.height()), 
			QPoint(pWidget->m_rectPlot.left()-3, pt.y()+fm.height()));
	}

	if (m_lineType != LineNone)
	{
		pPainter->setPen(m_pen);
		pPainter->drawLine(
			pWidget->mapToPlot(QPointF(pWidget->m_rectData.left(), m_dPos)), 
			pWidget->mapToPlot(QPointF(pWidget->m_rectData.right(), m_dPos)));
	}

	if (!m_pixmap.isNull())
	{
		// Ziel-Mittelpunkt errechnen
		QPoint pt = pWidget->mapToPlot(QPointF(0, m_dPos));
		pt.setX(pWidget->m_rectPlot.left() - m_pixmap.width()/2 - 5);

		// Linke obere Ecke errechnen und malen!
		pt -= QPoint (m_pixmap.width()/2, m_pixmap.height()/2);
		pPainter->drawPixmap(pt, m_pixmap);
	}

	if (!m_strText.isEmpty())
	{
		pPainter->setPen(m_penFont);
		pPainter->drawText(rectText, m_iTextFlags, m_strText);
	}
}

CPlotWidget::CPlotWidget(QWidget *pParent) : QFrame(pParent)
{
	clear();
}

void CPlotWidget::clear()
{
	m_type = PlotLines;
	m_bLimitAutoY = true;
	m_bLimitAutoX = true;
	m_dLimitXMin = 0.0;
	m_dLimitXMax = 10.0;
	m_dLimitYMin = 0.0;
	m_dLimitYMax = 10.0;
	m_dLimitXRound = 0.0;
	m_dLimitYRound = 0.0;
	m_dTicX = 0.0;
	m_penTicX = QPen(Qt::DashLine);
	m_penTicX.setColor(Qt::darkGray);
	m_dTicY = 0.0;
	m_penTicY = QPen(Qt::DashLine);
	m_penTicY.setColor(Qt::darkGray);
	m_iBorder = BorderLeft|BorderBottom;
	m_iBorderDistTop = 5;
	m_iBorderDistBottom = 5;
	m_iBorderDistLeft = 5;
	m_iBorderDistRight = 5;
	m_brushPlotBkg = QBrush(Qt::white);
	m_listTicX.clear();
	m_listTicY.clear();
	m_dBarWidth = 0.6;
	m_dBarOffset = 0.0;
	m_listCurves.clear();
}

void CPlotWidget::setBorderDistance(const int iLeft, const int iRight, const int iTop, const int iBottom)
{
	m_iBorderDistTop = iTop;
	m_iBorderDistBottom = iBottom;
	m_iBorderDistLeft = iLeft;
	m_iBorderDistRight = iRight;
}

void CPlotWidget::updateCache()
{
	m_rectPlot = plotArea();
	m_rectData = rectData();
}

QRect CPlotWidget::plotArea() const
{
QRect rect = frameRect();
	rect.adjust(m_iBorderDistLeft, m_iBorderDistTop, -m_iBorderDistRight, -m_iBorderDistBottom);
	return rect;
}

QRectF CPlotWidget::rectData() const
{
QRectF rect;
	if (m_bLimitAutoX || m_bLimitAutoY)
	{
		for (int i=0; i<m_listCurves.size(); i++)
		{
			rect = rect.united(m_listCurves.at(i).boundaries());
		}
	}
	if (!m_bLimitAutoX)
	{
		rect.setLeft(m_dLimitXMin);
		rect.setRight(m_dLimitXMax);
	}
	else if (m_dLimitXRound > 0.0)
	{
		rect.setLeft((unsigned)(rect.left() / m_dLimitXRound) * m_dLimitXRound);
		rect.setRight((unsigned)(rect.right() / m_dLimitXRound + 1) * m_dLimitXRound);
	}

	if (!m_bLimitAutoY)
	{
		rect.setTop(m_dLimitYMin);
		rect.setBottom(m_dLimitYMax);
	}
	else if (m_dLimitYRound > 0.0)
	{
		rect.setTop((unsigned)(rect.top() / m_dLimitYRound) * m_dLimitYRound);
		rect.setBottom((unsigned)(rect.bottom() / m_dLimitYRound + 1) * m_dLimitYRound);
	}
	return rect;
}

QPoint CPlotWidget::mapToPlot (QPointF p)
{
QPointF ret;
double m;

	// Umrechung X-Koordinate
	m = ((double) (m_rectPlot.left() - m_rectPlot.right())) / (m_rectData.left() - m_rectData.right());
	ret.setX(m * (p.x() - m_rectData.left()) + m_rectPlot.left());

	// Umrechnung Y-Koordinate (mit Spiegelung)
	m = ((double) (m_rectPlot.bottom() - m_rectPlot.top())) / (m_rectData.top() - m_rectData.bottom());
	ret.setY(m * (p.y() - m_rectData.top()) + m_rectPlot.bottom());

	return ret.toPoint();
}

void CPlotWidget::paintEvent (QPaintEvent *e)
{
QList<QPoint> listPoints;
QList<CPlotWidgetTic> listTicsX, listTicsY;
double d=0.0;
int i=0;
	
	QFrame::paintEvent(e);
	QPainter painter(this);

	updateCache();

	// Draw Background
	if (m_brushPlotBkg.style() != Qt::NoBrush)
	{
		painter.fillRect(m_rectPlot, m_brushPlotBkg);
	}

	// Draw Tics
	listTicsX = m_listTicX;
	if (listTicsX.isEmpty() && m_dTicX > 0.0)
	{
		d = ((unsigned)(m_rectData.left() / m_dTicX)) * m_dTicX;
		if (d < m_rectData.left()) d+=m_dTicX;
		while (d <= m_rectData.right())
		{
			CPlotWidgetTic tic(d, QString("%1").arg(d,0,'f',2));
			tic.setPen(m_penTicX);
			listTicsX.append(tic);
			d += m_dTicX;
		}
	}
	listTicsY = m_listTicY;
	if (listTicsY.isEmpty() && m_dTicY > 0.0)
	{
		d = ((unsigned)(m_rectData.top() / m_dTicY)) * m_dTicY;
		if (d < m_rectData.top()) d+=m_dTicY;
		while (d <= m_rectData.bottom())
		{
			CPlotWidgetTic tic(d, QString("%1").arg(d,0,'f',2));
			tic.setPen(m_penTicY);
			listTicsY.append(tic);
			d += m_dTicY;
		}
	}
	for (i=0; i<listTicsX.size(); i++)
		listTicsX.at(i).paintX(&painter, this);

	for (i=0; i<listTicsY.size(); i++)
		listTicsY.at(i).paintY(&painter, this);

	// Draw Curves
	if (m_type == PlotLines)
	{
		for (int i=0; i<m_listCurves.size(); i++)
		{
			CPlotWidgetCurve c = m_listCurves.at(i);
			for (int j=0; j<c.size(); j++)
			{
				CPlotWidgetPoint p = c.at(j);
				listPoints.append(mapToPlot (p));
			}
			painter.setPen(c.pen());
			painter.drawPolyline(listPoints.toVector());
		}
	}
	else if (m_type == PlotBarsSum && m_listCurves.size() != 0)
	{
		CPlotWidgetCurve c1 = m_listCurves.at(0);
		//painter.setPen(c1.pen());
		//painter.setBrush(c1.brush());
		QPoint ptTL, ptBR;
		for (int j=0; j<c1.size(); j++)
		{	// Alle Punkte der ersten Kurve durchgehen
			CPlotWidgetPoint p1 = c1.at(j);
			ptTL = mapToPlot(QPointF(p1.x() + m_dBarOffset - m_dBarWidth/2, p1.y()));
			ptBR = mapToPlot(QPointF(p1.x() + m_dBarOffset + m_dBarWidth/2, m_rectData.top()));
			if (p1.y() != 0.0 && m_rectPlot.contains(ptTL) && m_rectPlot.contains(ptBR)) 
				painter.fillRect(QRect(ptTL, ptBR), c1.brush());
			double dStart = p1.y();
			for (int i=1; i<m_listCurves.size(); i++)
			{	// alle Weiteren Kurven durchgehen
				CPlotWidgetCurve c2 = m_listCurves.at(i);
				for (int jj=0; jj<c2.size(); jj++)
				{	// alle Punkte der zweiten Kurve durchgehen
					CPlotWidgetPoint p2 = c2.at(jj);
					if (p2.x() != p1.x()) continue;
					ptTL = mapToPlot(QPointF(p2.x() + m_dBarOffset - m_dBarWidth/2, dStart+p2.y()));
					ptBR = mapToPlot(QPointF(p2.x() + m_dBarOffset + m_dBarWidth/2, dStart+m_rectData.top()));
					if (p2.y() != 0 && m_rectPlot.contains(ptTL) && m_rectPlot.contains(ptBR)) 
					{
						dStart += p2.y();
						painter.fillRect(QRect(ptTL, ptBR), c2.brush());
					}
				}
			}
		}
	}

	// Draw border
	painter.setPen(m_penBorder);
	if (m_iBorder & BorderLeft)
		painter.drawLine(m_rectPlot.topLeft(), m_rectPlot.bottomLeft());
	if (m_iBorder & BorderRight)
		painter.drawLine(m_rectPlot.topRight(), m_rectPlot.bottomRight());
	if (m_iBorder & BorderTop)
		painter.drawLine(m_rectPlot.topRight(), m_rectPlot.topLeft());
	if (m_iBorder & BorderBottom)
		painter.drawLine(m_rectPlot.bottomRight(), m_rectPlot.bottomLeft());
}
