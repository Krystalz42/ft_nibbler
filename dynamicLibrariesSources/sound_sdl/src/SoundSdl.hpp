#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <exception>
#include "ISound.hpp"
#include <vector>

class SoundSdl : public ISound {
public:

	class SdlSoundException : public std::exception {
	public:
		SdlSoundException(void) noexcept;

		SdlSoundException(std::string const &) noexcept;

		const char *what() const noexcept override;

		~SdlSoundException(void) noexcept = default;

		SdlSoundException(SdlSoundException const &src) noexcept;

	private:
		SdlSoundException &operator=(SdlSoundException const &rhs) noexcept;

		std::string error_;
	};

	SoundSdl(void);

	~SoundSdl(void) override;

	void setMusic(char const *path) override;

	void playMusic(void) override;

	void addNoise(std::string const &path) override;

	void playNoise(unsigned int index) override;

private:

	void clean_(void);

	Mix_Music *music_;
	std::vector<Mix_Chunk *> sound_;

	SoundSdl &operator=(SoundSdl const &rhs) = delete;

	SoundSdl(SoundSdl const &src) = delete;
};

extern "C" {
ISound *newSound(void);
void deleteSound(ISound *sound);
}
