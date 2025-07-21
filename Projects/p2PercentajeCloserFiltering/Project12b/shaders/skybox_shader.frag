#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Position;

layout(location = 0) out vec4 FragColor;

layout(set=0, binding = 1) uniform samplerCube CubemapTex;

void main()
{
    FragColor = texture(CubemapTex,Position);
}
