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
#include "KoRE_GUI/ShaderItem.h"
#include "KoRE/RenderManager.h"

koregui::RenderViewer::RenderViewer(QWidget *parent) : QGraphicsView(parent) {
  setDragMode(ScrollHandDrag);
  // setDragMode(RubberBandDrag);
  _scene.setBackgroundBrush(QBrush(Qt::darkGray));
  setScene(&_scene);
  setMinimumSize(800,600);
  const kore::ShaderProgram* shader = kore::RenderManager::getInstance()->getShaderProgram("MegaShader");
  koregui::ShaderItem* sitem = new koregui::ShaderItem(shader);
  _scene.addItem(sitem);
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
  QGraphicsView::keyPressEvent(event);
}

void koregui::RenderViewer::wheelEvent(QWheelEvent *event) {
  float scaleFactor = pow(2.0, event->delta() / 240.0);
  qreal factor = transform().scale(scaleFactor, scaleFactor)
                            .mapRect(QRectF(0, 0, 1, 1)).width();
  if (factor < 0.08 || factor > 2) return;
  scale(scaleFactor, scaleFactor);
}

void koregui::RenderViewer::contextMenuEvent(QContextMenuEvent *event) {
  ShaderItem* test = static_cast<ShaderItem*>(this->itemAt(event->pos()));
  if (test) {
    test->contextMenu(event->globalPos());
    return;
  }
  QMenu menu("TEST", this);
  menu.hideTearOffMenu();
  menu.addAction(QIcon("./assets/icons/testStar.png"), "Add", this, SLOT(zoomIn()), (Qt::CTRL + Qt::Key_N));
  menu.addAction("tu jenes", 0, 0);
  QMenu* lvl2  = menu.addMenu("Totoal");
  lvl2->addAction("Schnorch!", this, SLOT(zoomOut()));
  lvl2->addAction("Blubb!", 0, 0);
  //menu.addAction(...);
  //menu.addAction(...);
  menu.exec(event->globalPos());
}

void koregui::RenderViewer::addSelection(const QList<QGraphicsItem*>& items) {
  for (uint i = 0; i < items.size(); i++) {
    _scene.addItem(items[i]);
  }
}
