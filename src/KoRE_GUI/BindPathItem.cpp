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

#include "KoRE/Operations/BindOperations/BindAttribute.h"
#include "KoRE/Operations/BindOperations/BindUniform.h"
#include "KoRE/Operations/BindOperations/BindTexture.h"
#include "KoRE/Operations/BindOperations/BindImageTexture.h"
#include "KoRE/Operations/RenderMesh.h"

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

bool koregui::BindPathItem::checkBinding(koregui::ShaderInputItem* target) {
  // TODO(dospelt) implement check
  return true;
}

bool koregui::BindPathItem::initBinding(void) {
  kore::NodePass* nodePass = NULL;
  // see if nodePass already exists
  std::vector<kore::NodePass*> npasses =
    _end->getShaderPass()->getProgramPass()->getNodePasses();
  for (uint i = 0; i < npasses.size(); i++) {
    if(npasses[i]->getSceneNode() == _start->getNodeItem()->getSceneNode()) {
      nodePass = npasses[i];
      break;
    }
  }
  // if not, create new
  if(!nodePass) {
    nodePass = new kore::NodePass(_start->getNodeItem()->getSceneNode());
    _end->getShaderPass()->getProgramPass()->addNodePass(nodePass);
  }

  kore::ShaderProgram* prog = const_cast<kore::ShaderProgram*>(
    _end->getShaderPass()->getProgramPass()->getShaderProgram());

  // attribute binding
  if (prog->getAttribute(_end->getInput()->name) != NULL) {
    _bindOP = new kore::BindAttribute(_start->getData(), _end->getInput());
    nodePass->addOperation(_bindOP);
    if(_start->getData()->component->getType() == kore::COMPONENT_MESH) {
      // add renderMesh Op, if necessary
      std::vector<kore::Operation*> ops = nodePass->getOperations();
      if (ops[ops.size()-1]->getType() != kore::OP_RENDERMESH) {
        kore::RenderMesh* _renderOP = new kore::RenderMesh(
          static_cast<kore::MeshComponent*>(_start->getData()->component));
          nodePass->addOperation(_renderOP);
      }
    }
   return true;
  }

  // uniform binding
  if((prog->getUniform(_end->getInput()->name)) != NULL) {
    _bindOP = new kore::BindUniform(_start->getData(), _end->getInput());
    nodePass->addOperation(_bindOP);
    return true;
  }

  // texture binding
  // TODO(dospelt)*/
  return false;
}

void koregui::BindPathItem::removeBinding() {
}

void koregui::BindPathItem::startAnimation() {
}

void koregui::BindPathItem::stopAnimation() {
}

void koregui::BindPathItem::animate() {
}
