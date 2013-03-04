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

#include "KoRE_GUI/NodePathItem.h"
#include "KoRE_GUI/NodeItem.h"
#include <QPainter>

koregui::NodePathItem::NodePathItem(NodeItem* start,
                                    NodeItem* end,
                                    QGraphicsItem* parent)
                                  : _start(start),
                                    _end(end),
                                    QGraphicsPathItem(parent){
  setAcceptedMouseButtons(Qt::NoButton);
}

koregui::NodePathItem::~NodePathItem(void) {
}

QRectF koregui::NodePathItem::boundingRect() const {
  return path().boundingRect().adjusted(-2,-2,2,2);
}

void koregui::NodePathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

  QPointF start = mapFromItem(_start,_start->getWidth()/2,_start->getHeight());
  QPointF dest = _end->pos() + QPointF(_end->getWidth()/2, 0);
  QPointF dist = QPointF(0, (dest.y()- start.y())/2);
  QPainterPath path(start);
  path.cubicTo(start + dist, dest - dist, dest);

  setPath(path);
  painter->setPen(QPen(QColor(200, 200, 200), 3));
  painter->drawPath(path);
}
