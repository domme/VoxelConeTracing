/*
Awaits the inputs of a fullscreenQuad-vertexShader
*/

#version 420

smooth in vec2 uv;
layout(rgba8) uniform image2D image;

out vec4 color;

void main() {
	ivec2 coords = ivec2(uv * imageSize(image));
	imageStore(image, coords, vec4(1.0f, 0.0f, 0.0f, 0.0f));

	color = vec4(0.0, 0.0, 0.0, 0.0);
}