#ifndef NODEPATHITEM_H_
#define NODEPATHITEM_H_

#include <QGraphicsItem>
#include <QPainterPath>
#include "KoRE_GUI/NodeItem.h"

namespace koregui {
  class NodePathItem : public QGraphicsItem {
  public:
    NodePathItem(NodeItem* start, NodeItem* end, QGraphicsItem* parent = 0);
    ~NodePathItem(void);

    QRectF boundingRect() const;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget);

  private:
    NodeItem* _start;
    NodeItem* _end;
    QPainterPath _path;
  };
}
#endif  // NODEPATHITEM_H_