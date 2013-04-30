#version 420

in VertexData {
  vec3 posWS;
} In;

out vec4 color;

void main() {
  color = vec4(In.posWS, length(In.posWS));
}
