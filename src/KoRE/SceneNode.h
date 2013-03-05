/*
  Copyright © 2012 The KoRE Project

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

#ifndef CORE_INCLUDE_CORE_SCENENODE_H_
#define CORE_INCLUDE_CORE_SCENENODE_H_

#include <vector>
#include <string>
#include "KoRE/Common.h"
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/Components/Transform.h"

namespace kore {
  enum ETransfpomSpace {
    SPACE_LOCAL,
    SPACE_WORLD
  };
    typedef std::shared_ptr<SceneNode> SceneNodePtr;
  class SceneNode {
    friend class SceneLoader;
  public:
    SceneNode(void);
    virtual ~SceneNode(void);
    // bool isCompatibleWith(const SceneNode& otherNode) const;
    // bool isCompatibleWith(const SceneNode& otherNode,
    //                       EComponentType types) const;
    const SceneNodePtr& getParent(void) const;
    const std::vector<SceneNodePtr>& getChildren() const;
    const std::vector<SceneNodeComponentPtr> getComponents() const;
    const TransformPtr getTransform(void) const;
    const SceneNodeComponentPtr getComponent(EComponentType type) const;
    const uint64 getID(void) const;
    const uint getTag(void) const;
    const std::string getName(void) const;
    void getSceneNodesByTag(const uint tag,
                            std::vector<SceneNodePtr>& vNodes);
    void getSceneNodesByName(const std::string& name,
                             std::vector<SceneNodePtr>& vNodes);
    void getSceneNodesByComponent(const EComponentType componentType,
                                  std::vector<SceneNodePtr>& vNodes);
    void setParent(const SceneNodePtr& parent);
    void addChild(const SceneNodePtr& child);
    void addComponent(const SceneNodeComponentPtr& component);
    void setTag(const std::string& tagname);
    void setName(const std::string& name);

    const bool needsUpdate(void) const;
    void update(void);

    void translate(const glm::vec3& dir,
                   const ETransfpomSpace relativeTo = SPACE_LOCAL);

    void setTranslation(const glm::vec3& position,
                        const ETransfpomSpace relativeTo = SPACE_LOCAL);

    void rotate(const GLfloat& angle, const glm::vec3& axis, 
                const ETransfpomSpace relativeTo = SPACE_LOCAL);

    void scale(const glm::vec3& dim,
               const ETransfpomSpace relativeTo = SPACE_LOCAL);

    void setOrientation(const glm::vec3& v3Side,
                        const glm::vec3& v3Up,
                        const glm::vec3& v3Forward,
                        const ETransfpomSpace relativeTo = SPACE_LOCAL);
  private:
    uint64 _id;
    uint _tag;
    std::string _name;
    SceneNodePtr _parent;
    std::vector<SceneNodePtr> _children;
    std::vector<SceneNodeComponentPtr> _components;
    bool _dirty;  // if node has to be updated;
    TransformPtr _transform;
  };
};
#endif  // CORE_INCLUDE_CORE_SCENENODE_H_
