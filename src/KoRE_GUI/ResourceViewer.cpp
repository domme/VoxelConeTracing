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

#include "KoRE_GUI/ResourceViewer.h"
#include "KoRE/ResourceManager.h"

koregui::ResourceViewer::ResourceViewer(QWidget *parent)
                                      : QTabWidget(parent) {
  setWindowTitle("Resource Manager");
  std::vector<kore::Mesh*> meshlist = kore::ResourceManager::getInstance()->getMeshes();
  for (uint i = 0; i < meshlist.size(); i++) {
    _meshes.addItem(meshlist[i]->getName().c_str());
  }

  //_meshes
  addTab(&_meshes,"Meshes");

  // TODO
  //kore::ResourceManager::getInstance()->getShaderPrograms();
  this->addTab(new QWidget,"Textures");
  this->addTab(new QWidget,"ShaderPrograms");
  this->addTab(new QWidget,"FameBuffer");
}

koregui::ResourceViewer::~ResourceViewer() {
}

void koregui::ResourceViewer::update( void )
{

}

void koregui::ResourceViewer::keyPressEvent(QKeyEvent* event)
{

}
