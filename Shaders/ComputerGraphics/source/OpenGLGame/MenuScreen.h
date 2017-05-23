#pragma once
#include <OpenGLEngine\IScreen.h>
#include <OpenGLEngine\Texture2D.h>
#pragma comment(lib, "OpenGLEngine.lib")
using namespace OpenGLEngine;

class App;
class MenuScreen : public IScreen {
public:
	explicit MenuScreen() = default;
	~MenuScreen() = default;

	void Build(void) override;
	void Destroy(void) override;
	void OnEntry(void) override;
	void OnExit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	App *m_app; // Casted client main game pointer reference (IApp -> AppClient)
	// Sprites to load
	Texture2D m_title;
	Button2D m_playButton;
};

