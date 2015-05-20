#pragma once
#include "Engine/Renderable.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

class CubeMesh : public Renderable
{
public:

	CubeMesh(float _size) : size(_size) { Create(); }

	void Create()
	{
		numberOfVerts = 8;
		numberOfIndices = 36;

		float halfSize = size / 2;

		Vertex vertices[8];
		vertices[0].position = glm::vec4(-halfSize, -halfSize, -halfSize, 1);
		vertices[1].position = glm::vec4(-halfSize, -halfSize, halfSize, 1);
		vertices[2].position = glm::vec4(halfSize, -halfSize, halfSize, 1);
		vertices[3].position = glm::vec4(halfSize, -halfSize, -halfSize, 1);
		vertices[4].position = glm::vec4(-halfSize, halfSize, -halfSize, 1);
		vertices[5].position = glm::vec4(-halfSize, halfSize, halfSize, 1);
		vertices[6].position = glm::vec4(halfSize, halfSize, halfSize, 1);
		vertices[7].position = glm::vec4(halfSize, halfSize, -halfSize, 1);

		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		vertices[0].uv = glm::vec2(0, 0);
		vertices[1].uv = glm::vec2(1, 0);
		vertices[2].uv = glm::vec2(1, 1);
		vertices[3].uv = glm::vec2(1, 0);
		vertices[4].uv = glm::vec2(0, 1);
		vertices[5].uv = glm::vec2(0, 0);
		vertices[6].uv = glm::vec2(1, 0);
		vertices[7].uv = glm::vec2(1, 1);

		unsigned int indices[36]
		{
			0, 7, 4, //front
			0, 3, 7,
			1, 4, 5, //left
			1, 0, 4,
			2, 5, 6, //back
			2, 1, 5,
			3, 6, 7, //right
			3, 2, 6,
			4, 7, 6, //top
			4, 6, 5,
			0, 2, 3, //bottom
			0, 1, 2
		};

		//generating and binding buffers
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(Vertex), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //positiong
		glEnableVertexAttribArray(1); //normal
		glEnableVertexAttribArray(2); //uv
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + sizeof(glm::vec4));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) + sizeof(glm::vec4)));

		//unbind vertex array
		glBindVertexArray(0);
	}

private:

	float size;

};