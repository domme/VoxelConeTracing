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

#ifndef NODEPATHITEM_H_
#define NODEPATHITEM_H_

#include <QGraphicsPathItem>
#include <QPainterPath>

namespace koregui {
  class NodeItem;
  class NodePathItem : public QGraphicsPathItem {
  public:
    NodePathItem(NodeItem* start, NodeItem* end, QGraphicsItem* parent = 0);
    ~NodePathItem(void);

    QRectF boundingRect() const;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget);

  private:
    NodeItem* _start;
    NodeItem* _end;
  };
}
#endif  // NODEPATHITEM_H_
