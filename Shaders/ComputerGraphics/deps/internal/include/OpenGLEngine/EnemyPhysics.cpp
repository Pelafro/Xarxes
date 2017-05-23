#include "EnemyPhysics.h"


namespace OpenGLEngine {
	EnemyPhysics::EnemyPhysics()
	{
	}


	EnemyPhysics::~EnemyPhysics()
	{
	}


	void EnemyPhysics::init()
	{
		transform.position = glm::vec3(rand() % mapSize +1 + (-mapSize /2), 30, rand() % mapSize + 1 + (-mapSize / 2));
	}

}