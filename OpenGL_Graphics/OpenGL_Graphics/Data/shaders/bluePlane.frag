#version 410

in vec2 fragTexcoord;

out vec4 outColour;
uniform sampler2D planeTex;

void main()
{
	outColour = vec4(0, 0, 1, 1); //texture(planeTex, fragTexcoord).rrrr;
	outColour.a = 1;
}