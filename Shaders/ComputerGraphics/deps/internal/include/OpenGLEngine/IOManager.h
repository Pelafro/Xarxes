#pragma once
#include <vector>
#include <fstream>

namespace OpenGLEngine {
	
	bool LoadFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer) {
		std::ifstream file(filePath, std::ios::binary);
		if (file.fail()) {
			perror(filePath.c_str());
			return false;
		}
		file.seekg(0, std::ios::end);
		auto fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);
		fileSize -= static_cast<int>(file.tellg());//reduce file size by possible header bytes
		buffer.resize(fileSize);
		file.read(reinterpret_cast<char*>(&(buffer[0])), fileSize);
		file.close();
		return true;
	};

	bool LoadFileToBuffer(std::string filePath, std::string &buffer) {
		std::ifstream file(filePath, std::ios::binary);
		if (file.fail()) {
			perror(filePath.c_str());
			return false;
		}
		file.seekg(0, std::ios::end);
		auto fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);
		fileSize -= static_cast<int>(file.tellg());//reduce file size by possible header bytes
		buffer.resize(fileSize);
		file.read(reinterpret_cast<char*>(&(buffer[0])), fileSize);
		file.close();
		return true;
	};
}

