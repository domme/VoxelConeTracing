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

#include <vector>

#include "core/rendermanager.h"
#include "core/log.h"

kore::RenderManager* kore::RenderManager::getInstance(void) {
  static kore::RenderManager theInstance;
  return &theInstance;
}

kore::RenderManager::RenderManager(void) {
}

kore::RenderManager::~RenderManager(void) {
}

void kore::RenderManager::renderMesh
(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Shader>& shader) {
    const std::vector<Attribute>& vAttributes = mesh->getAttributes();

    for (unsigned int i = 0; i < vAttributes.size(); ++i) {
        const Attribute& att = vAttributes[i];

        // Ger shader's attribute index
        GLuint uAttLoc = 0;

        glEnableVertexAttribArray(uAttLoc);
        glVertexAttribPointer(uAttLoc, att.size,
                              att.type, GL_FALSE,
                              att.type, att.data);
    }

    // shader->useShader();
    glDrawArrays(GL_TRIANGLES, 0, mesh->getNumVertices());
}

const glm::ivec2& kore::RenderManager::getRenderResolution() const {
    return _renderResolution;
}

void kore::RenderManager::
    setRenderResolution(const glm::ivec2& newResolution) {
    _renderResolution = newResolution;
    resolutionChanged();
}

void kore::RenderManager::resolutionChanged() {
    // Update all resolution-dependant resources here
    // (e.g. GBuffer-Textures...)
}
