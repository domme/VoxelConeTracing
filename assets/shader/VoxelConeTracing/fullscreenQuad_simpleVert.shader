#version 420

layout(location = 0) in vec3 v_position;  // Quad-vertices in [-1..1]
smooth out vec2 uv;

void main() {
	gl_Position = vec4(v_position, 1.0);
	uv = (v_position.xy + 1.0) / 2.0;
}
