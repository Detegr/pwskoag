#version 150 core
#extension GL_ARB_explicit_attrib_location : require

layout(location=0) in vec2 in_pos;
layout(location=1) in vec2 in_uv;

uniform mat4 MVP;

out vec2 uv;

void main(void)
{
	uv = in_uv;
	vec4 v=vec4(in_pos, 0.0, 1.0);
	gl_Position = MVP * v;
}
