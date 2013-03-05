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

#ifndef SHADERITEM_H_
#define SHADERITEM_H_

#include <QGraphicsItem>
#include "KoRE/ShaderProgram.h"
#include "KoRE_GUI/ShaderInputItem.h"

namespace koregui {
  class ShaderItem : public QGraphicsItem {
  public:
    ShaderItem(const kore::ShaderProgram* shader,
                  QGraphicsItem* parent = 0);
    ~ShaderItem(void);

    void init(void);
    void refresh(void);
    inline int getHeight(void) {return _shaderheight;};
    inline int getWidth(void) {return _shaderwidth;};

    void contextMenu(QPoint pos);

  protected:
    QRectF boundingRect() const;
    void paint(QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

  private:
    kore::ShaderProgram* _shader;
    std::vector<ShaderInputItem*> _attributes;
    std::vector<ShaderInputItem*> _uniforms;
    std::vector<ShaderInputItem*> _sampler;
    uint _shaderwidth;
    uint _shaderheight;
  };
}
#endif  // SHADERITEM_H_
