#version 330
out vec4 out_color;

smooth in vec3 normal;
smooth in vec3 pos;
in vec3 UV;
uniform sampler2D tex;
uniform vec3 pointlightPos;

void main(void)
{
    // very basic lighting
    float falloffStart = 0.0f;
    float falloffEnd = 5.0f;
    vec3 dir_light = pointlightPos - pos;
    float lightDistance = length(dir_light);

    float lightIntensity = clamp( (falloffEnd - lightDistance) / (falloffEnd - falloffStart), 0.0, 1.0);

    float lighting = max(dot(normal, dir_light / lightDistance), 0);
    out_color = texture(tex, vec2(UV.x, UV.y))*lighting * 5.0f * lightIntensity;
}
