#include "SceneViewer.h"
#include <QList>
#include "NodeItem.h"

koregui::SceneViewer::SceneViewer(QWidget *parent) : QGraphicsView(parent) {
  _scene.setBackgroundBrush(QBrush(Qt::darkGray));
  setScene(&_scene);

  // setCacheMode(QGraphicsItem::CacheMode::DeviceCoordinateCache);
  setMinimumSize(800,600);
}

koregui::SceneViewer::~SceneViewer() {
  //clearTree();
}

void koregui::SceneViewer::showScene(kore::SceneNodePtr root) {
  clearScene();
  createNode(root);
}

void koregui::SceneViewer::clearScene(void) {
  QList<QGraphicsItem*> sceneset = _scene.items();
  for (unsigned int i = 0; i < sceneset.size(); i++) {
    QGraphicsItem* itemPtr = sceneset[i];
    _scene.removeItem(itemPtr);
    delete(itemPtr);
  }
}

void koregui::SceneViewer::createNode(kore::SceneNodePtr sourcenode) {
  NodeItem* nodeItem = new NodeItem(sourcenode);
  std::vector<kore::SceneNodePtr> pnode = sourcenode->getChildren();
  for (unsigned int i = 0; i<pnode.size(); i++) {
    createNode(pnode[i]);
  }
  nodeItem->setPos(0, 0);
  _scene.addItem(nodeItem);
}
