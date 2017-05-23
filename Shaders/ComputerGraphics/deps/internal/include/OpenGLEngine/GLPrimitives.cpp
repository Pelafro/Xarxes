#include "GLPrimitives.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <SDL2/SDL_ttf.h>
#pragma comment(lib, "SDL2_ttf.lib")

namespace OpenGLEngine {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		Vertex(glm::vec3 position_, glm::vec3 normal_, glm::vec2 uv_) : position(position_), normal(normal_), uv(uv_) {}
	};

	GLCube::GLCube() {
		glm::vec2 uv1 = { 0, 0 };
		glm::vec2 uv2 = { 1, 0 };
		glm::vec2 uv3 = { 0, 1 };
		glm::vec2 uv4 = { 1, 1 };

		glm::vec3 normal1 = glm::vec3(0.0f, 0.0f, -1.0f); //bot
		glm::vec3 normal2 = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 normal3 = glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 normal4 = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 normal5 = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 normal6 = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 mNormal1 = glm::normalize(normal2 + normal3 + normal4);
		glm::vec3 mNormal2 = glm::normalize(normal2 + normal4 + normal5);
		glm::vec3 mNormal3 = glm::normalize(normal2 + normal3 + normal6);
		glm::vec3 mNormal4 = glm::normalize(normal2 + normal5 + normal6);
		glm::vec3 mNormal5 = glm::normalize(normal1 + normal3 + normal4);
		glm::vec3 mNormal6 = glm::normalize(normal1 + normal4 + normal5);
		glm::vec3 mNormal7 = glm::normalize(normal1 + normal3 + normal6);
		glm::vec3 mNormal8 = glm::normalize(normal1 + normal6 + normal5);

		std::vector<Vertex> vertices;
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, -0.5f }, mNormal7, uv3);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, -0.5f }, mNormal5, uv1);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, -0.5f }, mNormal6, uv2);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, -0.5f }, mNormal6, uv2);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, -0.5f }, mNormal8, uv4);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, -0.5f }, mNormal7, uv3);

		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, -0.5f }, mNormal6, uv1);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, 0.5f }, mNormal2, uv3);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f }, mNormal4, uv4);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f }, mNormal4, uv4);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, -0.5f }, mNormal8, uv2);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, -0.5f }, mNormal6, uv1);

		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f }, mNormal4, uv4);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, 0.5f }, mNormal2, uv2);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, 0.5f }, mNormal1, uv1);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, 0.5f }, mNormal1, uv1);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, 0.5f }, mNormal3, uv3);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f }, mNormal4, uv4);

		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, -0.5f }, mNormal5, uv2);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, 0.5f }, mNormal1, uv4);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, 0.5f }, mNormal3, uv3);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, 0.5f }, mNormal3, uv3);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, -0.5f }, mNormal7, uv1);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, -0.5f }, mNormal5, uv2);

		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f }, mNormal4, uv3);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, 0.5f }, mNormal3, uv4);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, -0.5f }, mNormal7, uv2);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, -0.5f }, mNormal7, uv2);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, -0.5f }, mNormal8, uv1);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f }, mNormal4, uv3);

		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, 0.5f }, mNormal2, uv4);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, 0.5f }, mNormal1, uv3);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, -0.5f }, mNormal5, uv1);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, -0.5f }, mNormal5, uv1);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, -0.5f }, mNormal6, uv2);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, 0.5f }, mNormal2, uv4);

		///Create VAO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		///Create VBO
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
		///Configure vertex input
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(2); // uv
		glBindVertexArray(0);
	}
	void GLCube::Draw(ShaderProgram &program) const {
		glUniform1i(program.getUniformLocation("material.diffuse"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glm::mat4 m;
		m = glm::translate(m, position);
		glUniformMatrix4fv(program.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(m));

		glDisable(GL_CULL_FACE);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glEnable(GL_CULL_FACE);
	}

	GLText::GLText() {
		glm::vec2 uv1 = { 0, 0 };
		glm::vec2 uv2 = { 1, 0 };
		glm::vec2 uv3 = { 0, 1 };
		glm::vec2 uv4 = { 1, 1 };

		std::vector<Vertex> vertices;
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, uv1);
		vertices.emplace_back(glm::vec3{ 0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, uv2);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, uv4);
		vertices.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, uv4);
		vertices.emplace_back(glm::vec3{ -0.5f, 0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, uv3);
		vertices.emplace_back(glm::vec3{ -0.5f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, uv1);

		///Create VAO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		///Create VBO
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
		///Configure vertex input
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(2); // uv
		glBindVertexArray(0);

		glGenTextures(1, &textureid);
		glBindTexture(GL_TEXTURE_2D, textureid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GLText::SetText(const std::string& str, const SDL_Color &color, TTF_Font *font) const {
		SDL_Surface *surf = TTF_RenderUTF8_Blended(font, str.c_str(), color);
		//int p = int(pow(2, ceil(log(std::max(surf->w, surf->h)) / log(2))));
		SDL_Surface* ns = SDL_CreateRGBSurface(SDL_SWSURFACE, surf->w, surf->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_BlitSurface(surf, nullptr, ns, nullptr);
		SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_NONE);
		glBindTexture(GL_TEXTURE_2D, textureid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ns->w, ns->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ns->pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
		//SDL_SaveBMP(surf, GetPathToAsset("blended.bmp").c_str());
		SDL_FreeSurface(surf);
	}

	void GLText::Draw(ShaderProgram &program) const {
		glm::mat4 model;
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), { 1,0,0 });
		model = glm::rotate(model, glm::radians(rotation.y), { 0,1,0 });
		model = glm::rotate(model, glm::radians(rotation.z + 180), { 0,0,1 });
		model = glm::scale(model, scale);
		glUniformMatrix4fv(program.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));

		glDisable(GL_CULL_FACE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureid);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36); // Draw plane with 36 vertices
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(0);
		glEnable(GL_CULL_FACE);

	}
}
