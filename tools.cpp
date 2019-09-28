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

#include "tools.h"

#include <qpainter.h>
#include <qpalette.h>

unsigned afu_random (const unsigned uMin, const unsigned uMax)
{
unsigned u=0;
float f = (float) (uMax-uMin+1) / (float) RAND_MAX;
	u = (unsigned) ((float) rand () * f) + uMin;;
	return (u);
}

QDomElement createXmlTextElement(const QString& strName, const QString& strText, QDomDocument& doc)
{
QDomElement elem = doc.createElement(strName);
	elem.appendChild(doc.createTextNode(strText));
	return elem;
}

QPixmap createProgressBar (int w, int h, double dPercent)
{
QPixmap pix(w,h);
QPainter p(&pix);

	pix.fill(QPalette().color(QPalette::Dark));
	p.setBrush(Qt::blue);
	p.setPen(Qt::blue);
	if (dPercent != 0.0)
		p.drawRect(0, 0, (int)(((double)w)*dPercent), h);
	return pix;
}
