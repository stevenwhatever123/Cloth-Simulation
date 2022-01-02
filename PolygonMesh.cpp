#include "PolygonMesh.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

PolygonMesh::PolygonMesh()
{

}

bool PolygonMesh::loadOBJ(const char* path)
{
	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		std::cout << "Impossible to open the file" << "\n";
		return false;
	}

	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit loop

		// Read in data
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			//temp_vertices.push_back(vertex);
			vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				std::cout << "File can't be read by simple parser" << "\n";
				return false;
			}

			faceVertices.push_back(vertexIndex[0] - 1);
			faceVertices.push_back(vertexIndex[1] - 1);
			faceVertices.push_back(vertexIndex[2] - 1);

			faceUvs.push_back(uvIndex[0] - 1);
			faceUvs.push_back(uvIndex[1] - 1);
			faceUvs.push_back(uvIndex[2] - 1);

			faceNormals.push_back(normalIndex[0] - 1);
			faceNormals.push_back(normalIndex[1] - 1);
			faceNormals.push_back(normalIndex[2] - 1);
		}
		else if (strcmp(lineHeader, "o") == 0)
		{
			char stringBuffer[128];
			fscanf(file, "%s\n", stringBuffer);
			polygonType = stringBuffer;
			std::cout << polygonType << "\n";
		}
	}

	updateMinMax();

	if (strcmp(polygonType.c_str(), "Icosphere") == 0
		|| strcmp(polygonType.c_str(), "Sphere") == 0)
	{
		centerCoord = glm::vec3((maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2);
		//std::cout << maxX << " " << minX << "\n";
		//std::cout << centerCoord.x << " " << centerCoord.y << " " << centerCoord.z << "\n";
	}

	// Find out radius
	// Using y coordinates is easier to find
	radius = std::abs(centerCoord.y - minY); 
	std::cout << "Radius: " << radius << "\n";
}

void PolygonMesh::setRotating(bool b)
{
	rotating = b;
}

bool PolygonMesh::isRotating()
{
	return rotating;
}

void PolygonMesh::updateMinMax()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x < minX)
			minX = vertices[i].x;

		if (vertices[i].x > maxX)
			maxX = vertices[i].x;

		if (vertices[i].y < minY)
			minY = vertices[i].y;

		if (vertices[i].y > maxY)
			maxY = vertices[i].y;

		if (vertices[i].z < minZ)
			minZ = vertices[i].z;

		if (vertices[i].z > maxZ)
			maxZ = vertices[i].z;
	}
}

void PolygonMesh::updateRadius()
{
	if (strcmp(polygonType.c_str(), "Icosphere") == 0
		|| strcmp(polygonType.c_str(), "Sphere") == 0)
	{
		centerCoord = glm::vec3((maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2);
	}

	// Find out radius
	// Using y coordinates is easier to find
	radius = std::abs(centerCoord.y - minY);
}

void PolygonMesh::drawQuad(int a, int b, int c, int d)
{
	glBegin(GL_QUADS);
	glVertex3f(vertices[a].x, vertices[a].y, vertices[a].z);
	glVertex3f(vertices[b].x, vertices[b].y, vertices[b].z);
	glVertex3f(vertices[c].x, vertices[c].y, vertices[c].z);
	glVertex3f(vertices[d].x, vertices[d].y, vertices[d].z);
	glEnd();
}

void PolygonMesh::update(float deltaTime)
{
	// Rotate for task 6
	float theta;
	glm::vec3 rotationAxis(0.0, 1.0, 0.0);
	rotationMatrix = glm::rotate(0.5f * deltaTime, rotationAxis);

	for (int i = 0; i < vertices.size(); i++)
	{
		glm::vec4 currentVector(vertices[i].x, vertices[i].y, vertices[i].z, 1);
		glm::vec4 endVector = rotationMatrix * currentVector;
		vertices[i].x = endVector.x;
		vertices[i].y = endVector.y;
		vertices[i].z = endVector.z;
	}

	updateMinMax();
	updateRadius();
}

void PolygonMesh::render()
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < faceVertices.size() / 3; i++)
	{
		glVertex3f(vertices[faceVertices[i * 3]].x, vertices[faceVertices[i * 3]].y,
			vertices[faceVertices[i * 3]].z);
		glVertex3f(vertices[faceVertices[i * 3 + 1]].x, vertices[faceVertices[i * 3 + 1]].y,
			vertices[faceVertices[i * 3 + 1]].z);
		glVertex3f(vertices[faceVertices[i * 3 + 2]].x, vertices[faceVertices[i * 3 + 2]].y,
			vertices[faceVertices[i * 3 + 2]].z);
	}
	glEnd();
}