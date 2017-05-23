#pragma once
#include <string>
#include <OpenGLEngine\IScreen.h>
#include <OpenGLEngine\Text2D.h>
#pragma comment(lib, "OpenGLEngine.lib")
using namespace OpenGLEngine;

#define TEXT_WIDTH 700
#define TEXT_HEIGHT 100
#define TEXT_MAX_SIZE 3

class App;
class LoginScreen : public IScreen{
public:
	explicit LoginScreen() = default;
	~LoginScreen() = default;

	void Build(void) override;
	void Destroy(void) override;
	void OnEntry(void) override;
	void OnExit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	App *m_app; // Casted client main game pointer reference (IApp -> AppClient)
	std::string m_nickText{ "" };
	float m_counterSend{ 0 };
	// Text to show on screen
	Text2D m_loginMessage;
	Text2D m_loadMessage;
	Text2D m_userInput;
};

