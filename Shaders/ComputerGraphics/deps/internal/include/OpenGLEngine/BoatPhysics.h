#pragma once
#include "Transform.h"
#include "BulletPhysics.h"
namespace OpenGLEngine {
	
#define BRAKE_FORCE 100000.0f

	struct Config {
		float gravity = 0.0f;   // m/s^2
		float mass = 1200.0f; // kg
		float engineForce = 50000.0;
		float eBrakeForce = BRAKE_FORCE / 2.5f;
		float maxSteer = 1.0f;    // Maximum steering angle in radians
		float airResist = 4.5f;   // air resistance (* vel)
		float rollResist = 4.0f;   // rolling resistance force (* vel)
	};

	class BoatPhysics {
	public:
		Transform *transform{ nullptr };
		glm::vec3 front;
		float accel = 0.0f;
		float velocity = 0.0f;
		bool smoothSteer = true;
		bool safeSteer = true;
		Config cfg;
		int throttle = 0;
		int brake = 0;
		int ebrake = 0;
		float steer = 0.0;
		float steerAngle = 0.0;

		BoatPhysics();
		~BoatPhysics() = default;

		void AddTransform(Transform* _transform) { transform = _transform; }
		void Update(bool arrayK[5], float deltaTime);
	private:
		float applySmoothSteer(float steerInput, float dt);
		float applySafeSteer(float steerInput) const;
		void processInput(bool arrayK[5], float dt);
		void doPhysics(float deltaTime);
	};

}