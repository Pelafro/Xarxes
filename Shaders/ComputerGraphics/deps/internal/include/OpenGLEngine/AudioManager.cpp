#include "AudioManager.h"
#include "ErrorManager.h"

namespace OpenGLEngine {

#define MAX_CHANNELS 16

	void SoundEffect::Play(int loops) const {
		if (Mix_PlayChannel(-1, m_chunk, loops) == -1) {
			if (AudioManager::curChannel == MAX_CHANNELS) AudioManager::curChannel = 0;
			if (Mix_PlayChannel(AudioManager::curChannel, m_chunk, loops) == -1) SP_THROW_ERROR("Mix_PlayChannel error: " + std::string(Mix_GetError()));
			++AudioManager::curChannel;
		}
	}

	void Music::Play(int loops) const {
		if (Mix_PlayMusic(m_music, loops) == -1) SP_THROW_ERROR("Mix_PlayMusic error: " + std::string(Mix_GetError()));
	}

	int AudioManager::curChannel{ 0 };

	AudioManager::~AudioManager() {
		Destroy();
	}

	void AudioManager::Init() {
		if (m_isInitialized) SP_THROW_ERROR("Audio Manager error: Trying to initialize Audio Manager twice!");
		//Can be bitwise combination of: MIX_INIT_FAC, MIX_INIT_MOD, MIX_INIT_MP3, MIX_INIT_OGG
		if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1) SP_THROW_ERROR("Mix_Init error: " + std::string(Mix_GetError()));
		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) SP_THROW_ERROR("Mix_OpenAudio error: " + std::string(Mix_GetError()));
		m_isInitialized = true;
		Mix_AllocateChannels(MAX_CHANNELS);
	}

	void AudioManager::Destroy() {
		if (m_isInitialized) {
			m_isInitialized = false;
			Mix_AllocateChannels(0);
			for (auto &e : m_effectMap) Mix_FreeChunk(e.second);
			for (auto &m : m_musicMap) Mix_FreeMusic(m.second);
			m_effectMap.clear();
			m_musicMap.clear();
			Mix_CloseAudio();
			Mix_Quit();
		}
	}

	void AudioManager::SetEffectsVolume(int v) {
		for (auto e = m_effectMap.begin(), end = m_effectMap.end(); e != end; ++e) Mix_VolumeChunk(e->second, v);
	}

	void AudioManager::SetMusicVolume(int v) const {
		Mix_VolumeMusic(v);
	}

	SoundEffect AudioManager::LoadSoundEffect(const std::string& filePath) {
		auto it = m_effectMap.find(filePath); //try find audio in cache
		SoundEffect effect;
		if (it == m_effectMap.end()) {
			auto chunk = Mix_LoadWAV(filePath.c_str());
			if (chunk == nullptr) SP_THROW_ERROR("Mix_LoadWAV error: " + std::string(Mix_GetError()));
			m_effectMap[filePath] = chunk;
			effect.m_chunk = chunk;
			Mix_VolumeChunk(effect.m_chunk, m_effectsVolume);
		} else effect.m_chunk = it->second;
		return effect;
	}

	Music AudioManager::LoadMusic(const std::string & filePath) {
		auto it = m_musicMap.find(filePath); //try find audio in cache
		Music mus;
		if (it == m_musicMap.end()) {
			auto m = Mix_LoadMUS(filePath.c_str());
			if (m == nullptr) SP_THROW_ERROR("Mix_LoadMusic error: " + std::string(Mix_GetError()));
			m_musicMap[filePath] = m;
			mus.m_music = m;
			Mix_VolumeMusic(m_musicVolume);
		}
		else mus.m_music = it->second;
		return mus;
	}

#undef MAX_CHANNELS

}