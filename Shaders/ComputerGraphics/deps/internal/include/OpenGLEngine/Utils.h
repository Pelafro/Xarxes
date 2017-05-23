#pragma once
#include <GL\glew.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#include <SDL2\SDL.h>
#include <SDL2\SDL_ttf.h>
#include <SDL2\SDL_image.h>
#pragma comment(lib, "SDL2_ttf.lib")
#pragma comment(lib, "SDL2_image.lib")
#include "PathLoader.h"

namespace OpenGLEngine {

#define EnableGLHint() \
		/*Indicates the accuracy of fog calculation.If per - pixel fog calculation is not efficiently supported by the GL implementation, 
		hinting GL_DONT_CARE or GL_FASTEST can result in per - vertex calculation of fog effects. */ \
		glHint(GL_FOG_HINT, GL_NICEST); \
		/*Indicates the accuracy of the derivative calculation for the GL shading language fragment processing built - in functions : dFdx, dFdy, and fwidth.*/ \
		glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST); \
		/*Indicates the quality of filtering when generating mipmap images.*/ \
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST); \
		/*Indicates the sampling quality of antialiased lines.If a larger filter function is applied, hinting GL_NICEST can result in more pixel fragments being generated during rasterization.*/ \
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); \
		/*Indicates the quality of color, texture coordinate, and fog coordinate interpolation.If perspective - corrected parameter interpolation is not efficiently supported by the GL implementation,
		hinting GL_DONT_CARE or GL_FASTEST can result in simple linear interpolation of colors and / or texture coordinates.*/ \
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); \
		/*Indicates the sampling quality of antialiased points.If a larger filter function is applied, hinting GL_NICEST can result in more pixel fragments being generated during rasterization.*/ \
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); \
		/*Indicates the sampling quality of antialiased polygons.Hinting GL_NICEST can result in more pixel fragments being generated during rasterization, if a larger filter function is applied.*/ \
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); \
		/*Indicates the quality and performance of the compressing texture images. Hinting GL_FASTEST indicates that texture images should be compressed as quickly as possible, 
		while GL_NICEST indicates that texture images should be compressed with as little image quality loss as possible. 
		GL_NICEST should be selected if the texture is to be retrieved by glGetCompressedTexImage for reuse.*/ \
		glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST); \
	
	// Initialize SDL systems
	void InitSDL(void) {
		SDL_Init(SDL_INIT_EVERYTHING);	// Initialize everything in SDL (VIDEO, AUDIO, EVENTS,...)
		TTF_Init();						// Initialize SDL_ttf for text purposes
		IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
		// Tell we want a double buffered windows to avoid flickering
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	}

	inline void DestroySDL(void) {
		IMG_Quit();
		TTF_Quit();
		SDL_Quit();
	}

	// Initialize SDL attributes
	void InitOpenGL(bool enableGLHint = true) {
		glewExperimental = true;
		if (glewInit() != GLEW_OK) SP_THROW_ERROR("GLEW could not be initialized.");
		glClearDepth(1.0);			// Set the base depth when depth buffer
		glEnable(GL_DEPTH_TEST);	// Activate the Z-buffer
		glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable alpha blending
		glCullFace(GL_BACK); glEnable(GL_CULL_FACE); //glFrontFace(GL_CW);
		glLineWidth(5.0f);
		if (enableGLHint) EnableGLHint();
		SDL_GL_SetSwapInterval(0); // Set V-Sync
		printf("***  OpenGL Version: %s  ***\n", reinterpret_cast<char const*>(glGetString(GL_VERSION)));
	}

	// Create a message box in which to ask the user whether to play on fullscreen or windowed mode
	int AskUserForWindow(void) {
		const SDL_MessageBoxButtonData buttons[] = { { 0, 0, "NO" }, { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "YES" } };
		const SDL_MessageBoxColorScheme colorScheme = { { { 255,   0,   0 }, { 0, 255,   0 }, { 255, 255,   0 }, { 0,   0, 255 }, { 255,   0, 255 } } };
		const SDL_MessageBoxData messageBoxData = { SDL_MESSAGEBOX_INFORMATION, nullptr, "Select an option", "Do you want to play on full screen mode?", SDL_arraysize(buttons), buttons, &colorScheme };
		int buttonID;
		SDL_ShowMessageBox(&messageBoxData, &buttonID); // Whether to play on fullscreen mode or default normal mode
		return buttonID;
	}

}
