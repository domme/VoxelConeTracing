#include "NodeItem.h"
#include <QPainter>
#include <QCursor>
#include <QStaticText>

koregui::NodeItem::NodeItem(kore::SceneNode* sceneNode, QGraphicsItem* parent) : QGraphicsItem(parent) {
  //setCursor(QCursor(Qt::OpenHandCursor));
  setFlag(QGraphicsItem::ItemIsMovable, true);
  //QGraphicsItem::ItemIsSelectable
  _sceneNode = sceneNode;
  refresh();
}

koregui::NodeItem::~NodeItem(void) {
}

void koregui::NodeItem::refresh(void) {
  _nodeheight = 70 + (_sceneNode->_components.size() * 30);
}

QRectF koregui::NodeItem::boundingRect() const {
  return QRectF(0, 0, 100, _nodeheight);
}

void koregui::NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  QBrush b;
  b.setColor(Qt::GlobalColor::lightGray);
  b.setStyle(Qt::BrushStyle::SolidPattern);
  //b.setStyle(Qt::BrushStyle::BDiagPattern);
  painter->setBrush(b);
  painter->drawRoundedRect(QRect(0, 0, 100, _nodeheight), 10, 10);
  
  QFont font("Times");
  //font.setBold(true);
  font.setPointSize(12);
  QStaticText t(_sceneNode->getName().c_str());
  painter->setFont(font);
  painter->drawStaticText(10,10, t);

  b.setStyle(Qt::BrushStyle::BDiagPattern);

  for (uint i = 0; i<_sceneNode->_components.size(); i++) {
    switch (_sceneNode->_components[i]->getType()) {
    case kore::COMPONENT_TRANSFORM:
      b.setColor(Qt::GlobalColor::blue);
      break;
    case kore::COMPONENT_MESH:
      b.setColor(Qt::GlobalColor::yellow);
      break;
    case kore::COMPONENT_CAMERA:
      b.setColor(Qt::GlobalColor::green);
      break;
    case kore::COMPONENT_LIGHT:
      b.setColor(Qt::GlobalColor::darkCyan);
      break;
    case kore::COMPONENT_UNKNOWN:
    default:
      b.setColor(Qt::GlobalColor::red);
    }
    painter->setBrush(b);
    painter->drawRect(0, 60 + (i * 30), 100, 30);
  }
}
