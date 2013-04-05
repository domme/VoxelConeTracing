#version 420

layout(location = 0) in vec3 v_position;

uniform mat4 modelWorld;
uniform mat4 view;
uniform mat4 proj;

smooth out vec3 posWS;
flat out vec3 cubeCenter;
flat out float sideLengthHalf;

void main() {
	posWS = (modelWorld * vec4(v_position, 1.0)).xyz;
	
	cubeCenter = vec3(modelWorld[3][0], modelWorld[3][1], modelWorld[3][2]);
	sideLengthHalf = abs(v_position.x);

	gl_Position = proj * view * vec4(posWS, 1.0);
}
