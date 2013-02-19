#include "SceneViewer.h"
#include <QList>
#include "NodeItem.h"

kore::SceneViewer::SceneViewer(QWidget *parent) : QGraphicsView(parent) {
  _scene.setBackgroundBrush(QBrush(Qt::darkGray));
  setScene(&_scene);
  NodeItem* n = new NodeItem();
  _scene.addItem(n);
}

/*SceneViewer::SceneViewer(kore::SceneNode *root, QWidget *parent) : QGraphicsView(parent) {
  _scene.setBackgroundBrush(QBrush(Qt::darkGray));
  setScene(&_scene);
  //showTree(root);
}*/

kore::SceneViewer::~SceneViewer() {
  //clearTree();
}

/*void SceneViewer::showTree( kore::SceneNode *root ) {
  clearTree();
  createNode(root);
}*/

/*void kore::SceneViewer::clearTree(void) {
  QList<QGraphicsItem*> sceneset = _scene.items();
  for (unsigned int i = 0; i < sceneset.size(); i++) {
    QGraphicsItem* itemPtr = sceneset[i];
    _scene.removeItem(itemPtr);
    delete(itemPtr);
  }
}*/

/*void SceneViewer::createNode(kore::SceneNode *sourcenode) {
  NodeItem* nodeItem = new NodeItem(sourcenode);
  for (unsigned int i = 0; i<sourcenode->getChildren().size(); i++) {
    // TODO(dospelt) add node loading for children
  }
  nodeItem->setPos(0, 0);
  _scene.addItem(nodeItem);
}*/
