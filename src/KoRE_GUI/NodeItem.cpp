#include "KoRE_GUI/NodeItem.h"
#include <QPainter>
#include <QStaticText>

koregui::NodeItem::NodeItem(kore::SceneNodePtr sceneNode,
                            QGraphicsItem* parent)
                            : _sceneNode(sceneNode),
                              QGraphicsItem(parent) {

  setFlag(QGraphicsItem::ItemIsMovable, true);
  //setFlag(QGraphicsItem::ItemIsSelectable, true);
  std::vector<kore::SceneNodeComponentPtr> components =
    _sceneNode->getComponents();
    for (uint i = 0; i<components.size(); i++) {
      ComponentItem* compItem = new ComponentItem(components[i], this);
      _componentItems.push_back(compItem);
   }
  refresh();
}

koregui::NodeItem::~NodeItem(void) {
}

void koregui::NodeItem::refresh(void) {
  prepareGeometryChange();
  _nodeheight = 0;
  _nodeheight += 40; // place for node name
  for (uint i = 0; i<_componentItems.size(); i++) {
    _componentItems[i]->setPos(0, _nodeheight);
    _nodeheight += _componentItems[i]->getHeight();
  }
  _nodeheight += 20; // lower cap
  _nodewidth = 200;
}

QRectF koregui::NodeItem::boundingRect() const {
  return QRectF(0, 0, _nodewidth, _nodeheight);
}

void koregui::NodeItem::paint(QPainter* painter,
                              const QStyleOptionGraphicsItem* option,
                              QWidget* widget) {
  QBrush b;
  QPen p;
  QFont font("Arial");
  QStaticText text;

  p.setStyle(Qt::PenStyle::NoPen);
  b.setColor(QColor(44,44,44));
  b.setStyle(Qt::BrushStyle::SolidPattern);
  painter->setPen(p);
  painter->setBrush(b);
  painter->drawRoundedRect(QRect(0, 0, _nodewidth, _nodeheight), 10, 10);

  font.setBold(true);
  font.setPointSize(12);
  painter->setFont(font);

  text.setText(_sceneNode->getName().c_str());
  p.setColor(QColor(255,255,255));
  p.setStyle(Qt::PenStyle::SolidLine);
  painter->setPen(p);
  painter->drawStaticText(10,10, text);
}
