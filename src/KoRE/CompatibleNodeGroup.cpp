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
#include "KoRE/IDManager.h"


kore::CompatibleNodeGroup::CompatibleNodeGroup() {
  _id = kore::IDManager::getInstance()->genID();
}

kore::CompatibleNodeGroup::~CompatibleNodeGroup() {
  _typeCompatibles.clear();
  _dataCompatibles.clear();
  _nodes.clear();
}

void kore::CompatibleNodeGroup::addSceneNode(SceneNode* node) {
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

void kore::CompatibleNodeGroup::removeSceneNode(const SceneNode* node) {
  std::vector<SceneNode*>::iterator it = _nodes.begin();
  while(it < _nodes.end()) {
    if(node->getID() == (*it)->getID()) {
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

bool kore::CompatibleNodeGroup::isCompatible(kore::EComponentType type) const {
  if (_typeCompatibles.find(type) != _typeCompatibles.end()) {
    return _typeCompatibles.find(type)->second;
  }
  return false;
}

bool kore::CompatibleNodeGroup::isCompatible(kore::EComponentType type,
                                          const std::string& sdataname) const {
  if (_dataCompatibles.find(type) != _dataCompatibles.end()) {
    ShaderDataProxy attlist =_dataCompatibles.find(type)->second;
    if (attlist.find(sdataname) != attlist.end()) {
      return attlist.find(sdataname)->second;
    }
  }
  return false;
}

const std::vector<kore::SceneNodeComponent*>
  kore::CompatibleNodeGroup::getComponents(kore::EComponentType type) const {
  std::vector<kore::SceneNodeComponent*> components;
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
  _typeCompatibles.clear();
  _dataCompatibles.clear();
  if(_nodes.size() == 0) return;

  // set compatibility for first node
  std::vector<SceneNodeComponent*> components = _nodes[0]->getComponents();
  for (uint i = 0; i < components.size(); i++) {
    EComponentType type = components[i]->getType();
    _typeCompatibles[type] = true;
    std::vector<ShaderData> sdata = components[i]->getShaderData();
    for (uint j = 0; j < sdata.size(); j++) {
      std::string name = sdata[j].name;
      _dataCompatibles[type][name] = true;
    }
  }
  
  // add each node and update compatibility
  for (uint k = 1; k < _nodes.size(); k++) {
    std::vector<SceneNodeComponent*> components = _nodes[k]->getComponents();
    for (uint l = 0; l < components.size(); l++) {
      EComponentType type = components[l]->getType();
      // check if data is in map, otherwise add it false
      std::vector<ShaderData> sdata = components[l]->getShaderData();
      for (uint m = 0; m < sdata.size(); m++) {
        std::string name = sdata[m].name;
        if(_dataCompatibles[type].find(name) == _dataCompatibles[type].end()) {
          _dataCompatibles[type][name] = false;
        }
      }
      // check if we have data in map that is not in the current component
      for(ShaderDataProxy::iterator it = _dataCompatibles[type].begin();
          it != _dataCompatibles[type].end();
          it++) {
            if(components[l]->getShaderData(it->first) == NULL) {
              _dataCompatibles[type][it->first] = false;
            }
      }
      // check if component type is in map, otherwise add it false
      if(_typeCompatibles.find(type) == _typeCompatibles.end()) {
        _typeCompatibles[type] = false;
      }
    }
  }
}
