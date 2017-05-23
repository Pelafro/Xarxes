#pragma once
#include <OpenGLEngine/IScreen.h>
#include <OpenGLEngine/ShaderProgram.h>
#include <OpenGLEngine/GameObject.h>
#include <OpenGLEngine/BoatPhysics.h>
#include <OpenGLEngine/GLPrimitives.h>
#include <OpenGLEngine/BulletPhysics.h>
#include <OpenGLEngine/EnemyPhysics.h>
#include "RendererList.h"
#pragma comment(lib, "OpenGLEngine.lib")
using namespace OpenGLEngine;

#define MAX_POINT_LIGHTS 9
#define MAX_SPOT_LIGHTS 1
#define MAX_ENEMIES 9
#define MAX_BULLETS 10

class App;
class GameScreen : public IScreen {
public:
	explicit GameScreen() = default;
	~GameScreen() = default;

	void Build(void) override;
	void Destroy(void) override;
	void OnEntry(void) override;
	void OnExit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void UpdateBullets(float dt);
	void UpdateUI();
	void UpdateTime();
	void CheckBulletsCollisions();
	void UpdateLights();
private:
	void CheckInput(void);

	// Casted client main game pointer reference (IApp -> AppClient)
	App *m_app;

	// Shader programs
	ShaderProgram m_mainProgram;
	ShaderProgram m_lightProgram;

	// Camera
	GLCamera m_camera;

	// Game objects
	RendererList m_renderer;
	GameObject m_player;
	BulletPhysics m_bullets[MAX_BULLETS];
	EnemyPhysics m_enemies[MAX_ENEMIES];
	int indexBullet;
	GameObject skybox;
	GameObject skybox_bot;
	GameObject sea;
	bool gameOver;
	int score;
	int time;
	float m_timeStamp;
	float m_cooldownForTime=1000;

	// Text objects
	GLText m_textUI; 

	//Game physics
	BoatPhysics m_boat_physics;

	// Lights
	DirLight m_dirLight;
	PointLight m_pointLights[MAX_POINT_LIGHTS];
	SpotLight m_spotLights[MAX_SPOT_LIGHTS];

};
