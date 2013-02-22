#ifndef NODEITEM_H
#define NODEITEM_H

#include <vector>
#include <QGraphicsItem>
#include "KoRE/SceneNode.h"
#include "KoRE_GUI/ShaderDataItem.h"

namespace koregui {
  class NodeItem : public QGraphicsItem {
  public:
    NodeItem(kore::SceneNodePtr sceneNode, QGraphicsItem* parent = 0);
    ~NodeItem(void);

    void refresh(void);
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  private:
    kore::SceneNodePtr _sceneNode;
    uint _nodeheight;
    uint _nodewidth;

    std::vector<ShaderDataItem*> _shaderDataItems;
    QGraphicsScene* _scene;
  };
}
#endif  // NODEITEM_H
