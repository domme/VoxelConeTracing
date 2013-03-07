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

#include "KoRE_GUI/NodeItem.h"
#include <QPainter>
#include <QStaticText>
#include <QCursor>

koregui::NodeItem::NodeItem(kore::SceneNode* sceneNode,
                            QGraphicsItem* parent)
                            : _sceneNode(sceneNode),
                              QGraphicsItem(parent) {

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setCursor(QCursor(Qt::CursorShape::ArrowCursor));
  std::vector<kore::SceneNodeComponentPtr> components =
    _sceneNode->getComponents();
    for (uint i = 0; i<components.size(); i++) {
      ComponentItem* compItem = new ComponentItem(components[i].get(), this);
      _componentItems.push_back(compItem);
   }
  refresh();
}

koregui::NodeItem::~NodeItem(void) {
}

void koregui::NodeItem::refresh(void) {
  prepareGeometryChange();
  _nodeheight = 40; // place for node name
  for (uint i = 0; i<_componentItems.size(); i++) {
    _componentItems[i]->setPos(0, _nodeheight);
    _nodeheight += _componentItems[i]->getHeight();
  }
  _nodeheight += 20; // lower cap
  _nodewidth = 200;
}

QRectF koregui::NodeItem::boundingRect() const {
  return QRectF(0, 0, _nodewidth, _nodeheight);
}

void koregui::NodeItem::paint(QPainter* painter,
                              const QStyleOptionGraphicsItem* option,
                              QWidget* widget) {
  QBrush b;
  QPen p;
  QFont font("Arial");
  QStaticText text;

  text.setText(_sceneNode->getName().c_str());
  p.setStyle(Qt::PenStyle::NoPen);
  painter->setPen(p);

  b.setStyle(Qt::BrushStyle::SolidPattern);
  b.setColor((isSelected())?QColor(86,86,86):QColor(44,44,44));
  painter->setBrush(b);
  painter->drawRect(0, 0, _nodewidth, _nodeheight);

  font.setBold(true);
  font.setPointSize(12);
  painter->setFont(font);

  p.setColor(QColor(255,255,255));
  p.setStyle(Qt::PenStyle::SolidLine);
  painter->setPen(p);
  painter->drawStaticText(10,10, text);
}
