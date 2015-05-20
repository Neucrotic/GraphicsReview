#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

uniform mat4 ProjectionView;
uniform sampler2D perlinTexture;

out vec2 fragTexcoord;
out vec3 fragNormal;
out vec4 fragPos;

void main()
{
	fragPos = position;
	fragPos.y += texture(perlinTexture, texcoord).r * 5;

	fragNormal = normal;
	fragTexcoord = texcoord;
	
	gl_Position = ProjectionView * fragPos;
}