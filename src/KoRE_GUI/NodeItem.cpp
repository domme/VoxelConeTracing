#include "KoRE_GUI/NodeItem.h"
#include <QPainter>
#include <QStaticText>
#include "KoRE_GUI/ShaderDataItem.h"

koregui::NodeItem::NodeItem(kore::SceneNodePtr sceneNode, QGraphicsItem* parent)
                            : _sceneNode(sceneNode),
                              QGraphicsItem(parent) {
  setFlag(QGraphicsItem::ItemIsMovable, true);
  refresh();
}

koregui::NodeItem::~NodeItem(void) {
}

void koregui::NodeItem::refresh(void) {
  _nodeheight = 0;
  _nodeheight += 40; // node name
  std::vector<kore::SceneNodeComponentPtr> components = _sceneNode->getComponents();
   for (uint i = 0; i<components.size(); i++) {
     
     std::vector<kore::ShaderData>sdata = components[i]->getShaderData();
     for (uint j = 0; j < sdata.size(); j++) {
       ShaderDataItem* dataitem =  new ShaderDataItem(sdata[j], this);
       dataitem->setPos(0, 10 + _nodeheight + j * 30 );
     }
     _nodeheight += components[i]->getShaderData().size() * 30;
   }
   _nodeheight += 20; // lower cap
  _nodewidth = 200;
}

QRectF koregui::NodeItem::boundingRect() const {
  return QRectF(0, 0, _nodewidth, _nodeheight);
}

void koregui::NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  uint currentheight = 0;
  QBrush b;
  QPen p;
  p.setWidth(2);
  p.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
  p.setStyle(Qt::PenStyle::NoPen);
  b.setColor(QColor(44,44,44));
  b.setStyle(Qt::BrushStyle::SolidPattern);
  painter->setPen(p);
  painter->setBrush(b);
  painter->drawRoundedRect(QRect(0, 0, _nodewidth, _nodeheight), 10, 10);

  QFont font("Arial");
  font.setBold(true);
  font.setPointSize(12);
  painter->setFont(font);

  QStaticText text;
  text.setText(_sceneNode->getName().c_str());
  p.setColor(QColor(255,255,255));
  p.setStyle(Qt::PenStyle::SolidLine);
  painter->setPen(p);
  painter->drawStaticText(10,10, text);
  p.setStyle(Qt::PenStyle::NoPen);
  painter->setPen(p);

  currentheight+= 40;
  std::vector<kore::SceneNodeComponentPtr> components = _sceneNode->getComponents();
  for (uint i = 0; i<components.size(); i++) {

    std::vector<kore::ShaderData>sdata = components[i]->getShaderData();
    (i%2)?b.setColor(QColor(44,44,44)):b.setColor(QColor(55,55,55));
    painter->setBrush(b);
    painter->drawRect(0, currentheight, 200, sdata.size() * 30);

    switch (components[i]->getType()) {
    case kore::COMPONENT_TRANSFORM:
      b.setColor(QColor(35,203,173));
      break;
    case kore::COMPONENT_MESH:
      b.setColor(QColor(252,210,89));
      break;
    case kore::COMPONENT_CAMERA:
      b.setColor(QColor(199,89,214));
      break;
    case kore::COMPONENT_LIGHT:
      b.setColor(QColor(250,123,28));
      break;
    case kore::COMPONENT_UNKNOWN:
    default:
      b.setColor(Qt::GlobalColor::red);
    }

    // draw component color
    painter->setBrush(b);
    painter->drawRect(0, currentheight, 10, sdata.size() * 30);

    currentheight += sdata.size() * 30;
  }
}
