#include "NodeItem.h"
#include <QPainter>
#include <QCursor>
#include <QStaticText>

koregui::NodeItem::NodeItem(kore::SceneNode* sceneNode, QGraphicsItem* parent) : QGraphicsItem(parent) {
  //setCursor(QCursor(Qt::OpenHandCursor));
  setFlag(QGraphicsItem::ItemIsMovable, true);
  //QGraphicsItem::ItemIsSelectable
  _sceneNode = sceneNode;
}

koregui::NodeItem::~NodeItem(void) {
}

QRectF koregui::NodeItem::boundingRect() const {
  return QRectF(0, 0, 80, 100);
}

void koregui::NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  
  QBrush b;
  b.setColor(Qt::GlobalColor::lightGray);
  b.setStyle(Qt::BrushStyle::BDiagPattern);
  painter->setBrush(b);
  painter->drawRoundedRect(QRect(0, 0, 80, 100), 10, 10);
  
  QFont font("Times");
  //font.setBold(true);
  //font.setPointSize(14);
  QStaticText t(_sceneNode->getName().c_str());
  painter->setFont(font);
  painter->drawStaticText(10,10, t);

  //for (uint i = 0; i<_sceneNode->get)
}