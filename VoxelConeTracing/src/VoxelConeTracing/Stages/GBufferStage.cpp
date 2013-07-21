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

#include "VoxelConeTracing/Stages/GBufferStage.h"
#include "VoxelConeTracing/Rendering/DeferredPass.h"

GBufferStage::GBufferStage(kore::Camera* mainCamera,
                           std::vector<kore::SceneNode*>& vRenderNodes,
                           int width, int height) {
  std::vector<GLenum> drawBufs;
  drawBufs.resize(4);
  drawBufs[0] = GL_COLOR_ATTACHMENT0;
  drawBufs[1] = GL_COLOR_ATTACHMENT1;
  drawBufs[2] = GL_COLOR_ATTACHMENT2;
  drawBufs[3] = GL_COLOR_ATTACHMENT3;
  this->setActiveAttachments(drawBufs);

  kore::FrameBuffer* gBuffer = new kore::FrameBuffer("gBuffer");
  this->setFrameBuffer(gBuffer);

  kore::STextureProperties props;
  props.width = width;
  props.height = height;
  props.targetType = GL_TEXTURE_2D;

  props.format = GL_RGB;
  props.internalFormat = GL_RGB8;
  props.pixelType = GL_UNSIGNED_BYTE;
  gBuffer->addTextureAttachment(props,"DiffuseColor",GL_COLOR_ATTACHMENT0);

  props.format = GL_RGB;
  props.internalFormat = GL_RGB32F;
  props.pixelType = GL_FLOAT;
  gBuffer->addTextureAttachment(props,"Position",GL_COLOR_ATTACHMENT1);

  props.format = GL_RGB;
  props.internalFormat = GL_RGB32F;
  props.pixelType = GL_FLOAT;
  gBuffer->addTextureAttachment(props,"Normal",GL_COLOR_ATTACHMENT2);

  props.format = GL_RGB;
  props.internalFormat = GL_RGB32F;
  props.pixelType = GL_FLOAT;
  gBuffer->addTextureAttachment(props, "Tangent", GL_COLOR_ATTACHMENT3);

  props.format = GL_DEPTH_STENCIL;
  props.internalFormat = GL_DEPTH24_STENCIL8;
  props.pixelType = GL_UNSIGNED_INT_24_8;
  gBuffer->addTextureAttachment(props, "Depth_Stencil", GL_DEPTH_STENCIL_ATTACHMENT);

  //kore::Camera* lightcam = static_cast<Camera*>(lightNodes[0]->getComponent(COMPONENT_CAMERA));
  this->addProgramPass(new DeferredPass(mainCamera, vRenderNodes));


}

GBufferStage::~GBufferStage()
{

}

