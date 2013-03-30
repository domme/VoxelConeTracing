/*
Awaits the inputs of a fullscreenQuad-vertexShader
*/

#version 420

smooth in vec2 uv;

uniform vec2 imageDimensions;
layout(rgba8) coherent uniform image2D image;

out vec4 color;

void main() {
	ivec2 coords = ivec2(uv * vec2(512, 512));
	color = imageLoad(image, coords);
}