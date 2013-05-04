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
#include "VoxelConeTracing/VCTscene.h"
#include "KoRE/SceneManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/ResourceManager.h"

struct SDrawArraysIndirectCommand {
  SDrawArraysIndirectCommand() :
    numVertices(0),
    numPrimitives(0),
    firstVertexIdx(0),
    baseInstanceIdx(0) {}

  uint numVertices;
  uint numPrimitives;
  uint firstVertexIdx;
  uint baseInstanceIdx;
};

class ModifyIndirectBufferPass : public kore::ShaderProgramPass
{
  public:
    ModifyIndirectBufferPass(const kore::ShaderData* shdIndirectBuffer,
                             VCTscene* vctScene);
    virtual ~ModifyIndirectBufferPass(void);

    inline void SetCommand(const SDrawArraysIndirectCommand& cmd) {_cmd = cmd;}
    
    inline void SetNumVertices(uint numVertices)
      {_cmd.numVertices = numVertices;}

    inline void SetNumPrimitives(uint numPrimitives)
      {_cmd.numPrimitives = numPrimitives;}

  private:
    kore::RenderManager* _renderMgr;
    kore::SceneManager* _sceneMgr;
    kore::ResourceManager* _resMgr;

    SDrawArraysIndirectCommand _cmd;

    kore::ShaderData _shdNumVertices;
    kore::ShaderData _shdNumPrimitives;
    kore::ShaderData _shdFirstVertexIdx;
    kore::ShaderData _shdBaseInstanceIdx;
    const kore::ShaderData* _shdIndirectBuffer;

    kore::ShaderProgram _shader;
    VCTscene* _vctScene;
};

#endif // VCT_SRC_VCT_MODIFYINDIRECTBUFFERPASS_H_
