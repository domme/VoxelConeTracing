#include "SceneViewer.h"
#include <QGuiApplication>
#include <QKeyEvent>
#include <QList>
#include "KoRE_GUI/NodeItem.h"
#include "KoRE_GUI/NodePathItem.h"

koregui::SceneViewer::SceneViewer(QWidget *parent) : QGraphicsView(parent) {
  //setDragMode(ScrollHandDrag);
  //setDragMode(RubberBandDrag);
  _scene.setBackgroundBrush(QBrush(Qt::darkGray));
  setScene(&_scene);
  setMinimumSize(800,600);
}

koregui::SceneViewer::~SceneViewer() {
  clearScene();
}

void koregui::SceneViewer::showScene(kore::SceneNodePtr root) {
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
  ::createNode(kore::SceneNodePtr sourcenode,
               int x,
               int y) {
  NodeItem* nodeItem = new NodeItem(sourcenode);
  nodeItem->setPos(x, y);
  _scene.addItem(nodeItem);

  std::vector<kore::SceneNodePtr> pnode = sourcenode->getChildren();
  int startx = x -(estimateTreeWidth(sourcenode)/2);
  for (unsigned int i = 0; i<pnode.size(); i++) {
    int localwidth = estimateTreeWidth(pnode[i]);
    NodePathItem* path = new NodePathItem(nodeItem,
                                          createNode(pnode[i],
                                            startx + localwidth/2,
                                            y + nodeItem->getHeight() + 80));
    _scene.addItem(path);
    startx += localwidth;
  }
  return nodeItem;
}
int koregui::SceneViewer::estimateTreeWidth(kore::SceneNodePtr sourcenode) {
  std::vector<kore::SceneNodePtr> pnode = sourcenode->getChildren();
  int width = 250;
  if(pnode.size()!= 0) width = 0;
  for (unsigned int i = 0; i<pnode.size(); i++) {
    width += estimateTreeWidth(pnode[i]);
  }
  return width;
}

void koregui::SceneViewer::keyPressEvent(QKeyEvent * evnt) {
  if (evnt->key() == Qt::Key_Escape) QGuiApplication::quit();
}
