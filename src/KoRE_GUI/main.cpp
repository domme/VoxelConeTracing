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

#include <GL/glew.h>
#include <QApplication>
#include <QCommonStyle>

#include "KoRE/SceneManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"

#include "KoRE_GUI/KoRE_GUIStyle.h"
#include "KoRE_GUI/SceneViewer.h"
#include "KoRE_GUI/RenderViewer.h"
#include "KoRE_GUI/ResourceViewer.h"
#include "KoRE_GUI/GLWidget.h"

int main(int argc, char *argv[])
{

  // initialize Qt
  QApplication app(argc, argv);
  //app.setStyle(new koregui::KoRE_GUIStyle);

  // need of GL-Context
  GLWidget win;
  win.show();

  // now  other widgets
  //koregui::ResourceViewer resview;
  koregui::RenderViewer rview;
  //koregui::SceneViewer sview(&rview);
  //sview.showScene(kore::SceneManager::getInstance()->getRootNode());

  //sview.show();
  rview.show();
  //resview.show();

  return app.exec();
}
