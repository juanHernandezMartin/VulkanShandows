#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec4 ShadowCoord1;
layout(location = 4) in vec4 ShadowCoord2;
layout(location = 5) in vec4 ShadowCoord3;

layout(set=0, binding = 0) uniform TransformInfo {
  mat4 MVP;
  mat4 ViewMatrix;
  mat4 ModelViewMatrix;
  mat4 ShadowMatrix1;
  mat4 ShadowMatrix2;
  mat4 ShadowMatrix3;
} Transform;

layout(set=0, binding = 1) uniform MaterialInfo 
{
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float Shininess;
} Material;

layout(set=0, binding = 2) uniform LightInfo 
{
  vec3 Ldir;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
} Light;



bool shadowInRange(vec4 ShadowCoord)
{
  if( ShadowCoord.x < 1.0 && ShadowCoord.x > -1.0 &&
      ShadowCoord.y < 1.0 && ShadowCoord.y > -1.0)
  {
    return true;
  }

  return false;
}


layout(set=0, binding = 3) uniform sampler2D BaseTex;

layout(set=0, binding = 4) uniform sampler2DShadow ShadowMap1;
layout(set=0, binding = 5) uniform sampler2DShadow ShadowMap2;
layout(set=0, binding = 6) uniform sampler2DShadow ShadowMap3;

layout(location = 0) out vec4 FragColor;

vec3 phongColor(vec3 textureColor)
{
  vec4 lightDirViewSpace = Transform.ViewMatrix * vec4(Light.Ldir, 0.0);
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(-Position);
  vec3 lightDir = normalize(-vec3(lightDirViewSpace));
  vec3 reflectionDir = reflect(-lightDir, normal);

  float diffuseFactor = max(dot(lightDir, normal), 0.0);
  float specularFactor = pow(max(dot(reflectionDir, viewDir), 0.0), Material.Shininess);

  vec3 diffuse = Light.Ld * Material.Kd * diffuseFactor;
  vec3 specular = Light.Ls * Material.Ks * specularFactor;

  return diffuse * textureColor + specular;
}

float getSumShadow(sampler2DShadow ShadowMap, vec4 ShadowCoord)
{
  vec4 currShadowTexCoord = (ShadowCoord/ShadowCoord.w)*0.5 + vec4(0.5, 0.5, 0.5, 0.5);
  float sum = 0.0; 
  sum += textureProjOffset(ShadowMap, currShadowTexCoord, ivec2(-1,-1));
  sum += textureProjOffset(ShadowMap, currShadowTexCoord, ivec2(-1,1));
  sum += textureProjOffset(ShadowMap, currShadowTexCoord, ivec2(1,1));
  sum += textureProjOffset(ShadowMap, currShadowTexCoord, ivec2(1,-1));
  return sum * 0.25;
}

void main()
{
  vec3 TexColor = vec3( texture(BaseTex,TexCoord) );
  vec3 ambient = Light.La * Material.Ka * TexColor;
  vec3 diffAndSpec = phongColor(TexColor);

  float shadow = 1.0;


  if( shadowInRange(ShadowCoord1))
  {
    shadow = getSumShadow(ShadowMap1, ShadowCoord1);
  }
  else if( shadowInRange(ShadowCoord2))
  {
    shadow = getSumShadow(ShadowMap2, ShadowCoord2);
  }
  else if( shadowInRange(ShadowCoord3))
  {
    shadow = getSumShadow(ShadowMap3, ShadowCoord3);
  }

  

  
  
  FragColor = vec4(shadow * diffAndSpec + ambient, 1.0);
}

