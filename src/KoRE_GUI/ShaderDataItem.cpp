#include "KoRE_GUI/ShaderDataItem.h"
#include <QPainter>
#include <QCursor>
#include <QStaticText>
#include <QToolTip>

koregui::ShaderDataItem::ShaderDataItem(kore::ShaderData* data,
                                        QGraphicsItem* parent)
                                      : _data(data),
                                        QGraphicsItem(parent) {
  // setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
}

koregui::ShaderDataItem::~ShaderDataItem(void) {
}

void koregui::ShaderDataItem::refresh(void) {

}

QRectF koregui::ShaderDataItem::boundingRect() const {
  return QRectF(0, 0, 12, 12);
}

void koregui::ShaderDataItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  QPen p;
  p.setColor(QColor(44,44,44));
  p.setWidth(2);

  QBrush b;
  b.setColor(Qt::GlobalColor::yellow);
  b.setStyle(Qt::BrushStyle::SolidPattern);
  painter->setBrush(b);
  painter->setPen(p);
  painter->drawRect(0, 0, 12, 12);
}
