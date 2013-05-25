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

#ifndef VCT_SRC_VCT_OBFLAGPASS_H_
#define VCT_SRC_VCT_OBFLAGPASS_H_

#include "KoRE/Passes/ShaderProgramPass.h"
#include "VoxelConeTracing/VCTscene.h"
#include "KoRE/SceneManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/ResourceManager.h"

class ObFlagPass : public kore::ShaderProgramPass
{
  public:
    ObFlagPass(VCTscene* vctScene, bool executeOnce);
    virtual ~ObFlagPass(void);

  private:
    kore::RenderManager* _renderMgr;
    kore::SceneManager* _sceneMgr;
    kore::ResourceManager* _resMgr;

    kore::ShaderProgram _flagShader;
    VCTscene* _vctScene;
};

#endif //VCT_SRC_VCT_OBFLAGPASS_H_
