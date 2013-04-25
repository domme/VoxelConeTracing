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

#include "KoRE_GUI/ShaderPassItem.h"

#include <QMenu>
#include <QPainter>
#include <QStaticText>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

#include "KoRE/ShaderInput.h"
#include "KoRE/ShaderData.h"
#include "KoRE_GUI/ShaderEditor.h"
#include "KoRE_GUI/FrameBufferStageItem.h"
#include "KoRE_GUI/ShaderInputItem.h"

koregui::ShaderPassItem::ShaderPassItem(QGraphicsItem* parent) 
                              : _shader(NULL),
                                _name("<empty>"),
                                QGraphicsItem(parent) {
  _programpass = new kore::ShaderProgramPass();
  setData(0, "SHADERPROGRAMPASS");
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setCursor(QCursor(Qt::CursorShape::ArrowCursor));
  refresh();
}

koregui::ShaderPassItem::~ShaderPassItem(void) {
   koregui::FrameBufferStageItem * fbs
     = static_cast<koregui::FrameBufferStageItem*>(parentItem());
   if (fbs) {
     fbs->removeShaderPass(this);
   }
}

void koregui::ShaderPassItem::setShaderProgram(kore::ShaderProgram* prog) {
  _shader = prog;
  refresh();
}

void koregui::ShaderPassItem::refresh(void) {
  prepareGeometryChange();
  // destroy old shader inputs
  if(_attributes.size() > 0) {
    for (uint i = 0; i < _attributes.size(); i++) {
      delete(_attributes[i]);
    }
    _attributes.clear();
  }
  if(_uniforms.size() > 0) {
    for (uint j = 0; j < _uniforms.size(); j++) {
      delete(_uniforms[j]);
    }
    _uniforms.clear();
  }
  if(_sampler.size() > 0) {
    for (uint k = 0; k < _sampler.size(); k++) {
      delete(_sampler[k]);
    }
    _sampler.clear();
  }
  _programpass->setShaderProgram(_shader);
  // create inputs from shaderprogram object
  int tmpheight = 40;
  if(_shader) {
    _name = _shader->getName();
    std::vector<kore::ShaderInput> sinput = _shader->getAttributes();
    for (uint i = 0; i < sinput.size(); i++) {
      const kore::ShaderInput* tmp = _shader->getAttribute(sinput[i].name);
      ShaderInputItem* inpitem =  new ShaderInputItem(tmp, this, this);
      _attributes.push_back(inpitem);
      inpitem->setPos(-4, tmpheight + 30 * i);
    }
    tmpheight += 30 * sinput.size();
    sinput = _shader->getUniforms();
    for (uint j = 0; j < sinput.size(); j++) {
      const kore::ShaderInput* tmp = _shader->getUniform(sinput[j].name);
      ShaderInputItem* inpitem =  new ShaderInputItem(tmp, this, this);
      _uniforms.push_back(inpitem);
      inpitem->setPos(-4, tmpheight + 30 * j);
    }
  }

  _shaderheight = 40; // place for shader name
  _shaderheight += 30 * _attributes.size();
  _shaderheight += 30 * _uniforms.size();
  _shaderheight += 30 * _sampler.size();

  _shaderwidth = 200;
  static_cast<koregui::FrameBufferStageItem*>(parentItem())->refresh();
}

QRectF koregui::ShaderPassItem::boundingRect() const {
  return QRectF(0, 0, _shaderwidth, _shaderheight);
}

void koregui::ShaderPassItem::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget) {
  QBrush b;
  QPen p;
  QFont font("Arial");
  QStaticText text;

  p.setStyle(Qt::PenStyle::NoPen);
  b.setColor((isSelected())?QColor(86,86,86):QColor(66,66,66));
  b.setStyle(Qt::BrushStyle::SolidPattern);
  painter->setPen(p);
  painter->setBrush(b);
  painter->drawRect(0, 0, _shaderwidth, _shaderheight);

  font.setBold(true);
  font.setPointSize(12);
  painter->setFont(font);
  painter->drawImage(_shaderwidth - 26, 10, QImage("./assets/icons/gear.png"));

  text.setText(_name.c_str());
  p.setColor(QColor(255,255,255));
  p.setStyle(Qt::PenStyle::SolidLine);
  painter->setPen(p);
  painter->drawStaticText(10,10, text);

  font.setFamily("Consolas");
  font.setPointSize(9);
  p.setColor(QColor(200,200,200));
  painter->setPen(p);
  painter->setFont(font);

  int tmpheight = 40;
  for (uint i = 0; i<_attributes.size(); i++) {
    text.setText(_attributes[i]->getInput()->name.c_str());
     painter->drawStaticText(12,tmpheight + 30 * i, text);
  }
  tmpheight += 30 * _attributes.size();
  for (uint i = 0; i<_uniforms.size(); i++) {
    text.setText(_uniforms[i]->getInput()->name.c_str());
    painter->drawStaticText(12,tmpheight + 30 * i, text);
  }
}

void koregui::ShaderPassItem
  ::mousePressEvent(QGraphicsSceneMouseEvent * event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    QPointF p = event->pos();
    if (p.y() < 26 && p.x() > _shaderwidth - 26) {
      koregui::ShaderEditor* ed = new koregui::ShaderEditor(this);
      ed->setShaderProgram(QString(_name.c_str()));
      ed->show();
    }
  }
  QGraphicsItem::mousePressEvent(event);
}

void koregui::ShaderPassItem
  ::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  static_cast<koregui::FrameBufferStageItem*>(parentItem())->shaderMoved(this);
  QGraphicsItem::mouseMoveEvent(event);
}

void koregui::ShaderPassItem
  ::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  static_cast<koregui::FrameBufferStageItem*>(this->parentItem())->refresh();
  QGraphicsItem::mouseReleaseEvent(event);
}
