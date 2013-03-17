#version 330
out vec4 out_color;

in vec3 UV;
smooth in vec3 lightVecTS;
smooth in vec3 viewVecTS;

uniform sampler2D tex;
uniform sampler2D normalmap;


vec4 phong(vec2 uv, vec3 normal, vec3 lVec, vec3 vVec)
{
    vec4 vSpecular = vec4(0);
    vec4 vDiffuse = vec4(0);
    
    vec4 texColor = texture(tex, UV);
    vec4 vAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient * base;
    vVec=normalize(vVec);
    normal=normalize(normal);
    float intensity = 0.5;
        vec3 lVecNorm = normalize(lVec);
        float lambert = max(dot(normal,lVecNorm), 0.0);
        if(lambert > 0){
            vDiffuse = gl_LightSource.diffuse * lambert * texColor * intensity ;
            vec3 R = reflect(-vlVec, normal);
            if(gl_FrontMaterial.shininess>0){
                float specular = pow(clamp(dot(R, vVec), 0.0, 1.0), gl_FrontMaterial.shininess);
                vSpecular += gl_LightSource.specular * specular * intensity;
            }
        }
    return (vAmbient + vDiffuse + vSpecular);
}

void main(void)
{
    vec3 nmNormal = normalize(texture2D(normalmap, uv).xyz * 2.0 - 1.0); 
    out_color = phong(uv,nmNormal,lightVecTS,viewVecTS);
}





#version 120
#define MAX_LIGHTS 8
#define NUM_LIGHTS 2
#define MODE_PHONG 0
#define MODE_BUMP 1
#define MODE_PARALLAX 2
#define MODE_POM 3

varying vec3 lightVecTS[NUM_LIGHTS];
varying vec3 lightVecVS[NUM_LIGHTS];
varying vec3 viewVecTS;
varying vec3 viewVecVS;
varying vec3 normalVS;
varying vec2 vParallaxXY;
varying mat3 tbn;
varying vec2 outTexCoord0;
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform samplerCube cubeMap;
uniform int minSamples;
uniform int maxSamples;
uniform float ReflectionIndex;
uniform float RefractionIndex;
uniform float textureScaleS;
uniform float textureScaleT;
uniform	float fresnelBias;
uniform	float fresnelScale;
uniform	float fresnelPower;
uniform float Transmittance;
uniform int mode;
uniform bool Reflection;
uniform bool Refraction;
uniform bool ChromaticDispersion;
uniform vec3 ChromaticRatio;
uniform float fHeightScale;




vec2 RayIntersect(vec3 RayVec, vec2 uv){
    vec3 vRayVec = normalize(RayVec);
    vec3 fNormal = normalize(normalVS);
    //determite number of Samples depending on Ray angle
    float numSamples = mix(maxSamples,minSamples, dot(vRayVec,fNormal));
    int numCurrSample = 0;
    //Loop variables
    float stepSize = 1.0/numSamples;
    float stepHeight = 1.0;
    float lastHeight = 1.0;
    vec2 vFinalOffset = vec2(0);
    vec2 vCurrOffset = vParallaxXY * fHeightScale;
    vec2 vLastOffset = vCurrOffset;
    vec2 vOffsetStep = vParallaxXY * fHeightScale * stepSize;
    vec4 vCurrSample = vec4(0);
    vec4 vLastSample = vec4(0);
    while(numCurrSample < numSamples){
        vCurrSample = texture2D(normalMap,uv+vCurrOffset);
        if(vCurrSample.w > stepHeight){
            //intersection has to be between current and last sample
            lastHeight = stepHeight + stepSize;
            float intersect = (stepHeight * (lastHeight - vLastSample.w)- lastHeight *(stepHeight - vCurrSample.w))/
                              ((lastHeight-vLastSample.w) -(stepHeight - vCurrSample.w));
            vFinalOffset	=  vLastOffset + intersect * vOffsetStep;
            break;
        }
        else{
            numCurrSample++;
            vLastSample = vCurrSample;
            vLastOffset = vCurrOffset;
            vCurrOffset -= vOffsetStep;
            stepHeight -= stepSize;
        }
    }
    return vFinalOffset;
}

vec4 phong(vec2 uv, vec3 normal, vec3 lVec[NUM_LIGHTS], vec3 vVec){
    
    vec4 vSpecular = vec4(0);
    vec4 vDiffuse = vec4(0);
    
    vec4 base = texture2D(colorMap, uv);
    vec4 vAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient * base;
    vVec=normalize(vVec);
    normal=normalize(normal);
    float intensity = 0.5;
    for (int i=0; i<NUM_LIGHTS; i++){
        vec3 vlVec = normalize(lVec[i]);
        float lambert = max(dot(normal,vlVec), 0.0);
        if(lambert > 0){
            vDiffuse += gl_LightSource[i].diffuse * lambert * base * intensity ;
            vec3 R = reflect(-vlVec, normal);
            if(gl_FrontMaterial.shininess>0){
                float specular = pow(clamp(dot(R, vVec), 0.0, 1.0), gl_FrontMaterial.shininess);
                vSpecular += gl_LightSource[i].specular * specular * intensity;
            }
        }
    }
    return (vAmbient + vDiffuse + vSpecular);
}

void main()
{
    vec2 uv = vec2(outTexCoord0.s * textureScaleS,outTexCoord0.t* textureScaleT);
    vec4 final_color = vec4(0);
    
    vec3 incidentRayGS = vec3(0);
    vec3 normalGS = vec3(0);
    
    if(mode==MODE_PHONG){
        final_color = phong(uv,normalVS,lightVecVS,viewVecVS);
        
        incidentRayGS = normalize(mat3(gl_TextureMatrix[2]) * -viewVecVS);
        normalGS = normalize(mat3(gl_TextureMatrix[2]) * normalVS);
    }
    if(mode==MODE_BUMP){ 
        vec3 bump = normalize(texture2D(normalMap, uv).xyz * 2.0 - 1.0);
        final_color = phong(uv,bump,lightVecTS,viewVecTS);

        incidentRayGS = normalize(mat3(gl_TextureMatrix[2]) *tbn* -viewVecTS);
        normalGS = normalize(mat3(gl_TextureMatrix[2]) *tbn* bump);
    }
    
    if(mode==MODE_PARALLAX){
        float fHeightBias = fHeightScale * -0.5;
        float height = texture2D(normalMap, uv).w * fHeightScale + fHeightBias;
        vec2 vOffset = height * vParallaxXY;
        uv = uv + vOffset;
        
        vec3 bump = normalize(texture2D(normalMap, uv).xyz * 2.0 - 1.0);
        final_color = phong(uv,bump,lightVecTS,viewVecTS);

        incidentRayGS = normalize(mat3(gl_TextureMatrix[2]) *tbn* -viewVecTS);
        normalGS = normalize(mat3(gl_TextureMatrix[2]) *tbn* bump);
    
    }
    
    if(mode==MODE_POM){
        uv = uv + RayIntersect(viewVecVS,uv);

        vec3 bump = normalize(texture2D(normalMap, uv).xyz * 2.0 - 1.0);
        final_color = phong(uv,bump,lightVecTS,viewVecTS);

        incidentRayGS = normalize(mat3(gl_TextureMatrix[2]) *tbn* -viewVecTS);
        normalGS = normalize(mat3(gl_TextureMatrix[2]) *tbn* bump);
    }
    vec4 reflectColor = vec4(0);
    vec4 refractColor = vec4(0);

    if(Reflection){
        vec3 vReflect = normalize(reflect(incidentRayGS,normalGS));
        vec4 cube_colorReflect = textureCube(cubeMap,vReflect);
        reflectColor = mix(final_color,cube_colorReflect,ReflectionIndex);
        gl_FragColor = reflectColor;
    }

    if(Refraction){	
        vec4 cube_colorRefract = vec4(0);
        if(ChromaticDispersion){
            vec3 vRed   = refract(incidentRayGS, normalGS, ChromaticRatio.x);
            vec3 vGreen = refract(incidentRayGS, normalGS, ChromaticRatio.y);
            vec3 vBlue  = refract(incidentRayGS, normalGS, ChromaticRatio.z);
            cube_colorRefract.r = textureCube(cubeMap,vRed).r;
            cube_colorRefract.g = textureCube(cubeMap,vGreen).g;
            cube_colorRefract.b = textureCube(cubeMap,vBlue).b;
            cube_colorRefract.a = 1.0;
        }
        else{ 
        vec3 vRefract = refract(incidentRayGS,normalGS,RefractionIndex);
        cube_colorRefract = textureCube(cubeMap,vRefract);
        }
        
        refractColor = mix(final_color,cube_colorRefract,Transmittance);
        
        gl_FragColor = refractColor;
    }
    if(Reflection && Refraction){
        float FresnelFactor = clamp((fresnelBias + fresnelScale * pow(1 + dot(incidentRayGS, normalGS),fresnelPower)),0.0,1.0);
        vec4 reflectRefractColor = mix(refractColor,reflectColor,FresnelFactor);
        gl_FragColor = reflectRefractColor;
    }
    if(!Reflection && !Refraction) gl_FragColor = final_color;
    
}





