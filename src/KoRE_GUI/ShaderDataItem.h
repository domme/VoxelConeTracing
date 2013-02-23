#ifndef SHADERNODEITEM_H_
#define SHADERNODEITEM_H_

#include <QGraphicsItem>
#include "KoRE/ShaderData.h"

namespace koregui {
  class ShaderDataItem : public QGraphicsItem {
  public:
    ShaderDataItem(kore::ShaderData data, QGraphicsItem* parent = 0);
    ~ShaderDataItem(void);

    void refresh(void);
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  private:
    kore::ShaderData _data;
  };
}
#endif  // SHADERNODEITEM_H_
