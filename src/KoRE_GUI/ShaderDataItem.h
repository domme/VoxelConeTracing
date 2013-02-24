#ifndef SHADERDATAITEM_H_
#define SHADERDATAITEM_H_

#include <QGraphicsItem>
#include "KoRE/ShaderData.h"

namespace koregui {
  class ShaderDataItem : public QGraphicsItem {
  public:
    ShaderDataItem(kore::ShaderData* data, QGraphicsItem* parent = 0);
    ~ShaderDataItem(void);
    inline kore::ShaderData* getData(void) {return _data;};

    void refresh(void);
    QRectF boundingRect() const;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget);
  private:
    kore::ShaderData* _data;
  };
}
#endif  // SHADERDATAITEM_H_
