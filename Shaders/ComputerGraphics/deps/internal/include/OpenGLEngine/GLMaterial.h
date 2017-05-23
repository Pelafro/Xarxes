#pragma once
#include <glm/vec3.hpp>
#include "GLTexture.h"

namespace OpenGLEngine {

	struct GLMaterial {
		GLTexture diffuse;
		GLTexture normal;
		glm::vec3 specular{ 0,0,0 };
		glm::vec3 emissive{ 0,0,0 };
		float shininess{ 1 };
	};

	struct GlobalMaterial {
		GLMaterial *materialData{ nullptr };
		int numMaterials{ 0 };
	};

}
