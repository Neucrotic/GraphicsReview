#version 410

in vec4 gPosition;
in vec4 gNormal;
in vec2 gTexCoords;
in float gThreshold;

out vec4 FragColour;

uniform sampler2D PerlinTexture;
uniform sampler2D SnowTexture;
uniform sampler2D SandTexture;
uniform sampler2D GrassTexture;

void main() 
{ 
	vec4 dirtColour = texture2D(SandTexture, gTexCoords);
	vec4 snowColour = texture2D(SnowTexture, gTexCoords);
	vec4 grassColour = texture2D(GrassTexture, gTexCoords);

	vec4 grassToDirt = mix(grassColour, dirtColour, smoothstep(0.5f, 0.8f, gThreshold));
	vec4 dirtToSnow = mix(dirtColour, grassColour, smoothstep(0.8f, 1.0f, gThreshold));
	float d = max(0, dot(normalize(gNormal.xyz), vec3(0, 1, 0) ));
	
	FragColour = mix( grassToDirt, dirtToSnow,  step( 0.8f, gThreshold) ) * vec4(d, d, d, 1);
}