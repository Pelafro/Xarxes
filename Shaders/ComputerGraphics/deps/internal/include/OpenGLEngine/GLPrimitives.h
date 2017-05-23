#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2\SDL_ttf.h>
#pragma comment(lib, "SDL2_ttf.lib")
#include "ShaderProgram.h"

namespace OpenGLEngine {
class GLCube {
	public:
		GLuint vao, vbo, textureID{0};
		glm::vec3 position{ 0,0,0 };
		GLCube();
		void Draw(ShaderProgram &program) const;
	};

	class GLText {
	public:
		GLuint textureid;
		GLuint vao, vbo;
		glm::vec3 position{ 0,0,0 };
		glm::vec3 rotation{ 0,0,0 };
		glm::vec3 scale{ 0,0,0 };
		GLText();
		~GLText() = default;
		void SetText(const std::string &str, const SDL_Color &color, TTF_Font *font) const;
		void Draw(ShaderProgram &program) const;
	};

}


