#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec4 ShadowCoord;

layout(set = 0, binding = 0) uniform TransformInfo {
  mat4 MVP;
  mat4 ViewMatrix;
  mat4 ModelViewMatrix;
  mat4 ShadowMatrix;
} Transform;

layout(set = 0, binding = 1) uniform MaterialInfo {
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float Shininess;
} Material;

layout(set = 0, binding = 2) uniform LightInfo {
  vec3 Ldir;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
} Light;

layout(set = 0, binding = 3) uniform sampler2D BaseTex;

layout(set = 0, binding = 4) uniform sampler2D ShadowMap;

layout(location = 0) out vec4 FragColor;

void main() {
  ivec2 shadowSize = textureSize(ShadowMap, 0);
  // Coordenadas de pantalla (en píxeles)
  vec2 frag = gl_FragCoord.xy;
  // UVs directas (1:1 con el shadow map)
  vec2 uv = frag / vec2(shadowSize);
  float depth = texture(ShadowMap, uv).r;
  FragColor = vec4(vec3(depth), 1.0);
  return;
}
