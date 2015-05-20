#version 410

layout(location=0) in vec4 position;
layout(location=2) in vec2 texcoord;

uniform mat4 ProjectionView;

out vec2 fragTexcoord;

void main()
{
	fragTexcoord = texcoord;
	gl_Position = ProjectionView * position;
}