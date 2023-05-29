#pragma once

#include <string>

#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"
class Sound {
public:
	// Constructor
	Sound(std::string filepath);
	// Destructor
	~Sound();
	// PlaySound
	void Play();
	// Stop the sound
	void Stop();
	// Specific to SDL_Audio API
	void SetupDevice();

	void Run();

private:
	SDL_AudioDeviceID _device;
	SDL_AudioSpec	  _wavSpecs;
	Uint8*			  _waveBuffer;
	Uint32			  _waveLength;
};
/*
int main() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		SDL_Log("SDL could not initialize! Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_AudioSpec wavSpec;
	Uint32		  wavLength;
	Uint8*		  wavBuffer;

	if (SDL_LoadWAV("songs.wav", &wavSpec, &wavBuffer, &wavLength) == NULL) {
		SDL_Log("Failed to load WAV file! Error: %s\n", SDL_GetError());
		return 1;
	}

	// Open audio device
	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	if (deviceId == 0) {
		SDL_Log("Failed to open audio device! Error: %s\n", SDL_GetError());
		return 1;
	}

	// Play audio
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_ClearQueuedAudio(deviceId);
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_PlayAudioDevice(deviceId);

	// Wait until audio finishes playing
	while (SDL_GetQueuedAudioSize(deviceId) > 0) {
		SDL_Delay(100);
	}

	// Clean up
	SDL_CloseAudioDevice(deviceId);
	SDL_free(wavBuffer);


	Sound sound("ara-ara.wav");
	sound.PlaySound();
	sound.PlaySound();
	sound.PlaySound();
	sound.PlaySound();
	sound.Execute();
	SDL_Quit();

	return 0;
}
*/
