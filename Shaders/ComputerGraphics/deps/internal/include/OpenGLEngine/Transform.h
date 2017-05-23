#pragma once
#include <glm/vec3.hpp>

struct Transform {
	glm::vec3 position	{ 0,0,0 };
	glm::vec3 rotation	{ 0,0,0 };
	glm::vec3 scale		{ 1,1,1 };
};