#version 410

in vec2 fragTexcoord;
in vec4 fragPos;
in vec3 fragNormal;

out vec4 outColour;

uniform sampler2D perlinTexture;
//Light Data
uniform vec3 lightDir;
uniform vec3 lightColour;
uniform vec3 cameraPos;
uniform float specPower;
uniform vec3 ambientIntensity;
uniform vec3 light;

void main()
{
	/*vec3 normal = normalize(fragNormal);
	float diffuse = dot(light, normal);
	vec3 ambient = lightColour * ambientIntensity;
	
	float d = max(0, dot(normalize(fragNormal.xyz), lightDir));
	vec3 E = normalize(cameraPos - fragPos.xyz);
	vec3 R = reflect(-lightDir, fragNormal.xyz);
	float S = max(0, dot(E, R));
	
	vec4 Lighting = vec4((lightColour * d) + (lightColour * ambient), 1.0f);
	S = pow(S, specPower);
	outColour = texture(perlinTexture, fragTexcoord);// * Lighting;*/



	outColour = texture(perlinTexture, fragTexcoord).rrrr;
	outColour.a = 1;
	
	if (fragPos.y < 10)
	{
		outColour.r = -1.0f;
		outColour.b = -1.0f;
		//outColour.g = -1.0f;
	}
	if (fragPos.y > 25)
	{
		outColour.a = 1;
	}
}