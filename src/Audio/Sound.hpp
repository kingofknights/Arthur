#pragma once

#include <string>

#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"
class Sound {
public:
	// Constructor
	Sound(const std::string& filepath_);
	// Destructor
	~Sound();
	// PlaySound
	void Play();
	// Stop the sound
	void Stop() const;
	// Specific to SDL_Audio API
	void SetupDevice();

	void Run();

private:
	SDL_AudioDeviceID _device;
	SDL_AudioSpec	  _wavSpecs;
	Uint8*			  _waveBuffer;
	Uint32			  _waveLength;
};
