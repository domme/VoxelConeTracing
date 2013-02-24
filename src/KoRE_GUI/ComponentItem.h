#ifndef COMPONENTITEM_H_
#define COMPONENTITEM_H_

#include <QGraphicsItem>
#include "KoRE_GUI/ShaderDataItem.h"
#include "KoRE/Components/SceneNodeComponent.h"

namespace koregui {
  class ComponentItem : public QGraphicsItem {
  public:
    ComponentItem(kore::SceneNodeComponentPtr component,
                  QGraphicsItem* parent = 0);
    ~ComponentItem(void);

    void refresh(void);
    inline int getHeight(void) {return _componentheight;};
    inline int getWidth(void) {return _componentwidth;};
    inline bool isExpanded(void) {return _expanded;};

  protected:
    QRectF boundingRect() const;
    void paint(QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

  private:
    kore::SceneNodeComponentPtr _component;
    std::vector<ShaderDataItem*> _shaderDataItems;

    uint _componentwidth;
    uint _componentheight;
    bool _expanded;
  };
}
#endif  // COMPONENTITEM_H_