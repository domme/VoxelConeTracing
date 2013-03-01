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

#ifndef SRC_KORE_COMPATIBLENODEGROUP_H_
#define SRC_KORE_COMPATIBLENODEGROUP_H_
#include <vector>
#include <map>
#include "KoRE/Components/SceneNodeComponent.h"
namespace kore {
  class CompatibleNodeGroup {
  public:
    CompatibleNodeGroup();
    ~CompatibleNodeGroup();

    /*! \brief Add a SceneNode to the group and refresh compatibility list.
               Ignore if SceneNode is already part of the group.
        \param node The SceneNode to add.
    */
    void addSceneNode(const SceneNodePtr node);

    /*! \brief Remove SceneNode from the group and refresh compatibility list.
               Ignore if SceneNode is not part of the group.
        \param node The SceneNode to remove.
    */
    void removeSceneNode(const SceneNodePtr node);

    /*! \brief checks if a component type is compatible with the group.
        \param type ComponentType to check.
    */
    bool isCompatible(EComponentType type) const;

    /*! \brief checks if ShaderData is compatible with this group.
        \param type ComponentType to check.
        \param sdataname Name of the ShaderData to check.
    */
    bool isCompatible(EComponentType type, const std::string& sdataname) const;

    /*! \brief Gets all compatible components of the specified type
        \param type The type the of SceneNodeComponents.
        \return Vector of SceneNodeComponents.
    */
    const std::vector<SceneNodeComponentPtr>
      getComponents(EComponentType type) const;

    /*! \brief Gets all compatible ShaderData
        \param proxyname name of the ShaderData
        \param type The type the of SceneNodeComponents.
        \return Vector of ShaderData.
    */
    const std::vector<const ShaderData*>
      getShaderData(EComponentType type, const std::string& proxyname) const;

  private:
    /*! \brief Recreates internal map of compatible Components and ShaderData.
    */
    void refresh();

    typedef std::map<std::string, bool> ShaderDataProxy;
    typedef std::map<EComponentType, ShaderDataProxy> ComponentProxy;
    
    std::string _name;
    uint64 _id;
    ComponentProxy _dataCompatibles;
    std::map<EComponentType, bool> _typeCompatibles;
    std::vector<SceneNodePtr> _nodes;
  };
}
#endif  // SRC_KORE_COMPATIBLENODEGROUP_H_
