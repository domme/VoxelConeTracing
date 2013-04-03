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

#ifndef KORE_SRC_KORE_COMPONENTS_MATERIAL_H_
#define KORE_SRC_KORE_COMPONENTS_MATERIAL_H_

#include "KoRE/Common.h"
#include "KoRE/Components/SceneNodeComponent.h"
#include "KoRE/ShaderData.h"
#include "KoRE/Events.h"
#include "KoRE/BaseResource.h"

namespace kore {
  /*! This class stores generic heap-pointers to material-values so that many
      MaterialComponents can share the same values */
  class Material : public BaseResource {
    friend class SceneLoader;

  public:
    explicit Material();
    virtual ~Material();

    /*! \brief Called from the MaterialComponent, if a new ShaderData is added.
     *         All MaterialComponents, this material is attached to are
     *         notified (including the caller).
    */
    void addValue(ShaderData* shaderData);

       /*! \brief Appends a new value to the Material. The method creates and
               appends a new ShaderData-instance to the component's shaderData-
               list with the provided information.
        \param dataType The openGL-datatype of this value (e.g. GL_FLOAT3).
        \param name The (unique) name of the material-parameter
                    (e.g. "diffuse reflectivity").
        \param value The value of the material-parameter. This has to be
                     a pointer to an heap-allocated object. */
    void addValue(const std::string& name,
                  const GLuint dataType,
                  void* value);

       /*! \brief Sets the value of a material-parameter.
        \param name The name of the parameter (e.g. "diffuse reflectivity").
        \param value A pointer to the memory with the new value. */
    template<typename ValueT>
    void setValue(const std::string& name,
                  const GLuint dataType,
                  const ValueT& value);

    /*! \brief Called from the MaterialComponent, if a ShaderData is removed.
    *           All MaterialComponents, this material is attached to are
    *           notified (including the caller).
    */
    void removeValue(ShaderData* shaderData);

    inline const std::vector<ShaderData>& getValues() const {return _values;}
    inline const std::string& getName() const {return _name;}

    /*! \brief Returns true, if there is a material-parameter with the provided
               name. */
    bool hasValue(const std::string& name);

    Delegate1Param<ShaderData*>& getAddEvent() {return _eventDataAdded;}
    Delegate1Param<ShaderData*>& getRemoveEvent() {return _eventDataRemoved;}
  private:
    std::string _name; // Note: the sceneLoader will assign a unique name.
    std::vector<ShaderData> _values;
    Delegate1Param<ShaderData*> _eventDataAdded;
    Delegate1Param<ShaderData*> _eventDataRemoved;

    uint getShaderDataIdxForValue(const void* data);
    ShaderData* getShaderDataForValue(const void* data);
    bool containsDataPointer(const void* data);
    ShaderData* getValue(const std::string& name);
  };
}

#endif  // KORE_SRC_KORE_COMPONENTS_MATERIAL_H_
