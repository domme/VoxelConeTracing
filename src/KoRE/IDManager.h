/*
  Copyright(c) 2012 The KoRE Project

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

#ifndef SRC_KORE_IDMANAGER_H_
#define SRC_KORE_IDMANAGER_H_

#include "KoRE/Common.h"

namespace kore {
  class Mesh;
  class Texture;
  class FrameBuffer;
  class Material;
  class SceneNode;
  class ShaderProgram;

  class IDManager {
    friend class ProjectLoader;
  
  public:
    static IDManager *getInstance(void);
    ~IDManager(void);
    /*! \brief Generates a unique ID
     *  \return a unique ID
     */
    inline uint64 genID() {return ++_counter;}
    /*! \brief
    */
    const std::string& getURL(uint64 id) const;
    /*! \brief
    */
    const uint64 getID(const std::string& url) const;
    /*! \brief
    */
    void registerURL(uint64 id, const std::string& url);

    /*! /brief
    */
    std::string genURL(const std::string& name,
                       const std::string& filepath = "",
                       const uint fileIndex = 0) const;
    
    // TODO(dlazarek & co): Append new genURL-versions for new object types

  private:
    IDManager(void);
    uint64 _counter;
    std::string _internalPathName;
    std::string _invalidURL;
    std::map<uint64, std::string> _mapURL;
  };
}
#endif  // SRC_KORE_IDMANAGER_H_
