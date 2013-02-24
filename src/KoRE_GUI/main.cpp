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

#include "KoRE/SceneManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"

#include "KoRE_GUI/SceneViewer.h"
#include "KoRE_GUI/RenderViewer.h"
#include "KoRE_GUI/GLWindow.h"

int main(int argc, char *argv[])
{

    // initialize Qt
    QApplication app(argc, argv);
    koregui::SceneViewer sview;
    sview.show();

    //koregui::RenderViewer rview;
    //rview.show();

    // do stuff
    // need of GL-Context
    GLWindow win;
    kore::ResourceManager::getInstance()->loadScene("./assets/meshes/TestEnv.dae");
    sview.showScene(kore::SceneManager::getInstance()->getRootNode());
    return app.exec();
}
