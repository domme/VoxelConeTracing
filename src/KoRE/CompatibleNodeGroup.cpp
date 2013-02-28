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

#include "KoRE/CompatibleNodeGroup.h"
#include "KoRE/SceneManager.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Log.h"


kore::CompatibleNodeGroup::CompatibleNodeGroup() {
  _id = kore::SceneManager::getInstance()->createID();
}

kore::CompatibleNodeGroup::~CompatibleNodeGroup() {
}

void kore::CompatibleNodeGroup::addSceneNode(const SceneNodePtr node) {
  // check if node isn't already in group
  for (uint i = 0; i < _nodes.size(); i++) {
    if(node->getID() == _nodes[i]->getID()) {
      kore::Log::getInstance()
        ->write("[WARNING] Node '%s' already exists in Group '%s'\n",
                node->getName().c_str(),
                _name.c_str());
      return;
    }
  }
  // simply add it
  _nodes.push_back(node);
  refresh();
}

void kore::CompatibleNodeGroup::removeSceneNode(const SceneNodePtr node) {
  std::vector<SceneNodePtr>::iterator it = _nodes.begin();
  while(it < _nodes.end()) {
    if(node->getID() == it->get()->getID()) {
      _nodes.erase(it);
      refresh();
      return;
    }
    it++;
  }
  // log if node was not in group
  kore::Log::getInstance()
    ->write("[WARNING] Node '%s' does not exist in Group '%s'\n",
    node->getName().c_str(),
    _name.c_str());
}



bool kore::CompatibleNodeGroup::isCompatible(kore::EComponentType type) const{
  // TODO(dospelt)
  if (_compatibles.find(type) != _compatibles.end()) {
    //for (std::map<kore::EComponentType, kore::CompatibleNodeGroup::ShaderDataProxy>::iterator it =_compatibles.begin(); it < _compatibles.end(); it++) {
    _compatibles.begin();
    //}
  }
  return false;
}

bool kore::CompatibleNodeGroup::isCompatible(kore::EComponentType type,
                                           const std::string& sdataname) const{
  // TODO(dospelt)
  if (_compatibles.find(type) != _compatibles.end()) {
    //if (_compatibles[type]){
    }
  return false;
}

const std::vector<kore::SceneNodeComponentPtr>
  kore::CompatibleNodeGroup::getComponents(kore::EComponentType type) const {
  std::vector<kore::SceneNodeComponentPtr> components;
  for (uint i = 0; i < _nodes.size(); i++) {
    if(_nodes[i]->getComponent(type) != NULL
       && isCompatible(type)) {
      components.push_back(_nodes[i]->getComponent(type));
    }
  }
  return components;
}

const std::vector<const kore::ShaderData*>
  kore::CompatibleNodeGroup::getShaderData(EComponentType type,
                                          const std::string& proxyname) const {
  std::vector<const kore::ShaderData*> sdata;
  for (uint i = 0; i < _nodes.size(); i++) {
    if(_nodes[i]->getComponent(type) != NULL
       && _nodes[i]->getComponent(type)->getShaderData(proxyname) != 0
       && isCompatible(type, proxyname)) {
      sdata.push_back(_nodes[i]->getComponent(type)->getShaderData(proxyname));
    }
  }
  return sdata;
}

void kore::CompatibleNodeGroup::refresh() {
  _compatibles.clear();
  for (uint i = 0; i < _nodes.size(); i++) {
    // TODO(dospelt)
    //for (uint j = 0)
  }
}
