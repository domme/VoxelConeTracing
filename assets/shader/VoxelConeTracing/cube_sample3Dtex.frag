/*
Awaits the inputs of a Cube-vertex shader
*/

#version 420

smooth in vec3 posWS;
flat in vec3 cubeCenter;
flat in float sideLengthHalf;

out vec4 color;

void main() {
	float distanceFactor = length(posWS - cubeCenter);
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f) / (distanceFactor + sideLengthHalf);
}
