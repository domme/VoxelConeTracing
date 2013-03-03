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

#include "KoRE_GUI/ShaderItem.h"

#include <QPainter>
#include <QStaticText>
#include <QGraphicsSceneMouseEvent>

#include "KoRE/ShaderInput.h"
#include "KoRE/ShaderData.h"

koregui::ShaderItem::ShaderItem(kore::Shader* shader,
                                      QGraphicsItem* parent) 
                                    : _shader(shader),
                                      QGraphicsItem(parent) {
  setFlag(QGraphicsItem::ItemIsMovable, true);
  refresh();
}

koregui::ShaderItem::~ShaderItem(void) {
}

void koregui::ShaderItem::refresh(void) {
  _shaderheight = 200;
  _shaderwidth = 200;
}

QRectF koregui::ShaderItem::boundingRect() const {
  return QRectF(0, 0, _shaderwidth, _shaderheight);
}

void koregui::ShaderItem::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget) {
  QBrush b;
  QPen p;
  QStaticText t;
  QFont font("Consolas");

  painter->drawRect(0,0,200,200);
}

void koregui::ShaderItem::mousePressEvent(QGraphicsSceneMouseEvent * event){
  QGraphicsItem::mousePressEvent(event);
}

