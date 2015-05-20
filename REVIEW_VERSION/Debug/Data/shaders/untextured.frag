 #version 410
 
in vec4 vNormal;

out vec4 FragColor;

void main() 
{
	float d = max(0, dot(normalize(vNormal.xyz), vec3(0, 1, 0 ) ));

	FragColor = vec4(d, d, d, 1);
}