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

#ifndef NODEITEM_H
#define NODEITEM_H

#include <vector>
#include <QGraphicsItem>
#include "KoRE/SceneNode.h"
#include "KoRE_GUI/ComponentItem.h"
#include "KoRE_GUI/NodePathItem.h"

namespace koregui {
  class NodeItem : public QGraphicsItem {
  public:
    NodeItem(kore::SceneNode* sceneNode, QGraphicsItem* parent = 0);
    ~NodeItem(void);

    void refresh(void);
    QRectF boundingRect() const;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget);

    inline int getHeight(void) {return _nodeheight;};
    inline int getWidth(void) {return _nodewidth;};
    inline kore::SceneNode* getSceneNode(void) {return _sceneNode;};

  private:
    kore::SceneNode* _sceneNode;
    uint _nodeheight;
    uint _nodewidth;

    std::vector<ComponentItem*> _componentItems;
    NodePathItem* _parentPath;
    std::vector<NodePathItem*> _childrenPaths;
    QGraphicsScene* _scene;
  };
}
#endif  // NODEITEM_H
