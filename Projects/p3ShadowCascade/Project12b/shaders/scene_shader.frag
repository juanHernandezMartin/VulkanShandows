#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec4 ShadowCoord;

layout(set=0, binding = 0) uniform TransformInfo {
  mat4 MVP;
  mat4 ViewMatrix;
  mat4 ModelViewMatrix;
  mat4 ShadowMatrix;
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

layout(set=0, binding = 3) uniform sampler2D BaseTex;

layout(set=0, binding = 4) uniform sampler2DShadow ShadowMap;

layout(location = 0) out vec4 FragColor;

vec3 ads(vec3 TexColor)
{
  vec4 s4 = Transform.ViewMatrix*vec4(Light.Ldir, 0.0);
  vec3 n = normalize(Normal);
  vec3 v = normalize(-Position);
  vec3 s = normalize(-vec3(s4));
  vec3 r = reflect(-s, n);
  float dRate = max(dot(s, n), 0.0);
  float sRate = pow(max(dot(r, v), 0.0), Material.Shininess);
  vec3 difusse = Light.Ld * Material.Kd * dRate;
  vec3 specular = Light.Ls * Material.Ks * sRate;
  return difusse*TexColor + specular;
}

float shadow()
{
  vec4 ShadowTexCoord;
  ShadowTexCoord.x = (ShadowCoord.x/ShadowCoord.w) * 0.5 + 0.5;
  ShadowTexCoord.y = (ShadowCoord.y/ShadowCoord.w) * 0.5 + 0.5;
  ShadowTexCoord.z = (ShadowCoord.z/ShadowCoord.w);
  ShadowTexCoord.w = 1.0;

  if(ShadowTexCoord.x <0.0 || ShadowTexCoord.x >1.0 ) return 0.0;
  if(ShadowTexCoord.y <0.0 || ShadowTexCoord.y >1.0 ) return 0.0;

  float sum = 0;
  sum += textureProjOffset(ShadowMap, ShadowTexCoord, ivec2(-1,-1));
  sum += textureProjOffset(ShadowMap, ShadowTexCoord, ivec2(-1,1));
  sum += textureProjOffset(ShadowMap, ShadowTexCoord, ivec2(1,1));
  sum += textureProjOffset(ShadowMap, ShadowTexCoord, ivec2(1,-1));
  return sum * 0.25; 
}

void main()
{
  vec3 TexColor = vec3( texture(BaseTex,TexCoord) );
  vec3 ambient = Light.La * Material.Ka * TexColor;
  vec3 diffAndSpec = ads(TexColor);
  float shadow = shadow();
  FragColor = vec4(shadow * diffAndSpec+ ambient, 1.0);
}
