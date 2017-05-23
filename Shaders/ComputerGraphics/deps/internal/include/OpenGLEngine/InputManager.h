#pragma once
#include <glm/glm.hpp>
#include <unordered_map>

namespace OpenGLEngine {

	class InputManager {
		std::unordered_map<unsigned int, bool> m_keyMap;
		std::unordered_map<unsigned int, bool> m_prevKeyMap;
	public:
		glm::vec2 mouseCoords;
		int zoom{ 0 };

		explicit InputManager();
		~InputManager() = default;
		void update();

		void pressKey(unsigned keyID);
		void releaseKey(unsigned keyID);
		bool isKeyDown(unsigned keyID);
		static bool isKeyDown(const std::unordered_map<unsigned int, bool> &keyMap, unsigned keyID);
		bool isKeyPressed(unsigned keyID);
	};

}


