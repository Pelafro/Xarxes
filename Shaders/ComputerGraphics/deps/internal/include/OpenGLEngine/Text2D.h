#pragma once
#include <glm/vec2.hpp>
#include <SDL2\SDL.h>
#include <SDL2\SDL_ttf.h>
#pragma comment(lib, "SDL2_ttf.lib")

namespace OpenGLEngine {

	class Text2D {
	public:
		SDL_Texture *texture{ nullptr };
		glm::ivec2 position;
		int width, height;
		Text2D() = default;
		virtual ~Text2D() { SDL_DestroyTexture(texture); }
		void Load(const std::string &msg, const SDL_Color &color, SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font) {
			if (!msg.empty()) {
				SDL_Surface *textureSurface = TTF_RenderText_Blended(font, msg.c_str(), color);
				texture = SDL_CreateTextureFromSurface(renderer, textureSurface);
				width = textureSurface->w;
				height = textureSurface->h;
				SDL_FreeSurface(textureSurface);
			}
		}
		void Draw(SDL_Renderer *renderer) const {
			SDL_Rect destRect = { position.x, position.y, width, height };
			SDL_RenderCopy(renderer, texture, nullptr, &destRect);
		}
	};

}