#version 410

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;

out vec4 FragColour;

uniform vec3 LightDir;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

void main()
{
	mat3 TBN = mat3(normalize(vTangent), normalize(vBiTangent), normalize(vNormal));
	
	vec3 N = texture(normalTexture, vTexCoord).xyz * 2 - 1;
	
	float d = max(0, dot(normalize(TBN * N), normalize(LightDir)));
	
	FragColour = texture(diffuseTexture, vTexCoord);
	FragColour.rgb = FragColour.rgb * d;
}