#pragma once
#include <string>
#include <SDL2\SDL.h>

namespace OpenGLEngine {
	#define SP_THROW_ERROR(message) fatalError(message, __FILE__, __LINE__)
	inline void fatalError(std::string &&errorString, const std::string& fileName, int line) {
		errorString.append("\nFile: " + fileName + "\nLine: " + std::to_string(line));
		SDL_Quit();
		throw std::exception(errorString.c_str());
	}
}


