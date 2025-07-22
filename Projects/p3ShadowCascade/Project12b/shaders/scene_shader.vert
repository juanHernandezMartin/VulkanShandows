#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

layout(binding = 0) uniform TransformInfo {
  mat4 MVP;
  mat4 ViewMatrix;
  mat4 ModelViewMatrix;
  mat4 ShadowMatrix;
} Transform;

layout(location = 0) out vec3 Position;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoord;
layout(location = 3) out vec4 ShadowCoord;

void main()
{
  vec4 n4 = Transform.ModelViewMatrix*vec4(VertexNormal, 0.0);
  vec4 v4 = Transform.ModelViewMatrix*vec4(VertexPosition, 1.0);
  Normal = vec3(n4);
  Position = vec3(v4);
  TexCoord = VertexTexCoord;
  ShadowCoord = Transform.ShadowMatrix * vec4(VertexPosition, 1.0);
  gl_Position = Transform.MVP * vec4(VertexPosition, 1.0);
}