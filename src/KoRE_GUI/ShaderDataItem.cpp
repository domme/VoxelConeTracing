#include "KoRE_GUI/ShaderDataItem.h"
#include <QPainter>
#include <QCursor>
#include <QStaticText>

koregui::ShaderDataItem::ShaderDataItem(kore::ShaderData data,
                                        QGraphicsItem* parent)
                                      : _data(data),
                                        QGraphicsItem(parent) {
  setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
}

koregui::ShaderDataItem::~ShaderDataItem(void) {
}

void koregui::ShaderDataItem::refresh(void) {

}

QRectF koregui::ShaderDataItem::boundingRect() const {
  return QRectF(0, 0, 220, 15);
}

void koregui::ShaderDataItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  QStaticText t(_data.name.c_str());
  QFont font("Consolas");
  //font.setBold(true);
  font.setPointSize(9);
  painter->setFont(font);

  QPen p;
  p.setWidth(2);
  p.setJoinStyle(Qt::PenJoinStyle::RoundJoin);

  QBrush b;
  b.setColor(Qt::GlobalColor::yellow);
  b.setStyle(Qt::BrushStyle::SolidPattern);
  painter->setBrush(b);
  painter->drawRect(195, 0, 10, 10);

  
  p.setColor(QColor(200,200,200));
  painter->setPen(p);
  painter->drawStaticText(20,0,t);
}
