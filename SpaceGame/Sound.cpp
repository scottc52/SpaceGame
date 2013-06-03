#include "Sound.h"
#include <string>

#define SOUND_STEREO 2
#define SOUND_BUFFER_SIZE 1024

Sound::Sound(char* filename) {
	chunk = Mix_LoadWAV(filename);
	channel = -1;
	if (!chunk) {
		printf("Sound error: could not load filename %s\n", filename);
	}
}

Sound::~Sound() {
	Mix_HaltChannel(channel);
	Mix_FreeChunk(chunk);
	//printf("Destructor being called for some reason...\n");
}

void Sound::Play() {
	channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel == -1) {
		printf("Couldn't play sound; no channel available or something\n");
	} else {
		//printf("It should totally be playing the sound right now\n");
	}
}

void Sound::Stop() {
	Mix_HaltChannel(channel);
}

int Sound::InitializeSounds() {
	int errorCode = 0;
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1) {
		// SDL failed
		printf("SDL couldn't initialize\n");
		errorCode = -1;
	}
	else if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 
			SOUND_STEREO, SOUND_BUFFER_SIZE) == -1){
		// SDL Mixer failed
		printf("SDL Mixer couldn't  initialize\n");
		errorCode = -1;
	}
	return errorCode;
}

void Sound::UninitializeSounds() {
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}