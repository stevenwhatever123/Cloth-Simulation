#pragma once
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include "PolygonMesh.h"

class MassPoint
{
public:
	MassPoint(glm::vec3 vertex, float mass);
	MassPoint();
	//~MassPoint();

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;

	glm::vec3 springForce;
	float mass;

	glm::vec3 closestPoint;

	glm::vec3 windForce;

	// Restitution Coefficient
	// The amount of energy is lost when bouncing out collision
	float restCoef = 0.2f;

public:
	bool boundingBoxDetection(PolygonMesh p);
	glm::vec3 getNearestPointOnEdge(glm::vec3 vertexA, glm::vec3 vertexB, glm::vec3 currentPoint);
	bool checkIntersection(PolygonMesh p);
	void pushPoint();
	void update(float deltaTime);
};

class Spring
{
public:
	Spring(MassPoint a, MassPoint b, float ks, float kd);
	Spring();

	int indexA, indexB;

	float distance;

	MassPoint pointA, pointB;
	// Stiffness
	float ks;
	float restLength;
	// Damping factor
	float kd;

	float springForce;

	float dampForce;

	float totalForce;

	glm::vec3 springForce1;
	glm::vec3 springForce2;

	glm::vec3 dampForce1;
	glm::vec3 dampForce2;

	glm::vec3 totalForce1;
	glm::vec3 totalForce2;

public:
	void update();
};

class OBJ
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<unsigned int> normalIndices;

	std::vector<unsigned int> faceVertices;
	std::vector<unsigned int> faceUvs;
	std::vector<unsigned int> faceNormals;

	std::vector<float> minDistancesBetweenVertex;
	std::vector<float> secondMinDistancesBetweenVertex;
	
	std::vector <std::vector<unsigned int>> minDistanceNeighbour;
	std::vector <std::vector<unsigned int>> secondMinDistanceNeighbour;

	std::vector<MassPoint> allPoints;
	std::vector<Spring> allSpring;

	//float restLength = 1.41338f;
	float restLength;

	bool task5 = false;
	bool task6 = false;
	glm::vec3 windForce;
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float minZ = std::numeric_limits<float>::max();

	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();
	float maxZ = std::numeric_limits<float>::lowest();

public:
	OBJ();
	~OBJ();

	bool loadOBJ(const char* path);
	void performTask5(bool task5);
	void performTask6(bool task6);
	void update(float deltaTime);
	void update(float deltaTime, PolygonMesh p);
	void render();
	void exportOBJ(std::ofstream& outfile);
};