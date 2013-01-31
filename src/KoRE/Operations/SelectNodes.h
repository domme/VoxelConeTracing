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

#ifndef KORE_SELECT_NODES_H_
#define KORE_SELECT_NODES_H_

#include "KoRE/Operations/Operation.h"
#include "KoRE/SceneManager.h"
#include "KoRE/SceneNode.h"

namespace kore {
  enum SortingType {
    SORT_NODES_SIMPLE,
    SORT_NODES_COMPONENT,
    SORT_NODES_Z
  };
  class SelectNodes : public Operation {
  public:
    SelectNodes(const uint tag,
                const SceneNodePtr& root,
                SortingType type = SORT_NODES_SIMPLE);
    SelectNodes(const std::string& name,
                const SceneNodePtr& root,
                SortingType type = SORT_NODES_SIMPLE,
                bool partial_match = true);
    virtual ~SelectNodes(void);
    virtual void execute(void);
    virtual void update(void);
    virtual void reset(void);
  private:
    void append(const SceneNodePtr& root, const uint tag);
    void append(const SceneNodePtr& root, std::string& name);
    std::vector<SceneNodePtr> _nodes;
  };
};

#endif  // KORE_SELECT_NODES_H_
