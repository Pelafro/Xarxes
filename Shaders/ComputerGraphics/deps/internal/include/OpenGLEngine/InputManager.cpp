#include "InputManager.h"

namespace OpenGLEngine {

	InputManager::InputManager() : mouseCoords(0.0f,0.0f) {}

	void InputManager::update() {
		for (auto &it : m_keyMap) m_prevKeyMap[it.first] = it.second;
		zoom = 0;
	}

	void InputManager::pressKey(unsigned keyID) {
		m_keyMap[keyID] = true;
	}

	void InputManager::releaseKey(unsigned keyID) {
		m_keyMap[keyID] = false;
	}

	bool InputManager::isKeyDown(unsigned keyID) {
		auto it = m_keyMap.find(keyID);
		if (it != m_keyMap.end()) return it->second; //key found
		return false;
	}

	bool InputManager::isKeyDown(const std::unordered_map<unsigned int, bool> &keyMap, unsigned keyID) {
		auto it = keyMap.find(keyID);
		if (it != keyMap.end()) return it->second; //key found
		return false;
	}

	bool InputManager::isKeyPressed(unsigned keyID) {
		return (isKeyDown(keyID) && !isKeyDown(m_prevKeyMap, keyID));
	}

}
