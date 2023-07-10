#include "Sound.hpp"

#include <Lancelot/Logger/Logger.hpp>

Sound::Sound(const std::string& filepath_) {
	if (SDL_LoadWAV(filepath_.c_str(), &_wavSpecs, &_waveBuffer, &_waveLength) == nullptr) {
		LOG(ERROR, "{} {}", "sound loading error: ", SDL_GetError())
	}
	SetupDevice();
}

Sound::~Sound() {
	SDL_free(_waveBuffer);
	SDL_CloseAudioDevice(_device);
}

void Sound::Play() {
	SDL_ClearQueuedAudio(_device);
	SDL_QueueAudio(_device, _waveBuffer, _waveLength);
	SDL_PlayAudioDevice(_device);
}

void Sound::Stop() const {
	SDL_PauseAudioDevice(_device);
}

void Sound::SetupDevice() {
	// Request the most reasonable default device
	// Set the device for playback for 0, or '1' for recording.
	_device = SDL_OpenAudioDevice(nullptr, 0, &_wavSpecs, nullptr, 0);
	// Error message if no suitable device is found to play
	// audio on.
	if (0 == _device) {
		LOG(ERROR, "{} {}", "sound device error: ", SDL_GetError())
	}
}

void Sound::Run() {
	while (SDL_GetQueuedAudioSize(_device) > 0) {
		SDL_Delay(1);
	}
}
