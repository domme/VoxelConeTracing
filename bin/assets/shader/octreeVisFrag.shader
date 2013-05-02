#version 420
#extension GL_ARB_shader_image_size : enable

in VertexData {
  vec3 viewDirVS;
} In;

layout(r32ui) uniform volatile uimageBuffer voxelFragmentListPosition;
layout(r32ui) uniform volatile uimageBuffer voxelFragmentListColor;

// This is the atomic counter from the voxelize-pass which now holds the
// number of allocated voxels
layout(binding = 0) uniform atomic_uint voxel_num;



out vec4 color;


vec4 convRGBA8ToVec4(uint val) {
    return vec4( float((val & 0x000000FF)), 
                 float((val & 0x0000FF00) >> 8U), 
                 float((val & 0x00FF0000) >> 16U), 
                 float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF)   << 24U
            |(uint(val.z) & 0x000000FF) << 16U
            |(uint(val.y) & 0x000000FF) << 8U 
            |(uint(val.x) & 0x000000FF);
}

uint convVec3ToXYZ10(vec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

uint ivec3_xyz10(in ivec3 val) {
   return (uint(val.z) & 0x000003FF) << 20U
          |(uint(val.y) & 0x000003FF) << 10U 
          |(uint(val.x) & 0x000003FF);
}


void main(void) {
  vec4 col = vec4(0.0);
  for(int i = 0; i < int(100); ++i) {
    uvec4 colU = imageLoad(voxelFragmentListColor, i);
    memoryBarrier();
     col = convRGBA8ToVec4(colU.x) / vec4(255);
    if (length(col) > 0.1) {
      break;
    }
  }

  color = col;
}
