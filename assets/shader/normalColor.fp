#version 330
out vec4 out_color;

smooth in vec3 normal;
in vec3 UV;
uniform sampler2D tex;

void main(void)
{
    // very basic lighting
    vec3 dir_light = vec3(0.5, 1, 0.7);
    float lighting = max(dot(normal, normalize(dir_light)), 0);
   // out_color = vec4(texture(tex, vec2(UV.x, UV.y)).xyz * lighting, 1.0);
    //out_color.z = lighting;
    //out_color = vec4(UV.x,UV.y,1.0,1.0)*lighting;
    out_color = texture(tex, vec2(UV.x, UV.y))*lighting;
}
