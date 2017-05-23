#pragma once

namespace OpenGLEngine {

#define TARGET_FPS 120.0f

	enum KEY_MESSAGE {
		MSG_LOGIN, MSG_BEGIN, MSG_EXIT, MSG_ALIVE, MSG_UPDATE, MSG_NEW_PLAYER
	};

	struct input10 {
		bool w[10];
		bool a[10];
		bool s[10];
		bool d[10];
		float dt[10];
	};

	// Game app interface to the store the main runing game engine as a template
	class IApp {
	protected:
		bool m_isRunning {true};			// Whether game is running or not
	public:
		float fps{ 0 };			// How many frames per second the game is running to
		float deltaTime{ 0 };	// Difference between last frame and new frame

		explicit IApp() = default;
		virtual ~IApp() = default;

		// Initialize everything related to game internals
		virtual void Init(void) = 0;
		// Where magic occurs, to be used to play the whole game
		virtual void Run(void) = 0;
	};

}
