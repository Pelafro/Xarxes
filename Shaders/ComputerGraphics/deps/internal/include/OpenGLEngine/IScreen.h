#pragma once

namespace OpenGLEngine {

	#define SCREEN_INDEX_NO_SCREEN -1 // Default empty screen index

	// Possible states of a screen during the game
	enum class ScreenState { 
		NONE,		// Default empty state
		RUNNING,	// State of updating the current screen
		EXIT,		// State of leaving the current game
		SLEEP
	};

	class IApp;	// Forward declaration of the main game app interface
	// Game screen class interface to use as a template for each screen
	class IScreen {
	public:
		ScreenState currentState	{ ScreenState::NONE };		// Current state of the screen, NONE by default
		IApp* gameApp				{ nullptr };				// Reference pointer to current game app
		int screenIndex				{ SCREEN_INDEX_NO_SCREEN };	// Self screen index in game app, -1 by default

		explicit IScreen() = default;
		virtual ~IScreen() = default;

		// Called when the screen is added into an screen list
		virtual void Build() = 0;
		// Called at the end of the game app
		virtual void Destroy() = 0;

		// Called when entering into a new screen
		virtual void OnEntry() = 0;
		// Called when leaving from a screen
		virtual void OnExit() = 0;

		// Main update screen function called in game loop
		virtual void Update() = 0;
		// Main draw screen function called in game loop
		virtual void Draw() = 0;
	};

}