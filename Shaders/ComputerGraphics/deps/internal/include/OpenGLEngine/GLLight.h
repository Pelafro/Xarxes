#pragma once
#include <GL/glew.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace OpenGLEngine {

	#define COLOR_BLACK glm::vec3(.0f)
	#define COLOR_WHITE glm::vec3(1.0f, 1.0f, 1.0f)
	#define COLOR_RED	glm::vec3(1.0f, 0.0f, 0.0f)
	#define COLOR_GREEN glm::vec3(0.0f, 1.0f, 0.0f)
	#define COLOR_CYAN	glm::vec3(0.0f, 1.0f, 1.0f)
	#define COLOR_BLUE	glm::vec3(0.0f, 0.0f, 1.0f)
	
	struct BaseLight {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct DirLight : BaseLight {
		glm::vec3 direction;
	};

	struct PointLight : BaseLight {
		glm::vec3 position;
		float constant;
		float linear;
		float quadratic;
	};

	struct SpotLight : BaseLight {
		glm::vec3 position;
		glm::vec3 direction;
		float cutOff;
		float outerCutOff;

		float constant;
		float linear;
		float quadratic;
	};

	///TODO: simplify!!
	struct DebugLight {
		GLuint vao;
		int elements;
		DebugLight() {
			using namespace glm;
			using namespace std;

			const float pi = 3.1415926535897932384626433832795f;
			const float _2pi = 2.0f * pi;

			float radius = 0.5f;
			int slices = 32;
			int stacks = 32;
			elements = (slices * stacks + slices) * 6;

			vector<vec3> positions;
			vector<vec3> normals;
			vector<vec2> textureCoords;

			for (int i = 0; i <= stacks; ++i) {
				// V texture coordinate.
				float V = i / float(stacks);
				float phi = V * pi;

				for (int j = 0; j <= slices; ++j) {
					// U texture coordinate.
					float U = j / float(slices);
					float theta = U * _2pi;

					float X = cos(theta) * sin(phi);
					float Y = cos(phi);
					float Z = sin(theta) * sin(phi);

					positions.push_back(vec3(X, Y, Z) * vec3(radius, radius, radius));
					normals.push_back(vec3(X, Y, Z));
					textureCoords.push_back(vec2(U, V));
				}
			}
			// Now generate the index buffer
			vector<GLuint> indicies;

			for (int i = 0; i < slices * stacks + slices; ++i) {
				indicies.push_back(i);
				indicies.push_back(i + slices + 1);
				indicies.push_back(i + slices);

				indicies.push_back(i + slices + 1);
				indicies.push_back(i);
				indicies.push_back(i + 1);
			}

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			GLuint vbos[4];
			glGenBuffers(4, vbos);

			glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
			glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(vec2), textureCoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[3]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_STATIC_DRAW);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	};

};