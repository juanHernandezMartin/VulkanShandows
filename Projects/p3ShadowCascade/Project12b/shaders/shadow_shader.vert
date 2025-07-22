#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

layout(binding = 0) uniform TransformInfo {
  mat4 MVP;
} Transform;

void main()
{
  gl_Position = Transform.MVP * vec4(VertexPosition, 1.0);
}