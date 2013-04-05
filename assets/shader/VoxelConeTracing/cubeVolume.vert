#version 420

layout(location = 0) in vec3 v_position;

uniform mat4 modelWorld;
uniform mat4 view;
uniform mat4 proj;

smooth out vec3 posWS;

void main() {
	posWS = (modelWorld * vec4(v_position, 1.0)).xyz;
	
	gl_Position = proj * view * vec4(posWS, 1.0);
}
