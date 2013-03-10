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

#include "KoRE_GUI/RenderViewer.h"
#include <QGuiApplication>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include "KoRE_GUI/ShaderProgramItem.h"
#include "KoRE/ResourceManager.h"

koregui::RenderViewer::RenderViewer(QWidget *parent) : QGraphicsView(parent) {
  _mode = DEFAULT;
  setWindowTitle("RenderView");
  _scene.setBackgroundBrush(QBrush(QColor(23,23,23)));
  setScene(&_scene);
  setMinimumSize(800,600);
  const kore::ShaderProgram* shader = kore::ResourceManager::getInstance()->getShaderProgram("MegaShader");
  koregui::ShaderProgramItem* sitem = new koregui::ShaderProgramItem(shader);
  _scene.addItem(sitem);
  _typemap[reinterpret_cast<uint>(sitem)] = SHADER_PROGRAM_ITEM;
  sitem->setPos(0,0);
}

koregui::RenderViewer::~RenderViewer() {
  clearScene();
}

void koregui::RenderViewer::clearScene(void) {
  QList<QGraphicsItem*> sceneset = _scene.items();
  for (unsigned int i = 0; i < sceneset.size(); i++) {
    QGraphicsItem* itemPtr = sceneset[i];
    _scene.removeItem(itemPtr);
    delete(itemPtr);
  }
}

void koregui::RenderViewer::keyPressEvent(QKeyEvent * event) {
  if (event->key() == Qt::Key_Escape) QGuiApplication::quit();
  if (event->key() == Qt::Key_Shift) setDragMode(ScrollHandDrag);
  QGraphicsView::keyPressEvent(event);
}


void koregui::RenderViewer::keyReleaseEvent(QKeyEvent * event) {
  if (event->key() == Qt::Key_Shift) setDragMode(NoDrag);
  QGraphicsView::keyReleaseEvent(event);
}

void koregui::RenderViewer::wheelEvent(QWheelEvent *event) {
  float scaleFactor = pow(2.0, event->delta() / 240.0);
  qreal factor = transform().scale(scaleFactor, scaleFactor)
                            .mapRect(QRectF(0, 0, 1, 1)).width();
  if (factor < 0.08 || factor > 2) return;
  scale(scaleFactor, scaleFactor);
}

void koregui::RenderViewer::contextMenuEvent(QContextMenuEvent *event) {
  ShaderProgramItem* test = static_cast<ShaderProgramItem*>(this->itemAt(event->pos()));
  if (test) {
    test->contextMenu(event->globalPos());
    return;
  }
  QMenu menu("RenderContext", this);
  menu.addAction(QIcon("./assets/icons/testStar.png"),
                 "Create", this, SLOT(zoomIn()),
                 (Qt::CTRL + Qt::Key_N));
  QMenu* lvl2  = menu.addMenu(QIcon("./assets/icons/testStar.png"), "Create");
  lvl2->addAction("EmptyNode", this, SLOT(zoomOut()));
  lvl2->addAction("Group", 0, 0);
  menu.exec(event->globalPos());
}

void koregui::RenderViewer::mousePressEvent(QMouseEvent * event) {
  QGraphicsItem* item = itemAt(event->pos());
  if (item) {
    if(_typemap.count(reinterpret_cast<uint>(item)) != 0) {
      switch(_typemap[reinterpret_cast<uint>(item)]) {
      case NODE_ITEM:
        break;
      case SHADER_DATA_ITEM:
        kore::Log::getInstance()->write("HUHUUUUU!\n");
        break;
      default:
        // do nothing
        break;
      }
    }
  }
  QGraphicsView::mousePressEvent(event);
}

void koregui::RenderViewer::mouseReleaseEvent(QMouseEvent * event) {
  QGraphicsView::mouseReleaseEvent(event);
}

void koregui::RenderViewer::mouseMoveEvent(QMouseEvent *event) {
  QGraphicsView::mouseMoveEvent(event);
}

void koregui::RenderViewer
  ::addSelection(const std::vector<koregui::NodeItem*>& items) {
  for (uint i = 0; i < items.size(); i++) {
    createNode(items[i]->getSceneNode(), 0,0);
  }
}

void koregui::RenderViewer
  ::createNode(kore::SceneNode* sourcenode, int x, int y) {
  NodeItem* nodeItem = new NodeItem(sourcenode);
  _scene.addItem(nodeItem);
  _typemap[reinterpret_cast<uint>(nodeItem)] = NODE_ITEM;
  nodeItem->setPos(x, y);
}
