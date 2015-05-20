#include "Gizmos_Fix.h"
#include "gl_core_4_4.h"
#include "glm/ext.hpp"

Gizmos* Gizmos::sm_singleton = nullptr;

Gizmos::Gizmos(unsigned int a_maxLines, unsigned int a_maxTris,
			   unsigned int a_max2DLines, unsigned int a_max2DTris)
	: m_maxLines(a_maxLines),
	m_lineCount(0),
	m_lines(new GizmoLine[a_maxLines]),
	m_maxTris(a_maxTris),
	m_triCount(0),
	m_tris(new GizmoTri[a_maxTris]),
	m_transparentTriCount(0),
	m_transparentTris(new GizmoTri[a_maxTris]),
	m_max2DLines(a_max2DLines),
	m_2DlineCount(0),
	m_2Dlines(new GizmoLine[a_max2DLines]),
	m_max2DTris(a_max2DTris),
	m_2DtriCount(0),
	m_2Dtris(new GizmoTri[a_max2DTris])
{
	// create shaders
	const char* vsSource = "#version 150\n \
					 in vec4 Position; \
					 in vec4 Colour; \
					 out vec4 vColour; \
					 uniform mat4 ProjectionView; \
					 void main() { vColour = Colour; gl_Position = ProjectionView * Position; }";

	const char* fsSource = "#version 150\n \
					 in vec4 vColour; \
                     out vec4 FragColor; \
					 void main()	{ FragColor = vColour; }";
    
    
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, (const char**)&vsSource, 0);
	glCompileShader(vs);

	glShaderSource(fs, 1, (const char**)&fsSource, 0);
	glCompileShader(fs);

	m_shader = glCreateProgram();
	glAttachShader(m_shader, vs);
	glAttachShader(m_shader, fs);
	glBindAttribLocation(m_shader, 0, "Position");
	glBindAttribLocation(m_shader, 1, "Colour");
	glLinkProgram(m_shader);
    
	int success = GL_FALSE;
    glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(m_shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link Gizmo shader program!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
    
    // create VBOs
	glGenBuffers( 1, &m_lineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxLines * sizeof(GizmoLine), m_lines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_triVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_tris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_transparentTriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_transparentTris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DlineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DLines * sizeof(GizmoLine), m_2Dlines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DtriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DTris * sizeof(GizmoTri), m_2Dtris, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_lineVAO);
	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_triVAO);
	glBindVertexArray(m_triVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_transparentTriVAO);
	glBindVertexArray(m_transparentTriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_2DlineVAO);
	glBindVertexArray(m_2DlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_2DtriVAO);
	glBindVertexArray(m_2DtriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(GizmoVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Gizmos::~Gizmos()
{
	delete[] m_lines;
	delete[] m_tris;
	delete[] m_transparentTris;
	glDeleteBuffers( 1, &m_lineVBO );
	glDeleteBuffers( 1, &m_triVBO );
	glDeleteBuffers( 1, &m_transparentTriVBO );
	glDeleteVertexArrays( 1, &m_lineVAO );
	glDeleteVertexArrays( 1, &m_triVAO );
	glDeleteVertexArrays( 1, &m_transparentTriVAO );
	delete[] m_2Dlines;
	delete[] m_2Dtris;
	glDeleteBuffers( 1, &m_2DlineVBO );
	glDeleteBuffers( 1, &m_2DtriVBO );
	glDeleteVertexArrays( 1, &m_2DlineVAO );
	glDeleteVertexArrays( 1, &m_2DtriVAO );
	glDeleteProgram(m_shader);
}

void Gizmos::create(unsigned int a_maxLines /* = 0xffff */, unsigned int a_maxTris /* = 0xffff */,
					unsigned int a_max2DLines /* = 0xff */, unsigned int a_max2DTris /* = 0xff */)
{
	if (sm_singleton == nullptr)
		sm_singleton = new Gizmos(a_maxLines,a_maxTris,a_max2DLines,a_max2DTris);
}

void Gizmos::destroy()
{
	delete sm_singleton;
	sm_singleton = nullptr;
}

void Gizmos::clear()
{
	sm_singleton->m_lineCount = 0;
	sm_singleton->m_triCount = 0;
	sm_singleton->m_transparentTriCount = 0;
	sm_singleton->m_2DlineCount = 0;
	sm_singleton->m_2DtriCount = 0;
}

// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
// at the transform's translation. Optional scale available.
void Gizmos::addTransform(const glm::mat4& a_transform, float a_fScale /* = 1.0f */)
{
	glm::vec4 vXAxis = a_transform[3] + a_transform[0] * a_fScale;
	glm::vec4 vYAxis = a_transform[3] + a_transform[1] * a_fScale;
	glm::vec4 vZAxis = a_transform[3] + a_transform[2] * a_fScale;

	glm::vec4 vRed(1,0,0,1);
	glm::vec4 vGreen(0,1,0,1);
	glm::vec4 vBlue(0,0,1,1);

	
	auto vec1 = glm::vec3(a_transform[3].x, a_transform[3].y, a_transform[3].z);
	auto vec2X = glm::vec3(vXAxis.x, vXAxis.y, vXAxis.z);
	auto vec2Y = glm::vec3(vYAxis.x, vYAxis.y, vYAxis.z);
	auto vec2Z = glm::vec3(vZAxis.x, vZAxis.y, vZAxis.z);
	addLine(vec1, vec2X, vRed, vRed);
	addLine(vec1, vec2Y, vGreen, vGreen);
	addLine(vec1, vec2Z, vBlue, vBlue);
}

void Gizmos::addAABB(const glm::vec3& a_center, 
	const glm::vec3& a_rvExtents, 
	const glm::vec4& a_colour, 
	const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec3 vVerts[8];
	glm::vec3 vX(a_rvExtents.x, 0, 0);
	glm::vec3 vY(0, a_rvExtents.y, 0);
	glm::vec3 vZ(0, 0, a_rvExtents.z);

	if (a_transform != nullptr)
	{
		auto temp = (*a_transform * glm::vec4(vX,1));
		vX = glm::vec3(temp.x, temp.y, temp.z);
		temp = (*a_transform * glm::vec4(vY,1));
		vY = glm::vec3(temp.x, temp.y, temp.z);
		temp = (*a_transform * glm::vec4(vZ,1));
		vZ = glm::vec3(temp.x, temp.y, temp.z);
	}

	// top verts
	vVerts[0] = a_center - vX - vZ - vY;
	vVerts[1] = a_center - vX + vZ - vY;
	vVerts[2] = a_center + vX + vZ - vY;
	vVerts[3] = a_center + vX - vZ - vY;

	// bottom verts
	vVerts[4] = a_center - vX - vZ + vY;
	vVerts[5] = a_center - vX + vZ + vY;
	vVerts[6] = a_center + vX + vZ + vY;
	vVerts[7] = a_center + vX - vZ + vY;

	addLine(vVerts[0], vVerts[1], a_colour, a_colour);
	addLine(vVerts[1], vVerts[2], a_colour, a_colour);
	addLine(vVerts[2], vVerts[3], a_colour, a_colour);
	addLine(vVerts[3], vVerts[0], a_colour, a_colour);

	addLine(vVerts[4], vVerts[5], a_colour, a_colour);
	addLine(vVerts[5], vVerts[6], a_colour, a_colour);
	addLine(vVerts[6], vVerts[7], a_colour, a_colour);
	addLine(vVerts[7], vVerts[4], a_colour, a_colour);

	addLine(vVerts[0], vVerts[4], a_colour, a_colour);
	addLine(vVerts[1], vVerts[5], a_colour, a_colour);
	addLine(vVerts[2], vVerts[6], a_colour, a_colour);
	addLine(vVerts[3], vVerts[7], a_colour, a_colour);
}

void Gizmos::addAABBFilled(const glm::vec3& a_center, 
	const glm::vec3& a_rvExtents, 
	const glm::vec4& a_fillColour, 
	const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec3 vVerts[8];
	glm::vec3 vX(a_rvExtents.x, 0, 0);
	glm::vec3 vY(0, a_rvExtents.y, 0);
	glm::vec3 vZ(0, 0, a_rvExtents.z);

	if (a_transform != nullptr)
	{
		auto temp = (*a_transform * glm::vec4(vX,1));
		vX = glm::vec3(temp.x, temp.y, temp.z);
		temp = (*a_transform * glm::vec4(vY,1));
		vY = glm::vec3(temp.x, temp.y, temp.z);
		temp = (*a_transform * glm::vec4(vZ,1));
		vZ = glm::vec3(temp.x, temp.y, temp.z);
	}

	// top verts
	vVerts[0] = a_center - vX - vZ - vY;
	vVerts[1] = a_center - vX + vZ - vY;
	vVerts[2] = a_center + vX + vZ - vY;
	vVerts[3] = a_center + vX - vZ - vY;

	// bottom verts
	vVerts[4] = a_center - vX - vZ + vY;
	vVerts[5] = a_center - vX + vZ + vY;
	vVerts[6] = a_center + vX + vZ + vY;
	vVerts[7] = a_center + vX - vZ + vY;

	glm::vec4 vWhite(1,1,1,1);

	addLine(vVerts[0], vVerts[1], vWhite, vWhite);
	addLine(vVerts[1], vVerts[2], vWhite, vWhite);
	addLine(vVerts[2], vVerts[3], vWhite, vWhite);
	addLine(vVerts[3], vVerts[0], vWhite, vWhite);

	addLine(vVerts[4], vVerts[5], vWhite, vWhite);
	addLine(vVerts[5], vVerts[6], vWhite, vWhite);
	addLine(vVerts[6], vVerts[7], vWhite, vWhite);
	addLine(vVerts[7], vVerts[4], vWhite, vWhite);

	addLine(vVerts[0], vVerts[4], vWhite, vWhite);
	addLine(vVerts[1], vVerts[5], vWhite, vWhite);
	addLine(vVerts[2], vVerts[6], vWhite, vWhite);
	addLine(vVerts[3], vVerts[7], vWhite, vWhite);

	// top
	addTri(vVerts[2], vVerts[1], vVerts[0], a_fillColour);
	addTri(vVerts[3], vVerts[2], vVerts[0], a_fillColour);

	// bottom
	addTri(vVerts[5], vVerts[6], vVerts[4], a_fillColour);
	addTri(vVerts[6], vVerts[7], vVerts[4], a_fillColour);

	// front
	addTri(vVerts[4], vVerts[3], vVerts[0], a_fillColour);
	addTri(vVerts[7], vVerts[3], vVerts[4], a_fillColour);

	// back
	addTri(vVerts[1], vVerts[2], vVerts[5], a_fillColour);
	addTri(vVerts[2], vVerts[6], vVerts[5], a_fillColour);

	// left
	addTri(vVerts[0], vVerts[1], vVerts[4], a_fillColour);
	addTri(vVerts[1], vVerts[5], vVerts[4], a_fillColour);

	// right
	addTri(vVerts[2], vVerts[3], vVerts[7], a_fillColour);
	addTri(vVerts[6], vVerts[2], vVerts[7], a_fillColour);
}

void Gizmos::addCylinderFilled(const glm::vec3& a_center, float a_radius, float a_fHalfLength,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 white(1,1,1,1);

	float segmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v0top(0,a_fHalfLength,0);
		glm::vec3 v1top( sinf( i * segmentSize ) * a_radius, a_fHalfLength, cosf( i * segmentSize ) * a_radius);
		glm::vec3 v2top( sinf( (i+1) * segmentSize ) * a_radius, a_fHalfLength, cosf( (i+1) * segmentSize ) * a_radius);
		glm::vec3 v0bottom(0,-a_fHalfLength,0);
		glm::vec3 v1bottom( sinf( i * segmentSize ) * a_radius, -a_fHalfLength, cosf( i * segmentSize ) * a_radius);
		glm::vec3 v2bottom( sinf( (i+1) * segmentSize ) * a_radius, -a_fHalfLength, cosf( (i+1) * segmentSize ) * a_radius);

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v0top,1));
			v0top = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v1top,1));
			v1top = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2top,1));
			v2top = glm::vec3(temp.x, temp.y, temp.z);

			temp = (*a_transform * glm::vec4(v0bottom,1));
			v0bottom = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v1bottom,1));
			v1bottom = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2bottom,1));
			v2bottom = glm::vec3(temp.x, temp.y, temp.z);
		}

		// triangles
		addTri( a_center + v0top, a_center + v1top, a_center + v2top, a_fillColour);
		addTri( a_center + v0bottom, a_center + v2bottom, a_center + v1bottom, a_fillColour);
		addTri( a_center + v2top, a_center + v1top, a_center + v1bottom, a_fillColour);
		addTri( a_center + v1bottom, a_center + v2bottom, a_center + v2top, a_fillColour);

		// lines
		addLine(a_center + v1top, a_center + v2top, white, white);
		addLine(a_center + v1top, a_center + v1bottom, white, white);
		addLine(a_center + v1bottom, a_center + v2bottom, white, white);
	}
}

void Gizmos::addRing(const glm::vec3& a_center, float a_innerRadius, float a_outerRadius,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * a_outerRadius, 0, cosf( i * fSegmentSize ) * a_outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * a_outerRadius, 0, cosf( (i+1) * fSegmentSize ) * a_outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize ) * a_innerRadius, 0, cosf( i * fSegmentSize ) * a_innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize ) * a_innerRadius, 0, cosf( (i+1) * fSegmentSize ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v1outer,1));
			v1outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2outer,1));
			v2outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v1inner,1));
			v1inner = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2inner,1));
			v2inner = glm::vec3(temp.x, temp.y, temp.z);
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center + v2outer, a_center + v1outer, a_center + v1inner, a_fillColour);
			addTri(a_center + v1inner, a_center + v2inner, a_center + v2outer, a_fillColour);

			addTri(a_center + v1inner, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v2inner, a_center + v1inner, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1inner + a_center, a_center + v2inner + a_center, vSolid, vSolid);
			addLine(a_center + v1outer + a_center, a_center + v2outer + a_center, vSolid, vSolid);
		}
	}
}

void Gizmos::addDisk(const glm::vec3& a_center, float a_radius,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * a_radius, 0, cosf( i * fSegmentSize ) * a_radius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * a_radius, 0, cosf( (i+1) * fSegmentSize ) * a_radius );

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v1outer,1));
			v1outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2outer,1));
			v2outer = glm::vec3(temp.x, temp.y, temp.z);
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v1outer, a_center, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1outer, a_center + v2outer, vSolid, vSolid);
		}
	}
}

void Gizmos::addArc(const glm::vec3& a_center, float a_rotation,
	float a_radius, float a_arcHalfAngle,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * a_arcHalfAngle) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius);
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius);

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v1outer,1));
			v1outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2outer,1));
			v2outer = glm::vec3(temp.x, temp.y, temp.z);
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v1outer, a_center, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1outer, a_center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (a_fillColour.w == 0)
	{
		glm::vec3 v1outer( sinf( -a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( -a_arcHalfAngle + a_rotation ) * a_radius );
		glm::vec3 v2outer( sinf( a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( a_arcHalfAngle + a_rotation ) * a_radius );

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v1outer,1));
			v1outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2outer,1));
			v2outer = glm::vec3(temp.x, temp.y, temp.z);
		}

		addLine(a_center, a_center + v1outer, vSolid, vSolid);
		addLine(a_center, a_center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::addArcRing(const glm::vec3& a_center, float a_rotation, 
	float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */)
{
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * a_arcHalfAngle) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v1outer,1));
			v1outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2outer,1));
			v2outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v1inner,1));
			v1inner = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2inner,1));
			v2inner = glm::vec3(temp.x, temp.y, temp.z);
		}

		if (a_fillColour.w != 0)
		{
			addTri(a_center + v2outer, a_center + v1outer, a_center + v1inner, a_fillColour);
			addTri(a_center + v1inner, a_center + v2inner, a_center + v2outer, a_fillColour);

			addTri(a_center + v1inner, a_center + v1outer, a_center + v2outer, a_fillColour);
			addTri(a_center + v2outer, a_center + v2inner, a_center + v1inner, a_fillColour);
		}
		else
		{
			// line
			addLine(a_center + v1inner, a_center + v2inner, vSolid, vSolid);
			addLine(a_center + v1outer, a_center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (a_fillColour.w == 0)
	{
		glm::vec3 v1outer( sinf( -a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( -a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v1outer,1));
			v1outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2outer,1));
			v2outer = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v1inner,1));
			v1inner = glm::vec3(temp.x, temp.y, temp.z);
			temp = (*a_transform * glm::vec4(v2inner,1));
			v2inner = glm::vec3(temp.x, temp.y, temp.z);
		}

		addLine(a_center + v1inner, a_center + v1outer, vSolid, vSolid);
		addLine(a_center + v2inner, a_center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::addSphere(const glm::vec3& a_center, float a_radius, int a_rows, int a_columns, const glm::vec4& a_fillColour, 
								const glm::mat4* a_transform /*= nullptr*/, float a_longMin /*= 0.f*/, float a_longMax /*= 360*/, 
								float a_latMin /*= -90*/, float a_latMax /*= 90*/)
{
	float inverseRadius = 1/a_radius;
	//Invert these first as the multiply is slightly quicker
	float invColumns = 1.0f/float(a_columns);
	float invRows = 1.0f/float(a_rows);

	float DEG2RAD = glm::pi<float>() / 180;
	
	//Lets put everything in radians first
	float latitiudinalRange = (a_latMax - a_latMin) * DEG2RAD;
	float longitudinalRange = (a_longMax - a_longMin) * DEG2RAD;
	// for each row of the mesh
	glm::vec3* v4Array = new glm::vec3[a_rows*a_columns + a_columns];

	for (int row = 0; row <= a_rows; ++row)
	{
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis  = ratioAroundXAxis * latitiudinalRange + (a_latMin * DEG2RAD);
		float y  =  a_radius * sin(radiansAboutXAxis);
		float z  =  a_radius * cos(radiansAboutXAxis);
		
		for ( int col = 0; col <= a_columns; ++col )
		{
			float ratioAroundYAxis   = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (a_longMin * DEG2RAD);
			glm::vec3 v4Point( -z * sinf(theta), y, -z * cosf(theta) );
			glm::vec3 v4Normal( inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			if (a_transform != nullptr)
			{
				v4Point  = glm::vec3(*a_transform * glm::vec4(v4Point,1));
				v4Normal = glm::vec3(*a_transform * glm::vec4(v4Normal,1));
			}

			int index = row * a_columns + (col % a_columns);
			v4Array[index] = v4Point;
		}
	}
	
	for (int face = 0; face < (a_rows)*(a_columns); ++face )
	{
		int iNextFace = face + 1;		
		
		if( iNextFace % a_columns == 0 )
		{
			iNextFace = iNextFace - (a_columns);
		}

		addLine(a_center + v4Array[face], a_center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));
		
		if( face % a_columns == 0 && longitudinalRange < (glm::pi<float>() * 2))
		{
				continue;
		}
		addLine(a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));

		addTri( a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face], a_center + v4Array[iNextFace], a_fillColour);
		addTri( a_center + v4Array[iNextFace+a_columns], a_center + v4Array[face+a_columns], a_center + v4Array[face], a_fillColour);		
	}

	delete[] v4Array;	
}

void Gizmos::addHermiteSpline(const glm::vec3& a_start, const glm::vec3& a_end,
	const glm::vec3& a_tangentStart, const glm::vec3& a_tangentEnd, unsigned int a_segments, const glm::vec4& a_colour)
{
	a_segments = a_segments > 1 ? a_segments : 1;

	glm::vec3 prev = a_start;

	for ( unsigned int i = 1 ; i <= a_segments ; ++i )
	{
		float s = i / (float)a_segments;

		float s2 = s * s;
		float s3 = s2 * s;
		float h1 = (2.0f * s3) - (3.0f * s2) + 1.0f;
		float h2 = (-2.0f * s3) + (3.0f * s2);
		float h3 =  s3- (2.0f * s2) + s;
		float h4 = s3 - s2;
		glm::vec3 p = (a_start * h1) + (a_end * h2) + (a_tangentStart * h3) + (a_tangentEnd * h4);

		addLine(prev,p,a_colour,a_colour);
		prev = p;
	}
}

void Gizmos::addLine(const glm::vec3& a_rv0,  const glm::vec3& a_rv1, const glm::vec4& a_colour)
{
	addLine(a_rv0,a_rv1,a_colour,a_colour);
}

void Gizmos::addLine(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec4& a_colour0, const glm::vec4& a_colour1)
{
	if (sm_singleton != nullptr &&
		sm_singleton->m_lineCount < sm_singleton->m_maxLines)
	{
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v0.position = glm::vec4(a_rv0,1);
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v0.colour = a_colour0;
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v1.position = glm::vec4(a_rv1,1);
		sm_singleton->m_lines[ sm_singleton->m_lineCount ].v1.colour = a_colour1;

		sm_singleton->m_lineCount++;
	}
}

void Gizmos::addTri(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec3& a_rv2, const glm::vec4& a_colour)
{
	if (sm_singleton != nullptr)
	{
		if (a_colour.w == 1)
		{
			if (sm_singleton->m_triCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v0.position = glm::vec4(a_rv0,1);
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v1.position = glm::vec4(a_rv1,1);
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v2.position = glm::vec4(a_rv2,1);
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v0.colour = a_colour;
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v1.colour = a_colour;
				sm_singleton->m_tris[ sm_singleton->m_triCount ].v2.colour = a_colour;

				sm_singleton->m_triCount++;
			}
		}
		else
		{
			if (sm_singleton->m_transparentTriCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v0.position = glm::vec4(a_rv0,1);
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v1.position = glm::vec4(a_rv1,1);
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v2.position = glm::vec4(a_rv2,1);
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v0.colour = a_colour;
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v1.colour = a_colour;
				sm_singleton->m_transparentTris[ sm_singleton->m_transparentTriCount ].v2.colour = a_colour;

				sm_singleton->m_transparentTriCount++;
			}
		}
	}
}

void Gizmos::add2DAABB(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform /*= nullptr*/)
{	
	glm::vec2 verts[4];
	glm::vec2 vX(a_extents.x, 0);
	glm::vec2 vY(0, a_extents.y);

	if (a_transform != nullptr)
	{
		auto temp = (*a_transform *  glm::vec4(vX,0,1));
		vX = glm::vec2(temp.x, temp.y);
		temp = (*a_transform * glm::vec4(vY,0,1));
		vY = glm::vec2(temp.x, temp.y);
	}

	verts[0] = a_center - vX - vY;
	verts[1] = a_center + vX - vY;
	verts[2] = a_center - vX + vY;
	verts[3] = a_center + vX + vY;

	add2DLine(verts[0], verts[1], a_colour, a_colour);
	add2DLine(verts[1], verts[2], a_colour, a_colour);
	add2DLine(verts[2], verts[3], a_colour, a_colour);
	add2DLine(verts[3], verts[0], a_colour, a_colour);
}

void Gizmos::add2DAABBFilled(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform /*= nullptr*/)
{	
	glm::vec2 verts[4];
	glm::vec2 vX(a_extents.x, 0);
	glm::vec2 vY(0, a_extents.y);

	if (a_transform != nullptr)
	{
		auto temp = (*a_transform * glm::vec4(vX,0,1));
		vX = glm::vec2(temp.x, temp.y);
		temp = (*a_transform * glm::vec4(vY,0,1));
		vY = glm::vec2(temp.x, temp.y);
	}

	verts[0] = a_center - vX - vY;
	verts[1] = a_center + vX - vY;
	verts[2] = a_center + vX + vY;
	verts[3] = a_center - vX + vY;
	
	add2DTri(verts[0], verts[1], verts[2], a_colour);
	add2DTri(verts[0], verts[2], verts[3], a_colour);
}

void Gizmos::add2DCircle(const glm::vec2& a_center, float a_radius, unsigned int a_segments, const glm::vec4& a_colour, const glm::mat4* a_transform /*= nullptr*/)
{
	glm::vec4 solidColour = a_colour;
	solidColour.w = 1;

	float segmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec2 v1outer( sinf( i * segmentSize ) * a_radius, cosf( i * segmentSize ) * a_radius );
		glm::vec2 v2outer( sinf( (i+1) * segmentSize ) * a_radius, cosf( (i+1) * segmentSize ) * a_radius );

		if (a_transform != nullptr)
		{
			auto temp = (*a_transform * glm::vec4(v1outer,0,1));
			v1outer = glm::vec2(temp.x, temp.y);
			temp = (*a_transform * glm::vec4(v2outer,0,1));
			v2outer = glm::vec2(temp.x, temp.y);
		}

		if (a_colour.w != 0)
		{
			add2DTri(a_center, a_center + v1outer, a_center + v2outer, a_colour);
			add2DTri(a_center + v2outer, a_center + v1outer, a_center, a_colour);
		}
		else
		{
			// line
			add2DLine(a_center + v1outer, a_center + v2outer, solidColour, solidColour);
		}
	}
}

void Gizmos::add2DLine(const glm::vec2& a_rv0,  const glm::vec2& a_rv1, const glm::vec4& a_colour)
{
	add2DLine(a_rv0,a_rv1,a_colour,a_colour);
}

void Gizmos::add2DLine(const glm::vec2& a_rv0, const glm::vec2& a_rv1, const glm::vec4& a_colour0, const glm::vec4& a_colour1)
{
	if (sm_singleton != nullptr &&
		sm_singleton->m_2DlineCount < sm_singleton->m_max2DLines)
	{
		sm_singleton->m_2Dlines[ sm_singleton->m_2DlineCount ].v0.position = glm::vec4(a_rv0,1,1);
		sm_singleton->m_2Dlines[ sm_singleton->m_2DlineCount ].v0.colour = a_colour0;
		sm_singleton->m_2Dlines[ sm_singleton->m_2DlineCount ].v1.position = glm::vec4(a_rv1,1,1);
		sm_singleton->m_2Dlines[ sm_singleton->m_2DlineCount ].v1.colour = a_colour1;

		sm_singleton->m_2DlineCount++;
	}
}

void Gizmos::add2DTri(const glm::vec2& a_rv0, const glm::vec2& a_rv1, const glm::vec2& a_rv2, const glm::vec4& a_colour)
{
	if (sm_singleton != nullptr)
	{
		if (sm_singleton->m_2DtriCount < sm_singleton->m_max2DTris)
		{
			sm_singleton->m_2Dtris[ sm_singleton->m_2DtriCount ].v0.position = glm::vec4(a_rv0,1,1);
			sm_singleton->m_2Dtris[ sm_singleton->m_2DtriCount ].v1.position = glm::vec4(a_rv1,1,1);
			sm_singleton->m_2Dtris[ sm_singleton->m_2DtriCount ].v2.position = glm::vec4(a_rv2,1,1);
			sm_singleton->m_2Dtris[ sm_singleton->m_2DtriCount ].v0.colour = a_colour;
			sm_singleton->m_2Dtris[ sm_singleton->m_2DtriCount ].v1.colour = a_colour;
			sm_singleton->m_2Dtris[ sm_singleton->m_2DtriCount ].v2.colour = a_colour;

			sm_singleton->m_2DtriCount++;
		}
	}
}

void Gizmos::draw(const glm::mat4& a_projectionView)
{
	if ( sm_singleton != nullptr && (sm_singleton->m_lineCount > 0 || sm_singleton->m_triCount > 0 || sm_singleton->m_transparentTriCount > 0))
	{
		glUseProgram(sm_singleton->m_shader);
		
		unsigned int projectionViewUniform = glGetUniformLocation(sm_singleton->m_shader,"ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionView));

		if (sm_singleton->m_lineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_lineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_lineCount * sizeof(GizmoLine), sm_singleton->m_lines);

			glBindVertexArray(sm_singleton->m_lineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_lineCount * 2);
		}

		if (sm_singleton->m_triCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_triVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_triCount * sizeof(GizmoTri), sm_singleton->m_tris);

			glBindVertexArray(sm_singleton->m_triVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_triCount * 3);
		}

		if (sm_singleton->m_transparentTriCount > 0)
		{
			// not ideal to store these, but Gizmos must work stand-alone
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);
			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
			int src, dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			// setup blend states
			if (blendEnabled == GL_FALSE)
				glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_transparentTriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_transparentTriCount * sizeof(GizmoTri), sm_singleton->m_transparentTris);

			glBindVertexArray(sm_singleton->m_transparentTriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_transparentTriCount * 3);

			// reset state
			glDepthMask(depthMask);
			glBlendFunc(src, dst);
			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(0);
	}
}

void Gizmos::draw2D(const glm::mat4& a_projection)
{
	if ( sm_singleton != nullptr && (sm_singleton->m_2DlineCount > 0 || sm_singleton->m_2DtriCount > 0))
	{
		glUseProgram(sm_singleton->m_shader);
		
		unsigned int projectionViewUniform = glGetUniformLocation(sm_singleton->m_shader,"ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projection));

		if (sm_singleton->m_2DlineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DlineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DlineCount * sizeof(GizmoLine), sm_singleton->m_2Dlines);

			glBindVertexArray(sm_singleton->m_2DlineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_2DlineCount * 2);
		}

		if (sm_singleton->m_2DtriCount > 0)
		{
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);

			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

			int src, dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			if (blendEnabled == GL_FALSE)
				glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DtriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DtriCount * sizeof(GizmoTri), sm_singleton->m_2Dtris);

			glBindVertexArray(sm_singleton->m_2DtriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_2DtriCount * 3);

			glDepthMask(depthMask);

			glBlendFunc(src, dst);

			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(0);
	}
}