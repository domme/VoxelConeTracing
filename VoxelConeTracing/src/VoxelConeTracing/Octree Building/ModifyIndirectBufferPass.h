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

#ifndef VCT_SRC_VCT_MODIFYINDIRECTBUFFERPASS_H_
#define VCT_SRC_VCT_MODIFYINDIRECTBUFFERPASS_H_

#include "KoRE/Passes/ShaderProgramPass.h"
#include "VoxelConeTracing/Scene/VCTscene.h"
#include "KoRE/SceneManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/ResourceManager.h"


class ModifyIndirectBufferPass : public kore::ShaderProgramPass
{
  public:
    ModifyIndirectBufferPass(const kore::ShaderData* shdIndirectBuffer,
                             const kore::ShaderData* shdACnumVoxelsBuffer,
                             VCTscene* vctScene,
                             kore::EOperationExecutionType executionType);
    virtual ~ModifyIndirectBufferPass(void);

  private:
    void initCallBuffer();

    kore::RenderManager* _renderMgr;
    kore::SceneManager* _sceneMgr;
    kore::ResourceManager* _resMgr;

    const kore::ShaderData* _shdIndirectBuffer;
    const kore::ShaderData* _shdACnumVoxelsBuffer;

    kore::IndexedBuffer _callIndirectBuffer;

    kore::ShaderProgram _shader;
    VCTscene* _vctScene;
};

#endif // VCT_SRC_VCT_MODIFYINDIRECTBUFFERPASS_H_
