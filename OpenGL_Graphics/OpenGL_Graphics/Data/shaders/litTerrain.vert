#version 410

layout(location=0) in vec4 Position;
layout(location=2) in vec2 TexCoords;

out vec4 vPosition;
out vec2 vTexCoords;
out float vThreshold;

uniform mat4 ProjectionView;
uniform sampler2D PerlinTexture;

void main() 
{ 
	float perlinTexel = texture(PerlinTexture, TexCoords).r;
	vThreshold = perlinTexel;
	
	vec4 pos = Position;
	pos.y += perlinTexel * 100;

	vTexCoords = TexCoords;
	
	gl_Position = pos;
}