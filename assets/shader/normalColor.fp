#version 330
out vec4 out_color;

smooth in vec3 normal;
in vec3 UV;
uniform sampler2D tex;

void main(void)
{
	// very basic lighting
	vec3 dir_light = vec3(0, 1, -0.5);
	float test = max(dot(normal, normalize(dir_light)), 0);
	out_color = texture2D(tex, vec2(UV.x, UV.y)); //vec4(UV.x, UV.y, UV.z, 1) * test; // texture coming soon, UV has to be used in order to be present in active shader attributes
}
