#include "Timing.h"
#include <iostream>
#include <SDL2\SDL.h>

namespace OpenGLEngine {

#define NUM_SAMPLES 10

	FPSLimiter::FPSLimiter() :
		m_targetFPS(120.0f), 
		m_startTicks(0), 
		m_frameTicks(0),
		fps(0), deltaTime(0) {}

	FPSLimiter::FPSLimiter(float targetFPS) : 
		m_targetFPS(targetFPS),
		m_startTicks(0),
		m_frameTicks(0),
		fps(0), deltaTime(0) {}

	static float max(float a, float b) { return (a<b) ? b : a; };

	void FPSLimiter::calculateFPS() {
		static float frameTimes[NUM_SAMPLES];
		static auto curFrame = 0;

		static auto prevTicks = SDL_GetTicks();
		auto curTicks = SDL_GetTicks();

		frameTimes[curFrame] = static_cast<float>(curTicks - prevTicks);
		prevTicks = curTicks;

		static auto count = 0;
		curFrame++;
		if (count < NUM_SAMPLES) count = curFrame;
		else count = NUM_SAMPLES;
		if (curFrame == NUM_SAMPLES) curFrame = 0;

		float frameTimeAverage = 0;
		for (auto i = 0; i < count; i++) frameTimeAverage += frameTimes[i];
		frameTimeAverage /= count;

		if (frameTimeAverage > 0) fps = 1000.0f / frameTimeAverage;
		else fps = 60.0f;

		deltaTime = max(1/fps, 1/m_targetFPS);
	}

	void FPSLimiter::printFPS() const {
		static auto fpsTimer = 0;
		if (fpsTimer == 10) fpsTimer = 0, std::cout << fps << std::endl;
		fpsTimer++;
	}

	void FPSLimiter::begin() {
		m_startTicks = SDL_GetTicks();
	}

	void FPSLimiter::end() {
		calculateFPS();
		m_frameTicks = SDL_GetTicks() - m_startTicks;
		if (1000.0f / m_targetFPS > m_frameTicks) SDL_Delay(static_cast<Uint32>(1000.0f / m_targetFPS - m_frameTicks));
	}

#undef NUM_SAMPLES

}
