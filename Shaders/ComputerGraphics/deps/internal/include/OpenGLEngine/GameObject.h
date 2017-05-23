#pragma once 
#include "Transform.h"
#include "GLMesh.h"
#include "GLMaterial.h"
#include <string>

namespace OpenGLEngine {

	class GameObject {
	public:
		Transform transform;
		GlobalMesh *meshRef;
		GlobalMaterial *materialRef;
	};

	class DebugObject {
	public:
		Transform transform;
		glm::vec3 color{ 0,0,0 };
		GlobalMesh *meshRef;
	};

}