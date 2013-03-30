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

#ifndef SHADEREDITOR_H_
#define SHADEREDITOR_H_

#include <QWidget>
#include "ui_ShaderEditor.h"

#include "KoRE/ShaderProgram.h"
#include "KoRE_GUI/ShaderProgramItem.h"

namespace koregui {
  class ShaderEditor : public QWidget
  {
    Q_OBJECT

  public:
    ShaderEditor(ShaderProgramItem* pass, QWidget *parent = 0);
    ~ShaderEditor();

    public slots:
      void addNewShaderProgram(void);
      void shaderProgramChanged(const QString & name);
      void refresh(void);

  private:
    Ui::ShaderEditor ui;
    kore::ShaderProgram* _currentprogram;
    ShaderProgramItem* _currentitem;
  };
}

#endif  // SHADEREDITOR_H_
