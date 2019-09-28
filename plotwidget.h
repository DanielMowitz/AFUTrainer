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

#include <qframe.h>
#include <qpoint.h>
#include <qrect.h>
#include <qpen.h>
#include <qbrush.h>
#include <qmap.h>
#include <qfont.h>

class CPlotWidgetPoint;
class CPlotWidgetCurve;
class CPlotWidgetTic;
class CPlotWidget;

class CPlotWidgetPoint : public QPointF
{
public:
	CPlotWidgetPoint()													{ clear(); }
	CPlotWidgetPoint(const double x, const double y) : QPointF(x,y)	{}
	~CPlotWidgetPoint()													{}

	void clear();

protected:
};

class CPlotWidgetCurve : public QList<CPlotWidgetPoint>
{
public:
	CPlotWidgetCurve()													{ clear(); }
	~CPlotWidgetCurve()													{}

	void clear();
	QRectF boundaries() const;

	inline void setPen(const QPen& pen)									{ m_pen = pen; }
	inline QPen pen() const												{ return m_pen; }
	inline void setBrush(const QBrush& brush)							{ m_brush = brush; }
	inline QBrush brush() const											{ return m_brush; }

protected:
	QPen m_pen;
	QBrush m_brush;
};

class CPlotWidgetTic
{
public:
	enum LineType { LineNone, LineShort, LinePlot, LineFull };
	enum FillType { FillNone, FillPlot, FillAll };

	CPlotWidgetTic()													{ clear(); }
	CPlotWidgetTic(const double dPos, const QString& strText);
	CPlotWidgetTic(const double dPos, const double dWidth, const QString& strText);
	CPlotWidgetTic(const double dPos, const QPixmap& pixmap);
	~CPlotWidgetTic()													{}

	void clear();

	inline void setPen(const QPen& pen)									{ m_pen = pen; }
	inline QPen pen() const												{ return m_pen; }
	inline void setBrush(const QBrush& brush)							{ m_brush = brush; }
	inline QBrush brush() const											{ return m_brush; }
	inline void setFont(const QFont& font)								{ m_font = font; }
	inline QFont font() const											{ return m_font; }
	inline void setFontPen(const QPen& pen)								{ m_penFont = pen; }
	inline QPen fontPen() const											{ return m_penFont; }
	inline void setPos(const double dPos)								{ m_dPos = dPos; }
	inline double pos() const											{ return m_dPos; }
	inline void setWidth(const double dWidth)							{ m_dWidth = dWidth; }
	inline double width() const											{ return m_dWidth; }
	inline void setFillType(const FillType t)							{ m_fillType = t; }
	inline FillType fillType () const									{ return m_fillType; }
	inline void setLineType(const LineType t)							{ m_lineType = t; }
	inline LineType lineType () const									{ return m_lineType; }
	inline void setText(const QString& str)								{ m_strText = str; }
	inline QString text() const											{ return m_strText; }
	inline void setTextFlags (const int iFlags)							{ m_iTextFlags = iFlags; }
	inline int textFlags() const										{ return m_iTextFlags; }

	void paintX (QPainter *pPainter, CPlotWidget *pWidget) const;
	void paintY (QPainter *pPainter, CPlotWidget *pWidget) const;

protected:
	double m_dPos;

	// Beschriftung
	QPixmap m_pixmap;
	QString m_strText;
	QFont m_font;
	QPen m_penFont;
	int m_iTextFlags;

	// Line
	QPen m_pen;
	LineType m_lineType;

	// Background
	double m_dWidth;
	QBrush m_brush;
	FillType m_fillType;
};

class CPlotWidget : public QFrame
{
Q_OBJECT
public:
	enum PlotType { PlotPoints, PlotLines, PlotBarsSum, PlotBars };
	enum Border { BorderTop=0x01, BorderBottom=0x02, BorderLeft=0x04, BorderRight=0x08 };

	CPlotWidget(QWidget *pParent=0);
	~CPlotWidget()												{}

	void clear();

	inline void clearCurves()											{ m_listCurves.clear(); }
	inline void appendCurve (const CPlotWidgetCurve& c)					{ m_listCurves.append(c); }

	QRect plotArea() const;
	QRectF rectData() const;

	inline void setType (const PlotType t)								{ m_type = t; }
	inline PlotType type() const										{ return m_type; }
	inline void setLimitX(const double min, const double max)			{ m_dLimitXMin = min; m_dLimitXMax = max; m_bLimitAutoX = false; }
	inline void setLimitY(const double min, const double max)			{ m_dLimitYMin = min; m_dLimitYMax = max; m_bLimitAutoY = false; }
	inline void setAutoLimitX(const bool bAutoLimit)					{ m_bLimitAutoX = bAutoLimit; }
	inline void setAutoLimitY(const bool bAutoLimit)					{ m_bLimitAutoY = bAutoLimit; }
	inline void setAutoLimitRoundX(const double dRound)					{ m_dLimitXRound = dRound; }
	inline void setAutoLimitRoundY(const double dRound)					{ m_dLimitYRound = dRound; }

	inline void setLabelX(const QString& str)							{ m_strLabelX = str; }
	inline void setLabelY(const QString& str)							{ m_strLabelY = str; }

	inline void setPlotBackground (const QBrush& brush)					{ m_brushPlotBkg = brush; }
	inline void setBorderPen(const QPen& pen)							{ m_penBorder = pen; }
	inline QPen borderPen() const										{ return m_penBorder; }
	inline void setBorder(const int iBorder=BorderLeft|BorderBottom)	{ m_iBorder = iBorder; }
	inline int border() const											{ return m_iBorder; }
	void setBorderDistance(const int iLeft, const int iRight, const int iTop, const int iBottom);

	inline void setTicX(const double dTic)								{ m_dTicX = dTic; }
	inline void setTicY(const double dTic)								{ m_dTicY = dTic; }
	inline void setTicXPen(const QPen& pen)								{ m_penTicX = pen; }
	inline void setTicYPen(const QPen& pen)								{ m_penTicY = pen; }
	inline void setTicListX (const QList<CPlotWidgetTic>& list)			{ m_listTicX = list; }
	inline void setTicListY (const QList<CPlotWidgetTic>& list)			{ m_listTicY = list; }

	inline void setBarWidth(const double d)								{ m_dBarWidth = d; }
	inline void setBarOffset(const double d)							{ m_dBarOffset = d; }

protected:
	virtual void paintEvent (QPaintEvent *e);
	void updateCache();
	QPoint mapToPlot (QPointF p);

	friend class CPlotWidgetTic;

protected:
	PlotType m_type;
	QList<CPlotWidgetCurve> m_listCurves;

	double m_dLimitXMin, m_dLimitXMax, m_dLimitYMin, m_dLimitYMax;
	double m_dLimitXRound, m_dLimitYRound;
	bool m_bLimitAutoX, m_bLimitAutoY;

	// Border
	QBrush m_brushPlotBkg;
	QPen m_penBorder;
	int m_iBorder;						//!< Logische Verknüpfung aus BorderTop, BorderBottom, BorderLeft, BorderRight
	int m_iBorderDistTop;
	int m_iBorderDistBottom;
	int m_iBorderDistLeft;
	int m_iBorderDistRight;

	// Tics & Labels
	QList<CPlotWidgetTic> m_listTicX;
	QList<CPlotWidgetTic> m_listTicY;
	double m_dTicX;
	QPen m_penTicX;
	double m_dTicY;
	QPen m_penTicY;
	QString m_strLabelX;
	QString m_strLabelY;

	// Bars
	double m_dBarWidth;
	double m_dBarOffset;

	// Cache
	QRect m_rectPlot;
	QRectF m_rectData;
};

