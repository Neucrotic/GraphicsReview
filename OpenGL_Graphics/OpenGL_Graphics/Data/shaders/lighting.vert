#version 410

layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoords;
layout(location=3) in vec4 weights;
layout(location=4) in vec4 indices;

out vec4 vNormal;
out vec4 vPosition;
out vec2 vTexCoords;

uniform mat4 ProjectionView;
uniform mat4 global;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main()
{
	vTexCoords = TexCoords;
	vNormal = Normal;
	vPosition = Position;

	ivec4 newIndices = ivec4(indices);
	vec4 P = bones[newIndices.x] * Position * weights.x;
	P += bones[newIndices.y] * Position * weights.y;
	P += bones[newIndices.z] * Position * weights.z;
	P += bones[newIndices.w] * Position * weights.w;

	gl_Position = ProjectionView * P;
}