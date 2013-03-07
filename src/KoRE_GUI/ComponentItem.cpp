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

#include "KoRE_GUI/ComponentItem.h"

#include <QPainter>
#include <QStaticText>
#include <QGraphicsSceneMouseEvent>

#include "KoRE_GUI/NodeItem.h"
#include "KoRE/ShaderData.h"

koregui::ComponentItem::ComponentItem(kore::SceneNodeComponent* component,
                                      QGraphicsItem* parent) 
                                    : _component(component),
                                      QGraphicsItem(parent) {
  std::vector<kore::ShaderData> sdata = _component->getShaderData();
  for (uint i = 0; i < sdata.size(); i++) {
    const kore::ShaderData* tmp = _component->getShaderData(sdata[i].name);
    ShaderDataItem* dataitem =  new ShaderDataItem(tmp, this);
    _shaderDataItems.push_back(dataitem);
    dataitem->setVisible(true);
    dataitem->setPos(192, 30 + 30 * i);
  }
  _expanded = false;
  refresh();
}

koregui::ComponentItem::~ComponentItem(void) {
}

void koregui::ComponentItem::refresh(void) {
  _componentheight = 30;
  if(_expanded) {
    _componentheight += _shaderDataItems.size() * 30;
  }
  for (uint i= 0; i < _shaderDataItems.size(); i++) {
    _shaderDataItems[i]->setVisible(_expanded);
  }
  _componentwidth = 200;
  prepareGeometryChange();
  koregui::NodeItem* ni = reinterpret_cast<koregui::NodeItem*>(this->parentItem());
  ni->refresh();
}

QRectF koregui::ComponentItem::boundingRect() const {
  return QRectF(0, 0, _componentwidth, _componentheight);
}

void koregui::ComponentItem::paint(QPainter* painter,
                                   const QStyleOptionGraphicsItem* option,
                                   QWidget* widget) {
  QBrush b;
  QPen p;
  QStaticText t;
  QFont font("Arial");

  font.setBold(true);
  font.setPointSize(9);
  painter->setFont(font);
  p.setStyle(Qt::PenStyle::NoPen);
  painter->setPen(p);
  b.setStyle(Qt::BrushStyle::SolidPattern);
  switch (_component->getType()) {
  case kore::COMPONENT_TRANSFORM:
    b.setColor(QColor(35,203,173));
    t.setText("Transform");
    break;
  case kore::COMPONENT_MESH:
    b.setColor(QColor(252,210,89));
    t.setText("Mesh");
    break;
  case kore::COMPONENT_CAMERA:
    b.setColor(QColor(199,89,214));
    t.setText("Camera");
    break;
  case kore::COMPONENT_LIGHT:
    b.setColor(QColor(250,123,28));
    t.setText("Light");
    break;
  case kore::COMPONENT_UNKNOWN:
  default:
    b.setColor(Qt::GlobalColor::red);
  }

  // draw component color
  painter->setBrush(b);
  painter->drawRect(0, 0, _componentwidth, 20);
  if (_expanded)painter->drawRect(0, 0, 10, _componentheight);

  // draw text
  p.setColor(QColor(33,33,33));
  p.setStyle(Qt::PenStyle::SolidLine);
  p.setWidth(2);
  painter->setPen(p);
  painter->drawStaticText(10, 2, t);

  int currentheight = 30;

  if (_expanded) {
    painter->drawLine(_componentwidth - 16,10,_componentwidth-10,10);
    for (uint i = 0; i<_shaderDataItems.size(); i++) {
      t.setText((_shaderDataItems[i]->getData()->name.c_str()));
      font.setFamily("Consolas");
      painter->setFont(font);
      p.setStyle(Qt::PenStyle::SolidLine);
      p.setColor(QColor(220,220,220));
      painter->setPen(p);
      painter->drawStaticText(14,currentheight,t);
      currentheight += 30;
    }
  } else {
    painter->drawLine(_componentwidth - 16,10,_componentwidth-10,10);
    painter->drawLine(_componentwidth - 13,7,_componentwidth-13,13);
  }
}

void koregui::ComponentItem::mousePressEvent(QGraphicsSceneMouseEvent * event){
  
  if (event->button() == Qt::MouseButton::LeftButton) {
    QPointF p = event->pos();//event->buttonDownPos(Qt::MouseButton::LeftButton);
    if (p.y() < 20 && p.x() > _componentwidth - 18) {
      _expanded = !_expanded;
      refresh();
    }
  }
  QGraphicsItem::mousePressEvent(event);
}

