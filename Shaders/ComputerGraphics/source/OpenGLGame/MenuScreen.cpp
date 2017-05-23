#include "MenuScreen.h"
#include "App.h"
#include <OpenGLEngine\PathLoader.h>

void MenuScreen::Build(void) {
	m_app = dynamic_cast<App*>(gameApp);

	m_title.Load(0, 0, m_app->screenWidth, m_app->screenHeight, GetPathToAsset("images/title.jpg"), m_app->window.SDLWindow, m_app->renderer);


	m_playButton.Load(int(m_app->screenWidth*0.35f), int(m_app->screenHeight*0.75f),
					   int(m_app->screenWidth*0.3f), int(m_app->screenHeight*0.2f),
					   GetPathToAsset("images/play_default.png"),
					   GetPathToAsset("images/play_hover.png"),
						m_app->window.SDLWindow, m_app->renderer);
}

void MenuScreen::Destroy(void) {
	
}

void MenuScreen::OnEntry(void) {
	SDL_SetRenderDrawColor(m_app->renderer, 255, 255, 255, 255);
	m_playButton.Reset();
}

void MenuScreen::OnExit(void) {

}

void MenuScreen::Update(void) {
	SDL_Event evnt;
	if (SDL_PollEvent(&evnt)) m_app->OnSDLEvent(evnt);
	m_playButton.Update(m_app->inputManager);
	if (m_playButton.pressed) m_app->ChangeScreen((m_app->assetManager.Empty()) ? SCREEN_LOGIN : SCREEN_GAME);
	if (m_app->inputManager.isKeyPressed(SDLK_ESCAPE)) m_app->m_currentScreen->currentState = ScreenState::EXIT;
}

void MenuScreen::Draw(void) {
	SDL_RenderClear(m_app->renderer);
		m_title.Draw(m_app->renderer);
		m_playButton.Draw(m_app->renderer);
	SDL_RenderPresent(m_app->renderer);
}
