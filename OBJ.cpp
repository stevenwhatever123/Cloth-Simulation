#include "OBJ.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/glut.h>
#include <fstream>
#include <iostream>
#include <iomanip>

MassPoint::MassPoint(glm::vec3 vertex, float mass)
{
	this->position = vertex;
	this->velocity = glm::vec3(0, 0, 0);
	this->force = glm::vec3(0, 0, 0);
	this->mass = mass;

	springForce = glm::vec3(0, 0, 0);
	closestPoint = glm::vec3(std::numeric_limits<float>::max(), 
		std::numeric_limits<float>::max(), 
		std::numeric_limits<float>::max());
}

// Default Constructor
MassPoint::MassPoint()
{
	this->position = glm::vec3(0, 0, 0);
	this->velocity = glm::vec3(0, 0, 0);
	this->force = glm::vec3(0, 0, 0);
	this->mass = 2;
	this->windForce = glm::vec3(0, 0, 0);
}

// Return true if the point is inside the polygon mesh's bounding box
bool MassPoint::boundingBoxDetection(PolygonMesh p)
{
	bool insideX = this->position.x > p.minX && this->position.x < p.maxX;
	bool insideY = this->position.y > p.minY && this->position.y < p.maxY;
	bool insideZ = this->position.z > p.minZ && this->position.z < p.maxZ;

	return insideX && insideY && insideZ;
}

glm::vec3 MassPoint::getNearestPointOnEdge(glm::vec3 vertexA,
	glm::vec3 vertexB,
	glm::vec3 currentPoint)
{
	if (currentPoint == vertexA)
		return vertexA;
	if (currentPoint == vertexB)
		return vertexB;

	glm::vec3 line = (vertexB - vertexA);
	float lineLength = glm::length(line);
	glm::vec3 lineNormalised = glm::normalize(line);

	glm::vec3 v = currentPoint - vertexA;
	float d = glm::dot(v, lineNormalised);

	// Do math clamping
	d = std::max(0.0f, std::min(d, lineLength));
	return vertexA + lineNormalised * d;
}

bool MassPoint::checkIntersection(PolygonMesh p)
{
	if (boundingBoxDetection(p))
	{
		// We now assume there's only two type of polygon
		// 1. Cube 2. Icosphere

		// Collision for Cube
		if (strcmp(p.polygonType.c_str(), "Cube") == 0)
		{
			// Ignore x and z axis because we won't use it in the assignment

			float yMinDistance = std::abs(position.y - p.minY);
			float yMaxDistance = std::abs(position.y - p.maxY);

			float yCloset = yMinDistance > yMaxDistance ? yMaxDistance : yMinDistance;

			closestPoint = glm::vec3(
				position.x, 
				position.y + yCloset, 
				position.z);

			return true;
		}
		
		// Collision for Sphere or icosphere
		if (strcmp(p.polygonType.c_str(), "Icosphere") == 0
			|| strcmp(p.polygonType.c_str(), "Sphere") == 0)
		{
			// Check if the point is inside the sphere
			if (glm::distance(position, p.centerCoord) < p.radius)
			{
				int n = p.faceVertices.size();

				// Find closest point
				for (int i = 0; i < n - 1; i++)
				{
					glm::vec3 pointA = p.vertices[p.faceVertices[i]];
					glm::vec3 pointB = p.vertices[p.faceVertices[i + 1]];

					if ((i + 1) % 3 == 0)
					{
						pointA = p.vertices[p.faceVertices[i]];
						pointB = p.vertices[p.faceVertices[i - 2]];
					}

					glm::vec3 nearestPoint = getNearestPointOnEdge(pointA, pointB, position);
					if (glm::distance(position, nearestPoint)
						< glm::distance(position, closestPoint))
					{
						closestPoint = nearestPoint;
					}
				}
				return true;
			}
		}
	}
	return false;
}

void MassPoint::pushPoint()
{
	/*std::cout << "Current Position: " << this->position.x << " "
		<< this->position.y << " " << this->position.z << "\n";*/

	glm::vec3 pushVector = closestPoint - this->position;
	// Normalise the push vector
	//pushVector = glm::normalize(pushVector);
	//this->position = closestPoint;
	//this->velocity = this->velocity * -restCoef;

	this->position = closestPoint + (pushVector * restCoef);
	this->velocity = this->velocity * -restCoef;
} 

void MassPoint::update(float deltaTime)
{
	// Reset force
	force.x = 0;
	force.y = 0;
	force.z = 0;

	glm::vec3 gravity(0, -9.8, 0);

	force += gravity * mass + springForce + windForce;

	velocity += (force * deltaTime) / mass;

	position += velocity * deltaTime;
}

Spring::Spring(MassPoint a, MassPoint b, float ks, float kd)
{
	this->pointA = a;
	this->pointB = b;
	this->ks = ks;
	//this->restLength = restLength;
	this->kd = kd;

	this->restLength = glm::distance(pointB.position, pointA.position);
	//std::cout << "Rest Length: " << restLength << "\n";

	// Find the distance between two points and the rest length
	float differenceBetweenTwoLength = glm::distance(pointB.position, pointA.position) - this->restLength;

	this->springForce = this->ks * differenceBetweenTwoLength;

	this->dampForce = glm::dot(glm::normalize(pointB.position - pointA.position), (pointB.velocity - pointA.velocity)) * kd;

	this->totalForce = springForce + dampForce;
}

void Spring::update()
{
	// Find the distance between two points and the rest length
	float differenceBetweenTwoLength = glm::distance(pointB.position, pointA.position) - this->restLength;

	glm::vec3 directionA = glm::normalize(pointB.position - pointA.position);

	springForce1 = ks * (differenceBetweenTwoLength)*directionA;

	float length = glm::dot((pointB.velocity - pointA.velocity), directionA);
	dampForce1 = kd * length * directionA;

	totalForce1 = springForce1 + dampForce1;
	totalForce2 = -totalForce1;
}

OBJ::OBJ()
{

}

bool OBJ::loadOBJ(const char* path)
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
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		minDistancesBetweenVertex.push_back(-1);
		secondMinDistancesBetweenVertex.push_back(-1);
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		float minDistance = std::numeric_limits<float>::max();
		float secondMinDistance = std::numeric_limits<float>::max();
		for (int j = 0; j < vertices.size(); j++)
		{
			if (i != j)
			{
				float distance = glm::distance(vertices[i], vertices[j]);
				if (distance < minDistance)
				{
					secondMinDistance = minDistance;
					secondMinDistancesBetweenVertex[i] = secondMinDistance;

					minDistance = distance;
					minDistancesBetweenVertex[i] = minDistance;
				}
				else if (distance < secondMinDistance && distance != minDistance)
				{
					secondMinDistance = distance;
					secondMinDistancesBetweenVertex[i] = secondMinDistance;
				}
			}
		}
	}

	// We're gonna cheat because of an unknown bug that
	// it won't work on large object
	float maxSecondMin = 0;
	for (int i = 0; i < vertices.size(); i++)
	{
		if (secondMinDistancesBetweenVertex[i] > maxSecondMin)
		{
			maxSecondMin = secondMinDistancesBetweenVertex[i];
		}
	}

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		std::vector<unsigned int> minCol;
		std::vector<unsigned int> secondMinCol;
		for (unsigned int j = 0; j < vertices.size(); j++)
		{
			if (fabs(glm::distance(vertices[i], vertices[j]) - minDistancesBetweenVertex[i])
				< 0.001)
			{
				minCol.push_back(j);
			}

			if (fabs(glm::distance(vertices[i], vertices[j]) - maxSecondMin)
				<0.001)
			{
				secondMinCol.push_back(j);
			}
		}
		minDistanceNeighbour.push_back(minCol);
		secondMinDistanceNeighbour.push_back(secondMinCol);
	}

	// Convert to MassPoint
	for (int i = 0; i < vertices.size(); i++)
	{
		// Initialise all mass to 2
		MassPoint point(vertices[i], 2);
		allPoints.push_back(point);
	}

	for (int i = 0; i < minDistanceNeighbour.size(); i++)
	{
		for (int j = 0; j < minDistanceNeighbour[i].size(); j++)
		{
			// Check not creating duplicates
			bool duplicates = false;
			for (int x = 0; x < allSpring.size(); x++)
			{
				if (allSpring[x].indexA == minDistanceNeighbour[i][j] &&
					allSpring[x].indexB == i)
				{
					duplicates = true;
				}
			}

			if (!duplicates)
			{
				Spring spring(allPoints[i], allPoints[minDistanceNeighbour[i][j]], 100, 5);
				spring.indexA = i;
				spring.indexB = minDistanceNeighbour[i][j];
				allSpring.push_back(spring);
			}
		}

		for (int j = 0; j < secondMinDistanceNeighbour[i].size(); j++)
		{
			// Check not creating duplicates
			bool duplicates = false;
			for (int x = 0; x < allSpring.size(); x++)
			{
				if (allSpring[x].indexA == secondMinDistanceNeighbour[i][j] &&
					allSpring[x].indexB == i)
				{
					duplicates = true;
				}
			}

			if (!duplicates)
			{
				Spring spring(allPoints[i], allPoints[secondMinDistanceNeighbour[i][j]], 100, 5);
				spring.indexA = i;
				spring.indexB = secondMinDistanceNeighbour[i][j];
				allSpring.push_back(spring);
			}
		}
	}
}

void OBJ::performTask5(bool task5)
{
	this->task5 = task5;
	for (int i = 0; i < allPoints.size(); i++)
	{
		if (allPoints[i].position.x < minX)
			minX = allPoints[i].position.x;

		if (allPoints[i].position.x > maxX)
			maxX = allPoints[i].position.x;

		if (allPoints[i].position.y < minY)
			minY = allPoints[i].position.y;

		if (allPoints[i].position.y > maxY)
			maxY = allPoints[i].position.y;

		if (allPoints[i].position.z < minZ)
			minZ = allPoints[i].position.z;

		if (allPoints[i].position.z > maxZ)
			maxZ = allPoints[i].position.z;
	}

	windForce = glm::vec3(0, 0, 0);
	for (int i = 0; i < allPoints.size(); i++)
	{
		allPoints[i].windForce = windForce;
	}
}

void OBJ::performTask6(bool task6)
{
	this->task6 = task6;

	if (this->task6)
	{
		windForce = glm::vec3(10.0, 0, 0);
	}
	else
		windForce = glm::vec3(0, 0, 0);

	for (int i = 0; i < allPoints.size(); i++)
	{
		allPoints[i].windForce = windForce;
	}
}

void OBJ::update(float deltaTime)
{
	// Update Spring
	for (int i = 0; i < allSpring.size(); i++)
	{
		// Also reset springforce
		allPoints[allSpring[i].indexA].springForce = glm::vec3(0.0, 0.0, 0.0);
		allPoints[allSpring[i].indexB].springForce = glm::vec3(0.0, 0.0, 0.0);

		allSpring[i].pointA = allPoints[allSpring[i].indexA];
		allSpring[i].pointB = allPoints[allSpring[i].indexB];
	}

	for (int i = 0; i < allSpring.size(); i++)
	{
		allSpring[i].update();
	}

	for (int i = 0; i < allSpring.size(); i++)
	{
		allPoints[allSpring[i].indexA].springForce += allSpring[i].totalForce1;

		allPoints[allSpring[i].indexB].springForce += allSpring[i].totalForce2;
	}

	for (int i = 0; i < allPoints.size(); i++)
	{
		// Reset closestPoint
		allPoints[i].closestPoint = glm::vec3(std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max());

		// If we're running task5
		// We don't update position and velocity
		// of the two vertex in the corner
		if (task5)
		{
			//Don't update the selected two corners
			if ((allPoints[i].position.x == minX &&
				allPoints[i].position.y == maxY))
			{
				continue;
			}
			else
			{
				// Update Position
				allPoints[i].update(deltaTime);
			}
		}
		else
		{
			// Update Position
			allPoints[i].update(deltaTime);
		}
	}

	render();
}

void OBJ::update(float deltaTime, PolygonMesh p)
{
	// Update Spring
	for (int i = 0; i < allSpring.size(); i++)
	{
		// Also reset springforce
		allPoints[allSpring[i].indexA].springForce = glm::vec3(0.0, 0.0, 0.0);
		allPoints[allSpring[i].indexB].springForce = glm::vec3(0.0, 0.0, 0.0);

		allSpring[i].pointA = allPoints[allSpring[i].indexA];
		allSpring[i].pointB = allPoints[allSpring[i].indexB];
	}

	for (int i = 0; i < allSpring.size(); i++)
	{
		allSpring[i].update();
	}

	for (int i = 0; i < allSpring.size(); i++)
	{
		allPoints[allSpring[i].indexA].springForce += allSpring[i].totalForce1;

		allPoints[allSpring[i].indexB].springForce += allSpring[i].totalForce2;
	}

	for (int i = 0; i < allPoints.size(); i++)
	{
		// Reset closestPoint
		allPoints[i].closestPoint = glm::vec3(std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max());

		// Update Position
		allPoints[i].update(deltaTime);

		// Collision Detection
		if (allPoints[i].checkIntersection(p))
		{
			allPoints[i].pushPoint();
			if (p.isRotating())
			{
				glm::vec4 temp;
				temp.x = allPoints[i].position.x;
				temp.y = allPoints[i].position.y;
				temp.z = allPoints[i].position.z;
				temp.w = 1;
				temp = temp * p.rotationMatrix;

				allPoints[i].position.x = temp.x / temp.w;
				allPoints[i].position.y = temp.y / temp.w;
				allPoints[i].position.z = temp.z / temp.w;
			}
			//std::cout << i << "\n";
		}
	}

	render();
}

void OBJ::render()
{
	// Draw polygon
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < faceVertices.size() / 3; i++)
	{
		glColor3f(1.0, 0, 0);
		glVertex3f(allPoints[faceVertices[i * 3]].position.x,
			allPoints[faceVertices[i * 3]].position.y,
			allPoints[faceVertices[i * 3]].position.z);
		glVertex3f(allPoints[faceVertices[i * 3 + 1]].position.x,
			allPoints[faceVertices[i * 3 + 1]].position.y,
			allPoints[faceVertices[i * 3 + 1]].position.z);
		glVertex3f(allPoints[faceVertices[i * 3 + 2]].position.x,
			allPoints[faceVertices[i * 3 + 2]].position.y,
			allPoints[faceVertices[i * 3 + 2]].position.z);
	}
	glEnd();

	// Draw edge
	glLineWidth(0.5);
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < allSpring.size(); i++)
	{
		glColor3f(0.0, 0.0, 0.0);
		glVertex3f(allPoints[allSpring[i].indexA].position.x,
			allPoints[allSpring[i].indexA].position.y,
			allPoints[allSpring[i].indexA].position.z);
		glVertex3f(allPoints[allSpring[i].indexB].position.x,
			allPoints[allSpring[i].indexB].position.y,
			allPoints[allSpring[i].indexB].position.z);
	}
	glEnd();
	
	//// Show a specific vertex for debuging
	//glPointSize(10);
	//glBegin(GL_POINTS);
	//glColor3f(0.0, 1.0, 0.0);
	////glVertex3f(allPoints[allSpring[260].indexB].position.x,
	////	allPoints[allSpring[260].indexB].position.y,
	////	allPoints[allSpring[260].indexB].position.z);
	//glVertex3f(allPoints[305].position.x,
	//	allPoints[305].position.y,
	//	allPoints[305].position.z);
	//glEnd();
}

void OBJ::exportOBJ(std::ofstream& outfile)
{
	for (int vertex = 0; vertex < vertices.size(); vertex++)
	{
		outfile << "v " << allPoints[vertex].position.x << " "
			<< allPoints[vertex].position.y << " " << allPoints[vertex].position.z << "\n";
	}

	for (int uvIndex = 0; uvIndex < uvs.size(); uvIndex++)
	{
		outfile << "vt " << uvs[uvIndex].x << " " << uvs[uvIndex].y << "\n";
	}

	for (int normalIndex = 0; normalIndex < normals.size(); normalIndex++)
	{
		outfile << "vn " << normals[normalIndex].x << " " << normals[normalIndex].y << " " <<
			normals[normalIndex].z << "\n";
	}

	for (int face = 0; face < faceVertices.size(); face += 3)
	{
		outfile << "f ";
		for (int i = 0; i < 3; i++)
		{
			// Add one because obj is 1-based
			outfile << faceVertices[face + i] + 1 << "/" << faceUvs[face + i] + 1 << "/" << faceNormals[face + i] + 1;
			if (i != 2)
				outfile << " ";
		}
		outfile << "\n";
	}
}