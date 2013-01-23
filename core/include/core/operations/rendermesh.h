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

#include "core/operations/operation.h"
#include "core/components/mesh.h"
#include "core/components/camera.h"
#include "core/shader.h"

namespace kore {
  class RenderMesh: public Operation {
  public:
    explicit RenderMesh(void);
    explicit RenderMesh(const kore::MeshPtr& mesh,
                          const kore::CameraPtr& camera,
                          const kore::ShaderPtr& shader);

    virtual ~RenderMesh(void);
    virtual void execute(void);
    virtual void update(void);
    virtual void reset(void);

    const kore::MeshPtr& getMesh() const;
    void setMesh(const kore::MeshPtr& mesh);

    const kore::CameraPtr& getCamera() const;
    void setCamera(const kore::CameraPtr& camera);

    const kore::ShaderPtr& getShader() const;
    void setShader(const kore::ShaderPtr& shader);

  private:
    kore::MeshPtr _mesh;

    kore::CameraPtr _camera;
    kore::ShaderPtr _shader;
  };
  typedef std::shared_ptr<RenderMesh> RenderMeshOpPtr;
};
#endif  // CORE_INCLUDE_CORE_RENDERMESHOP_H_
