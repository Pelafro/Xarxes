#include <OpenGLEngine/PathLoader.h>
#include <OpenGLEngine/Utils.h>
#include <OpenGLEngine/Timing.h>
#include <OpenGLEngine/IScreen.h>
#include "App.h"
#include <ctime>


void App::Init(void) {
	InitSDL(); // Initialize everything related to SDL for the window
	unsigned int flags = 0;
	//if (AskUserForWindow() == 0) flags = WindowFlags::RESIZABLE; // Create default window resizable
	//else flags = WindowFlags::FULLSCREEN; // Create default window fullscreen
	window.create("Cannon Piracy Unleashed", &screenWidth, &screenHeight, flags);
	renderer = SDL_CreateRenderer(window.SDLWindow, 0, SDL_RENDERER_ACCELERATED);
	font = TTF_OpenFont(GetPathToAsset("fonts/ARIAL.TTF").c_str(), FONT_SIZE);
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
	InitOpenGL(); // Initialize OpenGL systems after GLWindow creation

	// Add the screens of the derived app into the list
	m_menuScreen = std::make_unique<MenuScreen>();
	m_screenList->AddScreen(m_menuScreen.get(), SCREEN_MENU);
	m_loginScreen = std::make_unique<LoginScreen>();
	m_screenList->AddScreen(m_loginScreen.get(), SCREEN_LOGIN);
	m_gameplayScreen = std::make_unique<GameScreen>();
	m_screenList->AddScreen(m_gameplayScreen.get(), SCREEN_GAME);
	m_currentScreen = m_screenList->SetScreen(SCREEN_MENU);
}

void App::LoadAssets(void) {
	clock_t bench = clock();
	std::cout << "==== LOAD ASSETS BEGIN ====" << std::endl;
	//assetManager.LoadMesh("mesh_skybox", "models/skybox/skybox.obj");
	//assetManager.LoadMesh("mesh_skybox_bot", "models/skybox/skyboxbot.obj");
	assetManager.LoadMesh("mesh_plane", "models/sea/plane.obj");
	assetManager.LoadMesh("mesh_kop", "models/boat/kop.obj");
	assetManager.LoadMesh("mesh_column", "models/bullet/column.obj");
	//assetManager.LoadMesh("mesh_enemy", "models/enemy/enemy.obj");
	std::cout << "Meshes loaded (" << ((clock() - bench) / 1000.0f) << " seconds)" << std::endl;
	bench = clock();

	assetManager.LoadMaterial("material_red", "images/plch/red.jpg");
	assetManager.LoadMaterial("material_green", "images/plch/green.jpg");
	assetManager.LoadMaterial("material_blue", "images/plch/blue.jpg");
	//assetManager.LoadMaterial("material_skybox", "models/skybox/skybox_diffuse.jpg");
	assetManager.LoadMaterial("material_sea", "models/sea/sea_diffuse.jpg");
	assetManager.LoadMaterial("material_kop", "models/boat/kop_diffuse.jpg");
	assetManager.LoadMaterial("material_column", "models/bullet/column_diffuse.jpg");
	//assetManager.LoadMaterial("material_enemy", "models/enemy/enemy_diffuse.jpg");
	std::cout << "Materials loaded (" << ((clock() - bench) / 1000.0f) << " seconds)" << std::endl;
	bench = clock();

	std::cout << "Circuit loaded (" << ((clock() - bench) / 1000.0f) << " seconds)" << std::endl;
	std::cout << "==== LOAD ASSETS END ====" << std::endl;
}

void App::ChangeScreen(int index) {
	m_currentScreen = m_screenList->SetScreen(index);
}

void App::OnSDLEvent(SDL_Event & evnt) {
	switch (evnt.type) { // Check for SDL event type
		case SDL_QUIT:
		m_currentScreen->currentState = ScreenState::EXIT; // Set screen state to exit application
		break; case SDL_MOUSEMOTION:
		inputManager.mouseCoords = { static_cast<float>(evnt.motion.x), static_cast<float>(evnt.motion.y) }; // Store the mouse coordinates each time mouse moves through the screen
		break; case SDL_KEYDOWN:
		inputManager.pressKey(evnt.key.keysym.sym); // Store which key has been pressed
		break; case SDL_KEYUP:
		inputManager.releaseKey(evnt.key.keysym.sym); // Store which key has been released
		break; case SDL_MOUSEBUTTONDOWN:
		inputManager.pressKey(evnt.button.button); // Store when mouse button is pressed
		break; case SDL_MOUSEBUTTONUP:
		inputManager.releaseKey(evnt.button.button); // Store when mouse button is released
		break; case SDL_MOUSEWHEEL:
		inputManager.zoom = evnt.wheel.y;
	}
}



void App::Update(void) {
	//if (m_currentScreen) { // If current screen exists
	switch (m_currentScreen->currentState) { // Check for the state of the screen
		case ScreenState::RUNNING:
		inputManager.update();	// Update the input manager instance
		m_currentScreen->Update(); // Update the current screen if running
		break;
		case ScreenState::EXIT:
		Exit(); // Call exit function to end the execution
		break;
		case ScreenState::SLEEP: default: break;
	}
	//} else Exit(); // Call exit function if screen doesn't exist
}

void App::Draw(void) const {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color and depth buffer
	if (m_currentScreen && m_currentScreen->currentState == ScreenState::RUNNING) { // If screen object exists and its state is running
		m_currentScreen->Draw(); // Then call the draw method of the screen
	}
}

void App::Run(void) {
	Init(); // Call the init everything function
	FPSLimiter fpsLimiter; // Spawn the main instance of the timing limiter
	fpsLimiter.setTargetFPS(TARGET_FPS); // Set the frames per second we whish to have, ideally 60-120

	while (m_isRunning) { // While game is running
		fpsLimiter.begin();					// Init FPS counter
		Update();							// Main update function
		if (!m_isRunning) break;			// Break main game loop if running attribute set to false
		Draw();								// Main draw function
		fpsLimiter.end();					// Calculate and restore FPS
		fps = fpsLimiter.fps;				// Get the current fps of the class instance
		deltaTime = fpsLimiter.deltaTime;	// Get the current fps of the class instance
	}
}

void App::Exit(void) {
	m_currentScreen->OnExit(); // Call the leaving method of the current screen
	if (m_screenList) {
		m_screenList->Destroy();
		m_screenList.reset();
	}
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);
	DestroySDL();
	m_isRunning = false; // Execution ends
}