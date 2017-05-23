#pragma once
#include <string>
#include <GL\glew.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

namespace OpenGLEngine {

	class ShaderProgram {
		GLuint m_programID			{ 0 };
		GLuint m_vertexShaderID		{ 0 };
		GLuint m_fragmentShaderID	{ 0 };
		int m_numAttributes			{ 0 };

		static void compileShader(const char* source, const std::string &name, GLuint id);
		void linkShaders() const;
	public:
		explicit ShaderProgram();
		~ShaderProgram();

		void LoadShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		GLint getUniformLocation(const std::string &uniformName) const;
		GLint getAttribLocation(const std::string &attribName) const;

		inline void bind() const	{ glUseProgram(m_programID); };
		inline void unbind() const	{ glUseProgram(0); };
	};

}

