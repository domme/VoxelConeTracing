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
#include <QCursor>
#include <QMenu>

#include "KoRE_GUI/ShaderPassItem.h"
#include "KoRE_GUI/ResourceViewer.h"
#include "KoRE_GUI/FrameBufferStageItem.h"

#include "KoRE/RenderManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/FrameBuffer.h"

koregui::RenderViewer::RenderViewer(QWidget *parent)
  : _currentpath(NULL),
    _bindTarget(NULL),
    QGraphicsView(parent) {
  setWindowTitle("RenderView");
  _scene.setBackgroundBrush(QBrush(QColor(23,23,23)));
  _scene.setParent(this);
  setScene(&_scene);
  setMinimumSize(800,600);
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
  if (event->key() == Qt::Key_Delete) {
    QList<QGraphicsItem*> sceneset = _scene.selectedItems();
    for (unsigned int i = 0; i < sceneset.size(); i++) {
      QGraphicsItem* itemPtr = sceneset[i];
      // Deletion of FrameBufferStages
      if (itemPtr->data(0) == QVariant("FRAMEBUFFERSTAGE")) {
        auto it = std::find(_framebufferStages.begin(),
                            _framebufferStages.end(),
                            itemPtr);
        if (it != _framebufferStages.end()) {
          _framebufferStages.erase(it);
        }
        _scene.removeItem(itemPtr);
        delete(itemPtr);
      }
      // Deletion of ShaderProgramPasses
      if (itemPtr->data(0) == QVariant("SHADERPROGRAMPASS")) {
        //_scene.removeItem(itemPtr);
        delete(itemPtr);
      }
    }
  }
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
  QGraphicsItem* item = itemAt(event->pos());
  _lastpos = event->pos();
  if (item) {
    if(item->data(0).toString() == "FRAMEBUFFERSTAGE") {
      _currentframebuffer = static_cast<koregui::FrameBufferStageItem*>(item);
      QMenu menu("RenderContext", this);
      QMenu* create  = menu.addMenu(QIcon("./assets/icons/testStar.png"), "Create");
      create->addAction("ShaderPass", this, SLOT(createShaderPass()));
      menu.exec(event->globalPos());
    }
  } else {
    QMenu menu("RenderContext", this);
    QMenu* create  = menu.addMenu(QIcon("./assets/icons/testStar.png"), "Create");
    create->addAction("EmptyNode", this, SLOT(createEmptyNode()));
    create->addAction("Group", this, SLOT(createEmptyGroup()));
    create->addAction("FBO Stage", this, SLOT(createFBOStage()));
    _currentframebuffer = NULL;
    menu.exec(event->globalPos());
  }
}

void koregui::RenderViewer::mousePressEvent(QMouseEvent * event) {
  QGraphicsItem* item = itemAt(event->pos());
  if (item && item->data(0).toString() == "SHADERDATA") {
      _currentpath = new BindPathItem(static_cast<ShaderDataItem*>(item),0);
      _currentpath->setDest(mapToScene(event->pos()));
      _scene.addItem(_currentpath);
  }
  QGraphicsView::mousePressEvent(event);
}

void koregui::RenderViewer::mouseReleaseEvent(QMouseEvent * event) {
  if(_currentpath) {
    if (_bindTarget) {
      _currentpath->setEnd(_bindTarget);
      if (_currentpath->initBinding()) {
        kore::Log::getInstance()->write("added new Binding :-P");
      }
      _bindTarget = NULL;
    } else {
      _scene.removeItem(_currentpath);
    }
     _currentpath = NULL;
  }
  QGraphicsView::mouseReleaseEvent(event);
}

void koregui::RenderViewer::mouseMoveEvent(QMouseEvent *event) {
  if(_currentpath) {
    QGraphicsItem* item = itemAt(event->pos());
    if (item && item->data(0).toString() == "SHADERINPUT") {
      _bindTarget = static_cast<ShaderInputItem*>(item);
      _bindTarget->setBinding(_currentpath);
      _currentpath->setEnd(_bindTarget);
      //item->update();
    } else {
      if(_bindTarget) {
        _currentpath->setEnd(NULL);
        _bindTarget->reset();
        _bindTarget = NULL;
      }
     _currentpath->setDest(mapToScene(event->pos()));
     _currentpath->update();
    }
  }
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
  nodeItem->setPos(x, y);
}

void koregui::RenderViewer
  ::framebufferMoved(FrameBufferStageItem* bufferstage) {
    uint i = 0;
    // find bufferstage index
    for (i; i < _framebufferStages.size(); i++) {
      if (_framebufferStages[i] == bufferstage) {
        break;
      }
    }
    // compare to previous shaderPass, if any
    if (i > 0) {
      if( _framebufferStages[i]->x() < _framebufferStages[i-1]->x()) {
        std::swap(_framebufferStages[i],_framebufferStages[i-1]);
        kore::RenderManager::getInstance()
          ->swapFramebufferStage(_framebufferStages[i]->getStage(),
                                 _framebufferStages[i-1]->getStage());
      }
    }
    // compare to next shaderPass, if any
    if( i < (_framebufferStages.size() - 1)) {
      if( _framebufferStages[i]->pos().x() > _framebufferStages[i+1]->pos().x()) {
        std::swap(_framebufferStages[i],_framebufferStages[i+1]);
        kore::RenderManager::getInstance()
          ->swapFramebufferStage(_framebufferStages[i]->getStage(),
                                 _framebufferStages[i+1]->getStage());
      }
    }
}

void koregui::RenderViewer::createFBOStage(void) {
  koregui::FrameBufferStageItem* fbitem = new koregui::FrameBufferStageItem();
  _scene.addItem(fbitem);
  fbitem->setPos(mapToScene(_lastpos));
  auto at = _framebufferStages.begin();
  for (at; at != _framebufferStages.end(); at++) {
     if (fbitem->x() < (*at)->x()) {
       _framebufferStages.insert(at, fbitem);
       return;
     }
  }
  _framebufferStages.push_back(fbitem);
}

void koregui::RenderViewer::createShaderPass(void) {
  if(_currentframebuffer) {
    koregui::ShaderPassItem* spitem
      = new koregui::ShaderPassItem(_currentframebuffer);
    _currentframebuffer->addShaderPass(spitem);
  }
}

void koregui::RenderViewer::createEmptyNode(void) {

}

void koregui::RenderViewer::createEmptyGroup(void) {

}
