/*
Awaits the inputs of a fullscreenQuad-vertexShader
*/

#version 420

smooth in vec2 uv;
layout(rgba8) uniform image2D image;

out vec4 color;

void main() {
	ivec2 coords = ivec2(uv * imageSize(image));
	color = imageLoad(image, coords);
}