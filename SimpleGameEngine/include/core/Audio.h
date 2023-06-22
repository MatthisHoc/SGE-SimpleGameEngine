#pragma once

#include <SDL_mixer.h>

#include <string>
#include <stdexcept>
#include <vector>
#include <map>

#define FREQUENCY MIX_DEFAULT_FREQUENCY
#define FORMAT MIX_DEFAULT_FORMAT
#define CHANNELS 2
#define SAMPLES 2048
#define START_NUM_CHANNELS 32

namespace sg
{
	class AudioException : public std::exception
	{
	public:
		AudioException(const char* message);
		virtual const char* what() const noexcept override;
	private:
		std::string m_message;
	};

	class Audio
	{
	public:
		static void playMusic(const std::string& path, int volume = SDL_MIX_MAXVOLUME);
		// Load the sound file and play it, use this if you only play the sound occasionally, otherwise use playSoundFast()
		static void playSound(const std::string& path, int volume = SDL_MIX_MAXVOLUME);
		// This function will add the sound in cache to avoid loading it on next call. Use this function if you intend to play the sound often
		static void playSoundFast(const std::string& path, int volume = SDL_MIX_MAXVOLUME);

	private:
		static void channelFinished(int channel);

		static std::vector<Mix_Chunk*> playingChunks;
		static std::map<std::string, Mix_Chunk*> savedSounds;
		static Mix_Music* playingMusic;

	private:
		friend class Window;
		static void initialize();
		static void quit();
		static void playSoundIntern(Mix_Chunk* sound, int volume, bool freeAfter = false);
	};
}