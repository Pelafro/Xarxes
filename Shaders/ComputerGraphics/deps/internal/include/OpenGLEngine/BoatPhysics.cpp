#include "BoatPhysics.h"
#include "PathLoader.h"

namespace OpenGLEngine {

	// Math functions
	static float max(float a, float b) { return(a >= b) ? a : b; }
	static float min(float a, float b) { return(a <= b) ? a : b; }
	static float sign(float n) { return n = (n < 0.0f) ? -1.0f : 1.0f; }
	static float clamp(float n, float _min, float _max) { return min(max(n, _min), _max); }

	BoatPhysics::BoatPhysics() {

	}

	float BoatPhysics::applySmoothSteer(float steerInput, float dt) {
		if (abs(steerInput) > 0.001f) {	//  Move toward steering input
			steer = clamp(steer + steerInput * dt * 3.0f, -1.0f, 1.0f); // -inp.right, inp.left);
		} else {//return to 0
			if (steer > 0.0f) steer = max(steer - dt * 3.0f, 0.0f);
			else if (steer < 0.0f)	steer = min(steer + dt * 3.0f, 0.0f);
		}return steer;
	};

	float BoatPhysics::applySafeSteer(float steerInput) const {
		float avel = min(velocity, 60.0f);  // m/s
		float steer = steerInput * (1.0f - (avel / 1260.0f));
		return steer;
	};

	void BoatPhysics::processInput(bool arrayK[5], float dt) {
		throttle = arrayK[0] ? 1 : 0;
		brake = arrayK[1] ? 1 : 0;
		ebrake = arrayK[4] ? 1 : 0;
		float steerInput = 0.0f;
		if (arrayK[2]) steerInput += -1.0f;
		if (arrayK[3]) steerInput += 1.0f;
		//  Perform filtering on steering...
		if (smoothSteer) steer = applySmoothSteer(steerInput, dt);
		else steer = steerInput;
		if (safeSteer)	steer = applySafeSteer(steer);
		//  Now set the actual steering angle
		steerAngle = steer * cfg.maxSteer;
	}

	void BoatPhysics::doPhysics(float deltaTime) {
		front = glm::vec3(sin((transform->rotation.y*M_PI) / 180), 0.0f, cos((transform->rotation.y*M_PI) / 180));

		float _brake = min(brake * BRAKE_FORCE + ebrake * cfg.eBrakeForce, BRAKE_FORCE);
		float _throttle = throttle * cfg.engineForce;

		float tractionForce = _throttle - _brake * sign(velocity);
		float dragForce = -cfg.rollResist * velocity - cfg.airResist * velocity * abs(velocity);

		float totalForce = dragForce + tractionForce;
		accel = totalForce / cfg.mass;  // forward/reverse accel

		velocity += accel * deltaTime;

		if (velocity < 0.5 && throttle == 0) velocity = 0.0f;

		if (velocity > 0.0f) transform->rotation.y += -(float((steerAngle*180.0f) / M_PI))*1.5f *deltaTime;

		glm::vec2 newPos = glm::vec2((transform->position + front* velocity *deltaTime).x, (transform->position + front* velocity *deltaTime).z);
		glm::vec2 front2 = glm::normalize(glm::vec2(front.x, front.z));
		glm::vec2 pFront2 = glm::vec2(-front2.y, front2.x);
		glm::vec2 positionsCol[4];
		positionsCol[0] = newPos + front2*2.0f + pFront2*1.25f;	positionsCol[1] = newPos + front2*2.0f - pFront2*1.25f;
		positionsCol[2] = newPos + front2*2.0f + pFront2*1.25f;	positionsCol[3] = newPos + front2*2.0f - pFront2*1.25f;

		
			transform->position = glm::vec3(newPos.x, 0.0f, newPos.y);
		
	}

	void BoatPhysics::Update(bool arrayK[5], float deltaTime) {
		processInput(arrayK, deltaTime);
		doPhysics(deltaTime);
	}


}