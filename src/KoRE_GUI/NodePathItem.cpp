#include "NodePathItem.h"
#include <QPainter>

koregui::NodePathItem::NodePathItem(NodeItem* start,
                                    NodeItem* end,
                                    QGraphicsItem* parent)
                                  : _start(start),
                                    _end(end),
                                    QGraphicsItem(parent){
  setAcceptedMouseButtons(Qt::NoButton);
}

koregui::NodePathItem::~NodePathItem(void) {
}

QRectF koregui::NodePathItem::boundingRect() const {
  return _path.boundingRect();
}

void koregui::NodePathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  QPointF start = _start->pos() + QPointF(_start->getWidth()/2, _start->getHeight());
  QPointF dest = _end->pos() + QPointF(_start->getWidth()/2, 0);

  QPainterPath path;
  path.moveTo(start);
  path.cubicTo(start + QPointF(0,50),
               dest - QPointF(0,50),
               dest);

  prepareGeometryChange();
  _path = path;
  painter->setPen(QPen(QColor(200, 200, 200), 3));
  painter->drawPath(path);
  //painter->drawPolygon(poly);
}
