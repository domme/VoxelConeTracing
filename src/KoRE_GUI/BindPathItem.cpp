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

#include "KoRE_GUI/BindPathItem.h"
#include "KoRE_GUI/NodeItem.h"
#include <QPainter>
#include <QCursor>

koregui::BindPathItem::BindPathItem(ShaderDataItem* start,
                                    ShaderInputItem* end,
                                    QGraphicsItem* parent)
                                  : _start(start),
                                    _end(end),
                                    QGraphicsPathItem(parent){
  setData(0, "BINDPATH");
  setAcceptedMouseButtons(Qt::NoButton);
  setCursor(QCursor(Qt::CursorShape::WhatsThisCursor));
  setZValue(-10);
}

koregui::BindPathItem::~BindPathItem(void) {
}

QRectF koregui::BindPathItem::boundingRect() const {
  return path().boundingRect().adjusted(-2,-2,2,2);
}

void koregui::BindPathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  // TODO(dospelt) use QPainterPathStroker
  QPointF start = mapFromItem(_start,6,6);
  QPointF dest = (_end)?mapFromItem(_end,6,6):_dragend;
  QPointF dist = QPointF((dest.x()- start.x())/2, 0);
  QPainterPath path(start);
  path.cubicTo(start + dist, dest - dist, dest);

  setPath(path);

  painter->setPen(QPen(QColor(200, 200, 200), 2));
  painter->drawPath(path);
}
