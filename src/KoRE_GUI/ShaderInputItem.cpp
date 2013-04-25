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

#include "KoRE_GUI/ShaderInputItem.h"
#include <QPainter>
#include <QCursor>

koregui::ShaderInputItem::ShaderInputItem(const kore::ShaderInput* input,
                                          ShaderPassItem* pass,
                                          QGraphicsItem* parent)
                                      : _input(input),
                                        _pass(pass),
                                        _binding(NULL),
                                        _mouseover(false),
                                        QGraphicsItem(parent) {
  setAcceptHoverEvents(true);
  setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  setData(0, "SHADERINPUT");
}

koregui::ShaderInputItem::~ShaderInputItem(void) {
}

void koregui::ShaderInputItem::reset(void) {
  _binding = NULL;
  _mouseover = false;
}


QRectF koregui::ShaderInputItem::boundingRect() const {
  return QRectF(-2, -2, 16, 16);
}

void koregui::ShaderInputItem::paint(QPainter* painter,
                                     const QStyleOptionGraphicsItem* option,
                                     QWidget* widget) {
  QPen p;
  p.setColor(QColor(44,44,44));
  p.setWidth(2);

  QBrush b;
  switch(_input->type) {
    case GL_FLOAT_MAT4:
      b.setColor(Qt::GlobalColor::green);
      break;
    default : b.setColor(Qt::GlobalColor::red);
  }
  b.setStyle(Qt::BrushStyle::SolidPattern);
  painter->setBrush(b);
  painter->setPen(p);
  if(_mouseover) {
    painter->drawRect(-2, -2, 16, 16);
  } else {
    painter->drawRect(0, 0, 12, 12);
  }
}

void koregui::ShaderInputItem
  ::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
  _mouseover = true;
  QGraphicsItem::hoverEnterEvent(event);
}
void koregui::ShaderInputItem
  ::hoverLeaveEvent(QGraphicsSceneHoverEvent * event) {
  _mouseover = false;
  QGraphicsItem::hoverLeaveEvent(event);
}
void koregui::ShaderInputItem::setBinding(koregui::BindPathItem* binding) {
  _binding = binding;
}
