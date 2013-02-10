#version 330
out vec4 out_color;

smooth in vec3 normal;

void main(void)
{
	out_color = vec4( normal, 1.0 );
}
