#include "BulletPhysics.h"
#include <math.h>
namespace OpenGLEngine {


	BulletPhysics::BulletPhysics()
	{
		enable = false;
	}


	BulletPhysics::~BulletPhysics()
	{
	}

	void BulletPhysics::init(Transform positionShip)
	{
		enable = true;
		transform = positionShip;
	}


	void BulletPhysics::doPhysics(float deltaTime) {
		if (enable) {
			front = glm::vec3(sin((transform.rotation.y*M_PI) / 180), 0.0f, cos((transform.rotation.y*M_PI) / 180));
			glm::vec2 newPos = glm::vec2((transform.position + front* velocity *deltaTime).x, (transform.position + front* velocity *deltaTime).z);
			transform.position = glm::vec3(newPos.x, 30.0f, newPos.y);
		}
	}

	bool BulletPhysics::isCollided(EnemyPhysics *enemy)
	{
		return glm::length((enemy->transform.position - transform.position)) <= (radius+ enemy->radius);
	}
}
