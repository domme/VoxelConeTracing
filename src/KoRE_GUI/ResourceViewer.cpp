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

  // deprecated soon^^
  const kore::FrameBuffer* frabuf = kore::FrameBuffer::BACKBUFFER;

  std::vector<kore::Mesh*> meshlist = kore::ResourceManager::getInstance()->getMeshes();
  for (uint i = 0; i < meshlist.size(); i++) {
    _meshes.addItem(meshlist[i]->getName().c_str());
  }
  addTab(&_meshes, "Meshes");

  std::vector<kore::Texture*> texlist = kore::ResourceManager::getInstance()->getTextures();
  for (uint i = 0; i < texlist.size(); i++) {
    _textures.addItem(texlist[i]->getName().c_str());
  }
  addTab(&_textures, "Textures");

  std::vector<kore::ShaderProgram*> programlist = kore::ResourceManager::getInstance()->getShaderPrograms();
  for (uint i = 0; i < programlist.size(); i++) {
    _programs.addItem(programlist[i]->getName().c_str());
  }
  addTab(&_programs,"ShaderPrograms");

  std::vector<kore::FrameBuffer*> bufferlist = kore::ResourceManager::getInstance()->getFramebuffers();
  for (uint i = 0; i < bufferlist.size(); i++) {
    _framebuffers.addItem(bufferlist[i]->getName().c_str());
  }
  addTab(&_framebuffers,"FameBuffer");

  resize(640, 480);
}

koregui::ResourceViewer::~ResourceViewer() {
}

void koregui::ResourceViewer::update(void) {
}

void koregui::ResourceViewer::keyPressEvent(QKeyEvent* event) {
}
