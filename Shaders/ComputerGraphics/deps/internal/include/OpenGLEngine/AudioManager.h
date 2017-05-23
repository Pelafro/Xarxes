#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>

//http://jcatki.no-ip.org:8080/SDL_mixer/SDL_mixer_frame.html

namespace OpenGLEngine {

	class SoundEffect {
		Mix_Chunk* m_chunk{ nullptr }; // Pointer to sound effect to be loaded
	public:
		explicit SoundEffect() = default;
		~SoundEffect() = default;
		// Run the loaded chunk, if loops = -1 -> loop forever, else play it loops times
		void Play(int loops = -1) const;
		friend class AudioManager;
	};

	class Music {
		Mix_Music* m_music{ nullptr }; // Pointer to soundtrack to be loaded
	public:
		explicit Music() = default;
		~Music() = default;
		// Run the loaded soundtrack, if loops = -1 -> loop forever, else play it loops times
		void Play(int loops = -1) const;
		// Pause current soundtrack
		static void Pause()		{ Mix_PauseMusic(); };
		// Stop current soundtrack
		static void Stop()		{ Mix_HaltMusic(); };
		// Resume current soundtrack
		static void Resume()	{ Mix_ResumeMusic(); };
		// Check if current soundtrack is playing
		static int IsPlaying()	{ return Mix_PlayingMusic(); };
		friend class AudioManager;
	};

	class AudioManager {
		std::map <std::string, Mix_Chunk*> m_effectMap; // Sound effects list with key value as the file path of the sound to be loaded
		std::map <std::string, Mix_Music*> m_musicMap; // Soundtracks list with key value as the file path of the melody to be loaded
		bool m_isInitialized	{ false }; // To check whether if the audio manager instance has been initialized
		int m_effectsVolume		{ static_cast<int>(MIX_MAX_VOLUME * 0.3f) }; // Set default mid-low volume for sound effects
		int m_musicVolume		{ static_cast<int>(MIX_MAX_VOLUME * 0.5f) }; // Set default mid volume for soundtracks
	public:
		static int curChannel; // Current audio index for the allocated channels, 0 by default

		explicit AudioManager() = default;
		~AudioManager();

		// Initialize audio system, enable audio formats and allocate channels
		void Init();
		// Erase all the soundtracks and sound effects loaded
		void Destroy();
		// Change the volume of the current list of sound effects
		void SetEffectsVolume(int v);
		// Get the volume of the current list of sound effects
		int GetEffectsVolume() const { return m_effectsVolume; };
		// Change the volume of the current soundtrack playing
		void SetMusicVolume(int v) const;
		// Get the volume of the current soundtrack playing
		int GetMusicVolume() const { return m_musicVolume; };
		// Load a sound effect from file and return it
		SoundEffect LoadSoundEffect(const std::string &filePath);
		// Load a soundtrack from file and return it
		Music LoadMusic(const std::string &filePath);
	};

}
