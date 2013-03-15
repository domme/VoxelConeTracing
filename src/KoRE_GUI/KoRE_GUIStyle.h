/*
  Copyright (c) 2012 The KoRE Project

  This file is part of KoRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoRE.  If not, see <http://www.gnu.org/licenses/>.
*/

/************************************************************************/
/* \author Dominik Ospelt                                               */
/************************************************************************/

#ifndef KOREGUISTYLE_H_
#define KOREGUISTYLE_H_

#include <QCommonStyle>
#include <QPalette>


class QPainterPath;
namespace koregui {
  class KoRE_GUIStyle : public QCommonStyle {
    Q_OBJECT

  public:
    KoRE_GUIStyle() {}

    void polish(QPalette &palette);
    void polish(QWidget *widget);
    void unpolish(QWidget *widget);

    int pixelMetric(PixelMetric metric,
      const QStyleOption *option,
      const QWidget *widget) const;

    int styleHint(StyleHint hint,
      const QStyleOption *option,
      const QWidget *widget,
      QStyleHintReturn *returnData) const;

    void drawPrimitive(PrimitiveElement element,
      const QStyleOption *option,
      QPainter *painter,
      const QWidget *widget) const;

    void drawControl(ControlElement control,
      const QStyleOption *option,
      QPainter *painter,
      const QWidget *widget) const;

  private:
    static void setTexture(QPalette &palette,
      QPalette::ColorRole role,
      const QPixmap &pixmap);
    static QPainterPath roundRectPath(const QRect &rect);
  };
}

#endif  // KOREGUISTYLE_H_
