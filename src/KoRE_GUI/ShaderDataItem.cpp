#include "KoRE_GUI/ShaderDataItem.h"
#include <QPainter>
#include <QCursor>
#include <QStaticText>

koregui::ShaderDataItem::ShaderDataItem(kore::ShaderData data,
                                        QGraphicsItem* parent)
                                      : _data(data),
                                        QGraphicsItem(parent) {
  //setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
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
  font.setPointSize(9);
  painter->setFont(font);

  QPen p;
  p.setColor(QColor(44,44,44));
  p.setWidth(2);

  QBrush b;
  b.setColor(Qt::GlobalColor::yellow);
  b.setStyle(Qt::BrushStyle::SolidPattern);
  painter->setBrush(b);
  painter->setPen(p);
  painter->drawRect(190, 0, 12, 12);

  p.setStyle(Qt::PenStyle::SolidLine);
  p.setColor(QColor(200,200,200));
  painter->setPen(p);
  painter->drawStaticText(20,0,t);
}
