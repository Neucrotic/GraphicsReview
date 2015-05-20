#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];
in vec2 vTexCoords[];
in float vThreshold[];

out vec4 gPosition;
out vec4 gNormal;
out vec2 gTexCoords;
out float gThreshold;

uniform mat4 ProjectionView;

void main() 
{
	vec3 pos1, pos2, pos3;

	pos1 = gl_in[0].gl_Position.xyz;
	pos2 = gl_in[1].gl_Position.xyz;
	pos3 = gl_in[2].gl_Position.xyz;

	gNormal = vec4(cross(normalize(pos2 - pos1), normalize(pos3 - pos1)), 0);

	gl_Position = ProjectionView * gl_in[0].gl_Position;
	gPosition = vPosition[0];
	gTexCoords = vTexCoords[0];
	gThreshold = vThreshold[0];
	EmitVertex();
	
	gl_Position = ProjectionView * gl_in[1].gl_Position;
	gPosition = vPosition[1];
	gTexCoords = vTexCoords[1];
	gThreshold = vThreshold[1];
	EmitVertex();
	
	gl_Position = ProjectionView * gl_in[2].gl_Position;
	gPosition = vPosition[2];
	gTexCoords = vTexCoords[2];
	gThreshold = vThreshold[2];
	EmitVertex();

}