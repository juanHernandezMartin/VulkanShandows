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


layout(set=0, binding = 3) uniform sampler2D BaseTex;

layout(set=0, binding = 4) uniform sampler2DShadow ShadowMap1;
layout(set=0, binding = 5) uniform sampler2DShadow ShadowMap2;
layout(set=0, binding = 6) uniform sampler2DShadow ShadowMap3;

layout(location = 0) out vec4 FragColor;

void main()
{
  FragColor = vec4( 0, 0, 0, 1);

  if( shadowInRange(ShadowCoord1))
  {
    vec3 phongFrag = phongColor(vec3(1.0, 0.0, 0.0));
    FragColor = vec4(phongFrag, 1.0);
  }
  else if( shadowInRange(ShadowCoord2))
  {
    vec3 phongFrag = phongColor(vec3(0.0, 1.0, 0.0));
    FragColor = vec4(phongFrag, 1.0);
  }
  else if( shadowInRange(ShadowCoord3))
  {
    vec3 phongFrag = phongColor(vec3(0.0, 0.0, 1.0));
    FragColor = vec4(phongFrag, 1.0);
  }
}