#ifndef NODEITEM_H
#define NODEITEM_H

#include <vector>
#include <QGraphicsItem>
#include "KoRE/SceneNode.h"
#include "KoRE_GUI/ComponentItem.h"

namespace koregui {
  class NodeItem : public QGraphicsItem {
  public:
    NodeItem(kore::SceneNodePtr sceneNode, QGraphicsItem* parent = 0);
    ~NodeItem(void);

    void refresh(void);
    QRectF boundingRect() const;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget);

    inline int getHeight(void) {return _nodeheight;};
    inline int getWidth(void) {return _nodewidth;};
  private:
    kore::SceneNodePtr _sceneNode;
    uint _nodeheight;
    uint _nodewidth;

    std::vector<ComponentItem*> _componentItems;
    QGraphicsScene* _scene;
  };
}
#endif  // NODEITEM_H
