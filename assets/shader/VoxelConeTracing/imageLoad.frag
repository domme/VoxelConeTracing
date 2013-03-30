/*
Awaits the inputs of a fullscreenQuad-vertexShader
*/

#version 420

varying vec2 uv;

uniform vec2 imageDimensions;
layout(binding = 0, rgba8) uniform image2D image;

out vec4 color;

void main() {
	ivec2 coords = ivec2(uv * vec2(512, 512));
	color = imageLoad(image, coords);
}