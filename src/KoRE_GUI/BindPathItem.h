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

#ifndef BINDPATHITEM_H_
#define BINDPATHITEM_H_

#include <QGraphicsPathItem>
#include <QPainterPath>
#include "KoRE_GUI/ShaderDataItem.h"
#include "KoRE_GUI/ShaderInputItem.h"

namespace koregui {
  class NodeItem;
  class BindPathItem : public QGraphicsPathItem {
  public:
    BindPathItem(ShaderDataItem* start, ShaderInputItem* end, QGraphicsItem* parent = 0);
    ~BindPathItem(void);

    QRectF boundingRect() const;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget);
    void setDest(QPointF dest) {_dragend = dest;}
    void setEnd(ShaderInputItem* end) {_end = end;}

  private:
    ShaderDataItem* _start;
    ShaderInputItem* _end;
    QPointF _dragend;
  };
}
#endif  // BINDPATHITEM_H_
