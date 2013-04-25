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

#ifndef SHADERINPUTITEM_H_
#define SHADERINPUTITEM_H_

#include <QGraphicsItem>
#include "KoRE/ShaderInput.h"
#include "KoRE_GUI/ShaderPassItem.h"

namespace koregui {
  class BindPathItem;
  class ShaderInputItem : public QGraphicsItem {
  public:
    ShaderInputItem(const kore::ShaderInput* input, ShaderPassItem* pass, QGraphicsItem* parent = 0);
    ~ShaderInputItem(void);
    inline const kore::ShaderInput* getInput(void) {return _input;}
    bool checkInput(BindPathItem* binding);
    inline ShaderPassItem* getShaderPass(void) {return _pass;}
    void setBinding(BindPathItem* binding);
    void reset();

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    QRectF boundingRect() const;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget);

  private:
    const kore::ShaderInput* _input;
    ShaderPassItem* _pass;
    BindPathItem* _binding;
    bool _mouseover;
  };
}
#endif  // SHADERINPUTITEM_H_
