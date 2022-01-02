#include "MassPoint.h"

glm::vec3 MassPoint::getGravityVector()
{
	glm::vec3 returnVal;
	returnVal.x = 0;
	returnVal.y = -9.82 * mass;
	returnVal.z = 0;
	return returnVal;
}

void MassPoint::update(float deltaTime)
{
	// Reset force
	force.x = 0;
	force.y = 0;
	force.z = 0;

	force += getGravityVector();

	velocity += (force * deltaTime) / mass;

	position += velocity * deltaTime;
}