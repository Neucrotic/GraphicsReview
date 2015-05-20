#version 410

in vec4 vNormal;
in vec4 vPosition;
in vec2 vTexCoords;
in vec4 vWeights;
in vec4 vIndices;

out vec4 FragColor;

uniform vec3 LightDir;
uniform vec3 LightColour;
uniform vec3 CameraPos;
uniform float SpecPow;

uniform sampler2D diffuse;

void main()
{
	vec4 diffColour = texture(diffuse, vTexCoords);

	vec3 d = diffColour.xyz * max(0, dot(normalize(vNormal.xyz), LightDir));
	
	vec3 E = normalize(CameraPos - vPosition.xyz);
	vec3 R = reflect(-LightDir, vNormal.xyz);
	
	float s = max(0, dot(E, R));
	s = pow(s, SpecPow);

	FragColor = vec4(LightColour * d + LightColour * s, 1);
}