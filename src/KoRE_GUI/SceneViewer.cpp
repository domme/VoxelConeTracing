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

#include "KoRE_GUI/SceneViewer.h"
#include <QGuiApplication>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QGraphicsScene>
#include "KoRE_GUI/RenderViewer.h"
#include "KoRE_GUI/NodeItem.h"
#include "KoRE_GUI/NodePathItem.h"

koregui::SceneViewer::SceneViewer(koregui::RenderViewer* renderview, QWidget *parent)
                                : _renderview(renderview),
                                  QGraphicsView(parent) {
  setWindowTitle("SceneView");
  setDragMode(RubberBandDrag);
  _scene.setBackgroundBrush(QBrush(QColor(23,23,23)));
  setScene(&_scene);
  setMinimumSize(800,600);
}

koregui::SceneViewer::~SceneViewer() {
  clearScene();
}

void koregui::SceneViewer::showScene(kore::SceneNode* root) {
  clearScene();
  createNode(root, 0,0);
}

void koregui::SceneViewer::clearScene(void) {
  QList<QGraphicsItem*> sceneset = _scene.items();
  for (unsigned int i = 0; i < sceneset.size(); i++) {
    QGraphicsItem* itemPtr = sceneset[i];
    _scene.removeItem(itemPtr);
    delete(itemPtr);
  }
}

koregui::NodeItem* koregui::SceneViewer
  ::createNode(kore::SceneNode* sourcenode,
               int x,
               int y) {
  NodeItem* nodeItem = new NodeItem(sourcenode);
  _scene.addItem(nodeItem);

  std::vector<kore::SceneNodePtr> pnode = sourcenode->getChildren();
  int startx = x -(estimateTreeWidth(sourcenode)/2);
  for (unsigned int i = 0; i<pnode.size(); i++) {
    int localwidth = estimateTreeWidth(pnode[i].get());
    NodePathItem* path = new NodePathItem(nodeItem,
                                          createNode(pnode[i].get(),
                                            startx + localwidth/2,
                                            y + nodeItem->getHeight() + 80));
    path->setZValue(-1);
    _scene.addItem(path);
    startx += localwidth;
  }
  nodeItem->setPos(x, y);
  return nodeItem;
}
int koregui::SceneViewer::estimateTreeWidth(kore::SceneNode* sourcenode) {
  std::vector<kore::SceneNodePtr> pnode = sourcenode->getChildren();
  int width = 250;
  if(pnode.size()!= 0) width = 0;
  for (unsigned int i = 0; i<pnode.size(); i++) {
    width += estimateTreeWidth(pnode[i].get());
  }
  return width;
}

void koregui::SceneViewer::keyPressEvent(QKeyEvent * event) {
  if (event->key() == Qt::Key_Escape) QGuiApplication::quit();
  if (event->key() == Qt::Key_Shift) setDragMode(ScrollHandDrag);
  QGraphicsView::keyPressEvent(event);
}

void koregui::SceneViewer::keyReleaseEvent(QKeyEvent * event) {
  if (event->key() == Qt::Key_Shift) setDragMode(RubberBandDrag);
  QGraphicsView::keyReleaseEvent(event);
}

void koregui::SceneViewer::wheelEvent(QWheelEvent *event) {
  float scaleFactor = pow(2.0, event->delta() / 240.0);

  qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
  if (factor < 0.08 || factor > 2) return;
  scale(scaleFactor, scaleFactor);
  //QGraphicsView::wheelEvent(event);
}

void koregui::SceneViewer::mousePressEvent(QMouseEvent * event) {
  QGraphicsView::mousePressEvent(event);
}

void koregui::SceneViewer::contextMenuEvent(QContextMenuEvent *event) {
  QMenu menu("SceneContext", this);
  menu.addAction(QIcon("./assets/icons/testStar.png"), "Add Selection to Renderview", this, SLOT(addSelectionToRenderview()), (Qt::CTRL + Qt::Key_A));
  menu.exec(event->globalPos());
}

void koregui::SceneViewer::addSelectionToRenderview() {
  QList<QGraphicsItem*> list = _scene.selectedItems();
  std::vector<NodeItem*> nodes;
  for(uint i = 0; i < list.size(); i++){
    nodes.push_back(static_cast<NodeItem*>(list.at(i)));
  }
  _renderview->addSelection(nodes);
}
