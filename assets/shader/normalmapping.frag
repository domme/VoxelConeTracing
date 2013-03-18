#version 330
out vec4 out_color;

smooth in vec3 UV;
smooth in vec3 lightVecTS;
smooth in vec3 viewVecTS;

uniform sampler2D tex;
uniform sampler2D normalmap;

vec4 phong(in vec2 uv, in vec3 normal, in vec3 lVec, in vec3 vVec)
{
    vec4 vSpecular = vec4(0);
    vec4 vDiffuse = vec4(0);
    int shininess = 5;

    vec4 texColor = texture(tex, UV.xy);
    vec4 vAmbient = 0.2 * vec4(1,1,1,1) * texColor;
    vVec = normalize(vVec);
    normal = normalize(normal);
    float intensity = 1;
    vec3 lVecNorm = normalize(lVec);
    float lambert = max(dot(normal,lVecNorm), 0.0);
    if(lambert > 0){
       vec4 lightcolor = vec4(1,1,1,1);
       vDiffuse = lightcolor * lambert * texColor * intensity ;
       vec3 R = normalize(reflect(-lVecNorm, normal));
       if(shininess>0){
            float specular = pow(clamp(dot(R, vVec), 0.0, 1.0), shininess);
            vSpecular = lightcolor * specular * intensity;
            }
    }
    return (vAmbient + vDiffuse + vSpecular);
}


void main(void)
{
    vec3 nmNormal = normalize(texture(normalmap, UV.xy).xyz * 2.0 - 1.0); 
    
    out_color = phong(UV.xy,nmNormal,lightVecTS,viewVecTS);
}