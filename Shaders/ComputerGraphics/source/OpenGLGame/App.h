#pragma once
#include <OpenGLEngine/IApp.h>
#include <OpenGLEngine/GLWindow.h>
#include <OpenGLEngine/InputManager.h>
#include <OpenGLEngine/ScreenList.h>
#include <OpenGLEngine/AssetManager.h>
#include <memory>
#include "MenuScreen.h"
#include "LoginScreen.h"
#include "GameScreen.h"

#define INIT_SCREEN_WIDTH 1200
#define INIT_SCREEN_HEIGHT 900
#define FONT_SIZE 70

enum SreenType { SCREEN_MENU, SCREEN_LOGIN, SCREEN_GAME };

class App : public IApp {
	std::unique_ptr<ScreenList> m_screenList{ std::make_unique<ScreenList>(dynamic_cast<IApp*>(this)) }; // Unique pointer instance to the list of screens of the game
	std::unique_ptr<MenuScreen> m_menuScreen;
	std::unique_ptr<LoginScreen> m_loginScreen;
	std::unique_ptr<GameScreen> m_gameplayScreen;

	explicit App() = default;
	App(App const&) = delete;
	void operator=(App const&) = delete;
	~App() = default;

	// Initialize everything related to game internals
	void Init(void) override;
	// Main update function of the game
	void Update(void);
	// Main draw function of the game
	void Draw(void) const;
public:
	int screenWidth{ INIT_SCREEN_WIDTH }, screenHeight{ INIT_SCREEN_HEIGHT };
	IScreen *m_currentScreen{ nullptr }; // Reference pointer to the screen running at the moment
	GLWindow window;			// Main instance of the OpenGL window
	InputManager inputManager;	// Main instance of the input manager class
	AssetManager assetManager;
	std::string nick{ "" };
	SDL_Renderer *renderer{ nullptr };
	TTF_Font *font{ nullptr };

	static App& Instance() {
		static App instance;
		return instance;
	}
	void LoadAssets(void);
	void ChangeScreen(int index);
	// Manage main SDL event types
	void OnSDLEvent(SDL_Event &evnt);
	// Where magic occurs, to be used to play the whole game
	void Run(void) override;
	// Destroy screen list and set game running to false
	void Exit(void);
};