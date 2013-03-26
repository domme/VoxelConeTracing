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
#include <QFileDialog>
#include "KoRE/ResourceManager.h"

koregui::ResourceViewer::ResourceViewer(QWidget *parent)
                                      : _layout(this),
                                        QWidget(parent) {
  setWindowTitle("Resource Manager");

  // deprecated soon^^
  const kore::FrameBuffer* frabuf = kore::FrameBuffer::BACKBUFFER;
  setLayout(&_layout);
  _layout.addWidget(&_tabs);
  _layout.addWidget(&_addButton);
  _addButton.setText("ADD");
  connect(&_addButton, SIGNAL(clicked()), this, SLOT(useBrowser()));

  std::vector<kore::Mesh*> meshlist
    = kore::ResourceManager::getInstance()->getMeshes();
  for (uint i = 0; i < meshlist.size(); i++) {
    _meshes.addItem(meshlist[i]->getName().c_str());
  }
  _tabs.addTab(&_meshes, "Meshes");
  _meshes.setObjectName("Meshes");

  std::vector<kore::Texture*> texlist
    = kore::ResourceManager::getInstance()->getTextures();
  for (uint i = 0; i < texlist.size(); i++) {
    _textures.addItem(texlist[i]->getName().c_str());
  }
  _tabs.addTab(&_textures, "Textures");
  _textures.setObjectName("Textures");

  std::vector<kore::ShaderProgram*> programlist
    = kore::ResourceManager::getInstance()->getShaderPrograms();
  for (uint i = 0; i < programlist.size(); i++) {
    _programs.addItem(programlist[i]->getName().c_str());
  }
  _tabs.addTab(&_programs, "ShaderPrograms");
  _programs.setObjectName("ShaderPrograms");

  std::vector<kore::FrameBuffer*> bufferlist
    = kore::ResourceManager::getInstance()->getFramebuffers();
  for (uint i = 0; i < bufferlist.size(); i++) {
    _framebuffers.addItem(bufferlist[i]->getName().c_str());
  }
  _tabs.addTab(&_framebuffers, "FameBuffer");
  _framebuffers.setObjectName("FrameBuffer");

  resize(640, 480);
}

koregui::ResourceViewer::~ResourceViewer() {
}

void koregui::ResourceViewer::update(void) {
}

void koregui::ResourceViewer::keyPressEvent(QKeyEvent* event) {
}

void koregui::ResourceViewer::useBrowser(void) {
  if (_tabs.currentWidget()->objectName() == "Meshes") {
    QStringList fileNames
      = QFileDialog::getOpenFileNames(this,
                                      tr("Load Mesh"),
                                      "./assets/",
                                      tr("Mesh Data (*.dae)"));
    for(uint i = 0; i < fileNames.size(); i++) {
      // TODO (dospelt) check if Mesh is already loaded
      kore::ResourceManager::getInstance()
        ->loadResources(fileNames[i].toStdString());
    }
  }
  if (_tabs.currentWidget()->objectName() == "Textures") {
    QStringList fileNames
      = QFileDialog::getOpenFileNames(this,
                                      tr("Load Texture"),
                                      "./assets/",
                                      tr("Image (*.png)"));
    for(uint i = 0; i < fileNames.size(); i++) {
      kore::ResourceManager::getInstance()->loadTexture(fileNames[i].toStdString());
    }
  }
  //"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
}
