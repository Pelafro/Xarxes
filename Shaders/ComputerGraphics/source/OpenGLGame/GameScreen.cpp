#include <OpenGLEngine/PathLoader.h>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include "GameScreen.h"
#include "App.h"

#define FIXED_ASPECT_RATIO 16 / 9

void GameScreen::Build(void) {
	m_app = dynamic_cast<App*>(gameApp);

	// Initialize camera with viewport dimensions
	int nw = (m_app->screenHeight * FIXED_ASPECT_RATIO);
	m_camera.Resize(nw + (m_app->screenWidth - nw) / 2, m_app->screenHeight);

	//Initialize texture shaders
	m_mainProgram.LoadShaders(GetPathToAsset("shaders/main.vert"), GetPathToAsset("shaders/main.frag"));
	//Initialize light shaders
	m_lightProgram.LoadShaders(GetPathToAsset("shaders/debug.vert"), GetPathToAsset("shaders/debug.frag"));
}

void GameScreen::Destroy(void) {

}

void GameScreen::OnEntry(void) {
	gameOver = false;
	//SDL_ShowCursor(0);
	indexBullet = 0;
	score = -8;
	time = 60;
	m_timeStamp = (float)(clock() + m_cooldownForTime);

	// Set player nick to text plane
	m_textUI.position = { m_player.transform.position.x, 182, m_player.transform.position.z };
	m_textUI.rotation = { 0, 0,0 };
	m_textUI.scale = { 355,15,355 };

	sea.meshRef = &m_app->assetManager.FindMesh("mesh_sea");
	sea.materialRef = &m_app->assetManager.FindMaterial("material_sea");
	m_renderer.Add(&sea);

	// Load player base kart model
	m_player.meshRef = &m_app->assetManager.FindMesh("mesh_boat");
	m_player.materialRef = &m_app->assetManager.FindMaterial("material_boat");
	m_renderer.Add(&m_player);

	// Load the enemies models
	for (int i = 0; i < MAX_BULLETS; i++) {
		m_bullets[i].meshRef = &m_app->assetManager.FindMesh("mesh_bullet");
		m_bullets[i].materialRef = &m_app->assetManager.FindMaterial("material_bullet");
		m_bullets[i].transform.position = { 0, 10000, 0 };
		m_renderer.Add(&m_bullets[i]);
	}

	// Load the enemies models
	for (int i = 0; i < MAX_ENEMIES; i++) {
		m_enemies[i].meshRef = &m_app->assetManager.FindMesh("mesh_enemy");
		m_enemies[i].materialRef = &m_app->assetManager.FindMaterial("material_enemy");
		m_enemies[i].transform.position = { 0, 10000, 0 };
		m_enemies->init();
		m_renderer.Add(&m_enemies[i]);
	}

	skybox.transform.position = { 0, -700, 0 };
	skybox.transform.scale = { 1, 1, 1};
	skybox.transform.rotation = { 0, 0, 0 };
	skybox.meshRef = &m_app->assetManager.FindMesh("mesh_skybox");
	skybox.materialRef = &m_app->assetManager.FindMaterial("material_skybox");
	skybox.materialRef->materialData->emissive = {0.15, 0.15, 0.3 };
	skybox.materialRef->materialData->specular = { 1, 1, 1 };
	m_renderer.Add(&skybox);

	skybox_bot.transform.position = { 0, -700, 0 };
	skybox_bot.transform.scale = { 1, 1, 1 };
	skybox_bot.meshRef = &m_app->assetManager.FindMesh("mesh_skybox_bot");
	skybox_bot.materialRef = &m_app->assetManager.FindMaterial("material_skybox");
	skybox_bot.materialRef->materialData->emissive = { 0.15, 0.15, 0.3 };
	skybox_bot.materialRef->materialData->specular = { 1, 1, 1 };
	m_renderer.Add(&skybox_bot);

	// Init player kart physics
	m_boat_physics.AddTransform(&m_player.transform);

	// LIGHTNING
	// Init directional light
	m_dirLight.direction = { -0.2f, -1.0f, -0.3f };
	m_dirLight.ambient = { 0.2f, 0.2f, 0.4f };
	m_dirLight.diffuse = { 0.1f, 0.1f, 0.2f };
	m_dirLight.specular = { 0.1f, 0.1f, 0.2f };
	m_renderer.Add(&m_dirLight);

	// Init point lights
	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		m_pointLights[i].position = { m_enemies[i].transform.position.x, 90, m_enemies[i].transform.position.z };
		m_pointLights[i].ambient = { 0.5f, 0.0f, 0.0f };
		m_pointLights[i].diffuse = { 1.0f, 0.0f, 0.0f };
		m_pointLights[i].specular = { 1.0f, 1.0f, 1.0f };
		m_pointLights[i].constant = 1.0f;
		m_pointLights[i].linear = 0.007f;
		m_pointLights[i].quadratic = 0.0002f;
		m_renderer.Add(&m_pointLights[i]);
	}

	// Init spot lights
	m_spotLights[0].position = { -10,60,0 };
	m_spotLights[0].direction = { 0, 0, 1 };
	m_spotLights[0].ambient = { 1.0f, 1.0f, 0.7f };
	m_spotLights[0].diffuse = { 1.0f, 1.0f, 0.7f };
	m_spotLights[0].specular = { 1.0f, 1.0f, 1.0f };
	m_spotLights[0].constant = 1.0f;
	m_spotLights[0].linear = 0.0054f;
	m_spotLights[0].quadratic = 0.00002f;
	m_spotLights[0].cutOff = glm::cos(glm::radians(25.0f));
	m_spotLights[0].outerCutOff = glm::cos(glm::radians(40.0f));
	m_renderer.Add(&m_spotLights[0]);

	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0

	// Send light and material attributes to fragment shader
	m_mainProgram.bind();
	m_renderer.SendLightAttributes(m_mainProgram, m_camera);
	m_renderer.SendMaterialAttributes(m_mainProgram, m_camera);
	m_mainProgram.unbind();

}

void GameScreen::OnExit(void) {
	m_renderer.Clear();
}



void GameScreen::UpdateBullets(float dt)
{
	if (m_app->inputManager.isKeyPressed(SDLK_SPACE))
	{
		m_bullets[indexBullet].init(m_player.transform);
		indexBullet++;
		indexBullet %= MAX_BULLETS;
	}

	for (int i = 0; i < MAX_BULLETS; i++) {
		m_bullets[i].doPhysics(dt);
	}
}

void GameScreen::UpdateUI()
{
	m_textUI.position = { m_player.transform.position.x, 182, m_player.transform.position.z };
	m_textUI.rotation = m_player.transform.rotation;
}

void GameScreen::UpdateTime()
{
	if (m_timeStamp <= clock() && time >= 0) {
		time--;
		m_timeStamp = (float)(clock() + m_cooldownForTime);
		m_textUI.SetText("    Player: " + m_app->nick + "                                   Score: " + std::to_string(score) + "        Time: " + std::to_string(time) + "    ", { 255, 255, 255 }, m_app->font);
	}else if (time <= 0){
		gameOver = true;
	}
}

void GameScreen::CheckBulletsCollisions()
{
	for (int i = 0; i < MAX_BULLETS; i++) {
		for (int j = 0; j < MAX_ENEMIES; j++) {
			if(m_bullets[i].isCollided(&m_enemies[j]))
			{
				m_enemies[j].init();
				m_bullets[i].transform.position = { 0, 10000, 0 };
				m_bullets[i].enable = false;
				score++;
				m_textUI.SetText("    Player: " + m_app->nick + "                                   Score: " + std::to_string(score) + "        Time: " + std::to_string(time) + "    ", { 255, 255, 255 }, m_app->font);
			}
		}
	}
}

void GameScreen::UpdateLights()
{	
	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		m_pointLights[i].position = { m_enemies[i].transform.position.x, 90, m_enemies[i].transform.position.z};
	}

	m_spotLights[0].position = m_player.transform.position;
	m_spotLights[0].direction = m_boat_physics.front;
}

void GameScreen::Update(void) {
	if (!gameOver) {
		static int m_inputCounter = 0;
		static input10 m_in2send;
		CheckInput();
		static bool temp[5];
		memset(temp, false, 5); // reset all elements to false
		if (m_app->inputManager.isKeyDown(SDLK_w)) temp[0] = true;
		if (m_app->inputManager.isKeyDown(SDLK_a)) temp[2] = true;
		if (m_app->inputManager.isKeyDown(SDLK_s)) temp[1] = true;
		if (m_app->inputManager.isKeyDown(SDLK_d)) temp[3] = true;
		if (m_app->inputManager.isKeyDown(SDLK_SPACE)) temp[4] = true;
		m_boat_physics.Update(temp, gameApp->deltaTime);
		
		glm::vec3 perFront = glm::vec3(-m_boat_physics.front.z, 0.0f, m_boat_physics.front.x);

		//std::cout << m_player->transform.position.x << "," << m_player->transform.position.z << std::endl;

		m_camera.Translate(m_player.transform.position - (m_boat_physics.front*450.0f) + glm::vec3(0.0f, 200.0f, 0.0f));
		m_camera.SetTarget(glm::vec3{ 0,2,0 } +m_player.transform.position);
		//	std::cout << m_player.transform.position.x << " / " << m_player.transform.position.z << std::endl;


		UpdateBullets(gameApp->deltaTime);
		UpdateLights();
		UpdateUI();
		CheckBulletsCollisions();
		UpdateTime();


		if (m_app->inputManager.isKeyPressed(SDLK_ESCAPE)) m_app->ChangeScreen(SCREEN_MENU);
	}
}

void GameScreen::CheckInput(void) {
	SDL_Event evnt;
	if (SDL_PollEvent(&evnt)) {
		m_app->OnSDLEvent(evnt);
		if (evnt.type == SDL_WINDOWEVENT) {
			switch (evnt.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
				SDL_GetWindowSize(m_app->window.SDLWindow, &m_app->screenWidth, &m_app->screenHeight);
				glViewport(0, 0, m_app->screenWidth, m_app->screenHeight); // Set the OpenGL viewport to window dimensions
				int nw = (m_app->screenHeight * FIXED_ASPECT_RATIO);
				m_camera.Resize(nw + (m_app->screenWidth - nw) / 2, m_app->screenHeight);
				break;
			}
		}
	}
	if (m_app->inputManager.isKeyPressed(SDLK_e)) RendererList::DEBUG_DRAW = !RendererList::DEBUG_DRAW;
	if (m_app->inputManager.isKeyPressed(SDLK_q)) RendererList::DEBUG_MODE = (RendererList::DEBUG_MODE == GL_TRIANGLES) ? GL_LINES : GL_TRIANGLES;
}

void GameScreen::Draw(void) {
	m_mainProgram.bind();
		m_renderer.DrawObjects(m_mainProgram, m_camera);
		m_textUI.Draw(m_mainProgram);
	m_mainProgram.unbind();

	if (RendererList::DEBUG_DRAW)
		m_lightProgram.bind(),
			m_renderer.DrawDebug(m_lightProgram, m_camera),
		m_lightProgram.unbind();

	m_app->window.swapBuffer(); // Swap OpenGL buffers if double-buffering is supported
}