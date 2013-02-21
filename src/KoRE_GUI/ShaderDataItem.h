#ifndef SHADERNODEITEM_H_
#define SHADERNODEITEM_H_

#include <QGraphicsItem>
#include "KoRE/DataTypes.h"

namespace koregui {
  class ShaderDataItem : public QGraphicsItem {
  public:
    ShaderDataItem(QGraphicsItem* parent = 0);
    ~ShaderDataItem(void);

    void refresh(void);
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  private:
  };
}
#endif  // SHADERNODEITEM_H_
