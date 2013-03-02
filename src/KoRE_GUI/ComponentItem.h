/*
  Copyright (c) 2012 The KoRE Project

  This file is part of KoRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoRE.  If not, see <http://www.gnu.org/licenses/>.
*/

/************************************************************************/
/* \author Dominik Ospelt                                               */
/************************************************************************/

#ifndef COMPONENTITEM_H_
#define COMPONENTITEM_H_

#include <QGraphicsItem>
#include "KoRE_GUI/ShaderDataItem.h"
#include "KoRE/Components/SceneNodeComponent.h"

namespace koregui {
  class ComponentItem : public QGraphicsItem {
  public:
    ComponentItem(kore::SceneNodeComponent* component,
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
    kore::SceneNodeComponent* _component;
    std::vector<ShaderDataItem*> _shaderDataItems;

    uint _componentwidth;
    uint _componentheight;
    bool _expanded;
  };
}
#endif  // COMPONENTITEM_H_