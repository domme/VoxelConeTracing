/*
Awaits the inputs of a Cube-vertex shader
*/

#version 420

smooth in vec3 posWS;
out vec4 color;

uniform sampler3D tex3D;

void main() {
	ivec3 texSize = textureSize(tex3D, 0);
	vec3 samplePos = posWS / vec3(texSize.x, texSize.y, texSize.z);

	color = texture3D(tex3D, samplePos);
}
