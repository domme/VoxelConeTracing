#version 330
out vec4 out_color;

smooth in vec3 normal;
in vec3 UV;
uniform sampler2D tex;

void main(void)
{
	// very basic lighting
	vec3 dir_light = vec3(0, -1, -0.5);
	float lighting = max(dot(normal, normalize(dir_light)), 0) + 1.0f;
	out_color = vec4(texture2D(tex, vec2(UV.x, UV.y)).xyz, 1.0) * lighting;
	//out_color = vec4(0.0 * lighting;
}
