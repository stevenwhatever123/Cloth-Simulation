#pragma once
#include <glm/glm.hpp>

class MassPoint
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;
	float mass;

public:
	glm::vec3 getGravityVector();
	void update(float deltaTime);
};

