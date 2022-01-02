#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <limits>
class PolygonMesh
{
public:
	PolygonMesh();

	// Property of the polygon mesh
	// 1 = floor, 2 = cube
	std::string polygonType = "";
	glm::vec3 centerCoord;
	float radius;

	bool rotating = false;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> edges;
	
	std::vector<unsigned int> faceVertices;
	std::vector<unsigned int> faceUvs;
	std::vector<unsigned int> faceNormals;

	// For bounding box
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float minZ = std::numeric_limits<float>::max();

	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();
	float maxZ = std::numeric_limits<float>::lowest();

	glm::mat4 rotationMatrix;

	bool loadOBJ(const char *path);
	void setRotating(bool b);
	bool isRotating();
	void updateMinMax();
	void updateRadius();
	void drawQuad(int a, int b, int c, int d);
	void update(float deltaTime);
	void render();
};

