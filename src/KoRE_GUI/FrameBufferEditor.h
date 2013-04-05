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

#ifndef FRAMEBUFFEREDITOR_H_
#define FRAMEBUFFEREDITOR_H_

#include <QWidget>

#include "ui_FrameBufferEditor.h"

#include "KoRE/FrameBuffer.h"
#include "KoRE_GUI/FrameBufferItem.h"

namespace koregui{
  class FrameBufferEditor : public QWidget
  {
      Q_OBJECT

  public:
      FrameBufferEditor(FrameBufferItem* stage, QWidget *parent = 0);
      ~FrameBufferEditor();

      void setFramebuffer(const QString& name);

  public slots:
    void addNewFramebuffer(void);
    void framebufferChanged(int index);
    //void applySettings(void);
    void addTarget(void);
    void refresh(void);

  private:
      Ui::FrameBufferEditor ui;
      kore::FrameBuffer* _currentbuffer;
      FrameBufferItem* _currentitem;
  };
}

#endif // FRAMEBUFFEREDITOR_H_
