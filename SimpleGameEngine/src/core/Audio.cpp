#include "core/Audio.h"

#include "assistants/Resources.h"

namespace sg
{
	AudioException::AudioException(const char* message)
	{
		m_message += "[Audio Exception] ";
		m_message += message;
		m_message += ": ";
		m_message += SDL_GetError();
	}

	const char* AudioException::what() const
	{
		return m_message.c_str();
	}

	std::vector<Mix_Chunk*> Audio::playingChunks(START_NUM_CHANNELS);
	std::map<std::string, Mix_Chunk*> Audio::savedSounds;
	Mix_Music* Audio::playingMusic = nullptr;

	void Audio::initialize()
	{
		if (Mix_OpenAudio(FREQUENCY, FORMAT, CHANNELS, SAMPLES) != 0)
		{
			throw AudioException("Failed to open audio");
		}

		Mix_AllocateChannels(START_NUM_CHANNELS);

		// Initialize vector destined to hold playing sounds
		for (int i = 0; i < START_NUM_CHANNELS; ++i)
		{
			playingChunks.push_back(nullptr);
		}

		Mix_ChannelFinished(channelFinished);
	}

	void Audio::quit()
	{
		// Free every saved sounds
		for (auto& kvp : savedSounds)
		{
			Mix_FreeChunk(kvp.second);
		}

		Mix_FreeMusic(playingMusic);

		Mix_CloseAudio();
	}

	void Audio::channelFinished(int channel)
	{
		Mix_FreeChunk(playingChunks[channel]);
		playingChunks[channel] = nullptr;
	}

	void Audio::playMusic(const std::string& path, int volume)
	{
		playingMusic = Mix_LoadMUS(sg::Resources::pathTo(path).c_str());
		if (!playingMusic)
		{
			throw AudioException("Failed to load music");
		}

		Mix_VolumeMusic(volume);
		if (Mix_PlayMusic(playingMusic, -1) == -1)
		{
			throw AudioException("Failed to play music");
		}
	}

	void Audio::playSoundIntern(Mix_Chunk* sound, int volume, bool freeAfter)
	{
		Mix_VolumeChunk(sound, volume);
		int channel = Mix_PlayChannel(-1, sound, 0);
		if (channel == -1)
		{
			throw AudioException("Failed to play sound sample");
		}
		else if (freeAfter)
		{
			playingChunks[channel] = sound;
		}
	}

	void Audio::playSoundFast(const std::string& path, int volume)
	{
		auto found = savedSounds.find(path);

		// Play sound normally
		if (found != savedSounds.end())
		{
			playSoundIntern((*found).second, volume);
		}
		// Add sound to map and play it
		else
		{
			std::string pathToResource = sg::Resources::pathTo(path);
			Mix_Chunk* sample = Mix_LoadWAV(pathToResource.c_str());
			if (sample == nullptr)
			{
				throw AudioException("Failed to load .wav file");
			}

			savedSounds[pathToResource] = sample;

			playSoundIntern(sample, volume);
		}
	}

	void Audio::playSound(const std::string& path, int volume)
	{
		Mix_Chunk* sample = Mix_LoadWAV(sg::Resources::pathTo(path).c_str());
		if (sample == nullptr)
		{
			throw AudioException("Failed to load .wav file");
		}
		else
		{
			playSoundIntern(sample, volume, true);
		}
	}
}