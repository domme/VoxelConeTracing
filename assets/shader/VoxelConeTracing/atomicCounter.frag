/*
Awaits the inputs of a fullscreenQuad-vertexShader
*/

#version 420

smooth in vec2 uv;
layout(binding = 0, offset = 0) uniform atomic_uint atomicCounter;

out vec4 color;

void main() {
	uint counter = atomicCounterIncrement(atomicCounter);
	float r = float(counter) / (800.0 * 600.0);
	color = vec4(r, 0, 0, 1);
}