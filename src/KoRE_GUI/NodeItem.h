#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
//#include "KoRE/SceneNode.h"

class NodeItem : public QGraphicsItem {
public:
  //NodeItem(kore::SceneNode* sceneNode, QGraphicsItem* parent = 0);
  NodeItem(QGraphicsItem* parent = 0);
  ~NodeItem(void);
  QRectF boundingRect() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
private:
  //kore::SceneNode* _sceneNode;
};
#endif  // NODEITEM_H
