#include "NodeItem.h"
#include <QPainter>
#include <QCursor>

NodeItem::NodeItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
  //setCursor(QCursor(Qt::OpenHandCursor));
  setFlag(QGraphicsItem::ItemIsMovable, true);
  //QGraphicsItem::ItemIsSelectable
}

/*NodeItem::NodeItem(kore::SceneNode* sceneNode, QGraphicsItem* parent) : QGraphicsItem(parent) {
  //setCursor(QCursor(Qt::OpenHandCursor));
  setFlag(QGraphicsItem::ItemIsMovable, true);
  //QGraphicsItem::ItemIsSelectable
}*/

NodeItem::~NodeItem(void) {
}

QRectF NodeItem::boundingRect() const {
  return QRectF(0, 0, 80, 100);
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->drawRect(0, 0, 80, 100);
}