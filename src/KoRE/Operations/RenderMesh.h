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

#ifndef CORE_INCLUDE_CORE_RENDERMESHOP_H_
#define CORE_INCLUDE_CORE_RENDERMESHOP_H_

#include "KoRE/Operations/Operation.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/ShaderProgram.h"

namespace kore {
  class RenderMesh: public Operation {
  public:
    explicit RenderMesh(void);
    explicit RenderMesh(const kore::MeshComponent* mesh);

    virtual ~RenderMesh(void);
    virtual void update(void);
    virtual void reset(void);
    virtual bool isValid(void) const;
    virtual bool dependsOn(const void* thing) const;

    void connect(const kore::MeshComponent* mesh);

    const kore::MeshComponent* getMesh() const;
    void setMesh(const kore::MeshComponent* mesh);

  private:
    const kore::MeshComponent* _meshComponent;
    const kore::ShaderProgram* _shaderProgram;

    virtual void doExecute(void) const;
  };
};
#endif  // CORE_INCLUDE_CORE_RENDERMESHOP_H_
