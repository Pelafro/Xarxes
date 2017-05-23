#pragma once

namespace OpenGLEngine {

	class FPSLimiter {
		float m_targetFPS;
		unsigned __int32 m_startTicks;
		unsigned __int32 m_frameTicks;

		void calculateFPS();
	public:
		float fps;
		float deltaTime;

		explicit FPSLimiter();
		explicit FPSLimiter(float targetFPS);
		~FPSLimiter() = default;

		void setTargetFPS(float targetFPS) { m_targetFPS = targetFPS; };
		void printFPS() const;

		void begin();
		void end();
	};

}

