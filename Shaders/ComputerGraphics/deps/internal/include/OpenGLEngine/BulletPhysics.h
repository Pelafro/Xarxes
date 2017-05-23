#pragma once
#include "Transform.h"
#include <glm/glm.hpp>
#include "GameObject.h"
#include "EnemyPhysics.h"
#include <iostream>

namespace OpenGLEngine {

# define M_PI           3.14159265358979323846  /* pi */
	class BulletPhysics : public GameObject
	{
	public:
		bool enable = false;
		float velocity = 500.0f;
		float radius = 4;
		glm::vec3 front;

		BulletPhysics();
		~BulletPhysics();

		void init(Transform positionShip);
		void doPhysics(float deltaTime);

		bool isCollided(EnemyPhysics *enemy);
	};
}

