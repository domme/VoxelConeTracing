/*
 Copyright (c) 2012 The VCT Project

  This file is part of VoxelConeTracing and is an implementation of
  "Interactive Indirect Illumination Using Voxel Cone Tracing" by Crassin et al

  VoxelConeTracing is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VoxelConeTracing is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VoxelConeTracing.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
* \author Dominik Lazarek (dominik.lazarek@gmail.com)
* \author Andreas Weinmann (andy.weinmann@gmail.com)
*/

#include "VoxelConeTracing/Stages/ShadowMapStage.h"
#include "VoxelConeTracing/Rendering/ShadowMapPass.h"

ShadowMapStage::ShadowMapStage(kore::SceneNode* lightNode,
                               std::vector<kore::SceneNode*>& vRenderNodes,
                               int shadow_width, int shadow_height) {
 
  kore::FrameBuffer* _shadowBuffer = new kore::FrameBuffer("shadowBuffer");
  std::vector<GLenum> drawBufs;
  drawBufs.push_back(GL_COLOR_ATTACHMENT0);
  this->setActiveAttachments(drawBufs);

  kore::STextureProperties SMprops;
  SMprops.width = shadow_width;
  SMprops.height = shadow_height;
  SMprops.targetType = GL_TEXTURE_2D;
  SMprops.format = GL_DEPTH_STENCIL;
  SMprops.internalFormat =  GL_DEPTH24_STENCIL8;
  SMprops.pixelType = GL_UNSIGNED_INT_24_8;
  _shadowBuffer->addTextureAttachment(SMprops,"ShadowMap",GL_DEPTH_STENCIL_ATTACHMENT);

  SMprops.format = GL_RGB;
  SMprops.internalFormat =  GL_RGB32F;
  SMprops.pixelType = GL_FLOAT;
  _shadowBuffer->addTextureAttachment(SMprops,"SMposition",GL_COLOR_ATTACHMENT0);

  this->setFrameBuffer(_shadowBuffer);
  this->addProgramPass(new ShadowMapPass(
                       vRenderNodes, lightNode,
                       glm::uvec2(SMprops.width, SMprops.height)));
}

ShadowMapStage::~ShadowMapStage() {

}

