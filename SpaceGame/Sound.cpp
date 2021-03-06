#include "Sound.h"
#include "GameObject.h"
#include <string>

#define SOUND_STEREO 2
#define SOUND_BUFFER_SIZE 1024
#define MAX_VOLUME_INT 128

// **************
// REGULAR SOUNDS
// **************

map<char*, Mix_Chunk*> *Sound::filenamesToChunks = new map<char*, Mix_Chunk*>();

void Sound::init(char* filename, GameObject *object, Vector3f position, float maxVolume) {
	// Try to find chunk in existing map (if filename's been loaded already).
	// If it's not there, load it and put it there.
	// C++ has no map.contains function, so we need to use stupidity instead.
	map<char*, Mix_Chunk*>::iterator it = filenamesToChunks->find(filename);
	if (it != filenamesToChunks->end()) {
		chunk = filenamesToChunks->at(filename);
	} else {
		chunk = Mix_LoadWAV(filename);
		if (!chunk) {
			printf("Sound error: could not load filename %s\n", filename);
		} else {
			(*filenamesToChunks)[filename] = chunk;
		}
	}

	this->maxVolume = maxVolume;
	channel = -1;
	this->object = object;
	this->position = position;
}

Sound::Sound(char* filename, float maxVolume) {
	init(filename, NULL, Vector3f(0.f, 0.f, 0.f), maxVolume);
}

Sound::Sound(char* filename, GameObject *object, float maxVolume) {
	init(filename, object, Vector3f(0.f, 0.f, 0.f), maxVolume);
}

Sound::Sound(char* filename, Vector3f position, float maxVolume) {
	init(filename, NULL, position, maxVolume);
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
		Mix_Volume(channel, this->maxVolume * MAX_VOLUME_INT);
	}
}

void Sound::Loop() {
	channel = Mix_PlayChannel(-1, chunk, -1);
	if (channel == -1) {
		printf("Couldn't play sound; no channel available or something\n");
	} else {
		Mix_Volume(channel, this->maxVolume * MAX_VOLUME_INT);
	}
}

void Sound::Update(Vector3f playerPosition, Vector3f playerLook, Vector3f playerUp) {
	Vector3f playerToSound;
	if (object) {
		playerToSound = object->GetPosition() - playerPosition;
	} else {
		playerToSound = position - playerPosition;
	}
	Mix_Volume(channel, ((float) MAX_VOLUME_INT) * this->maxVolume / playerToSound.norm());
	Vector3f playerLeft = playerUp.cross(playerLook).normalized();
	float dot = playerLeft.dot(playerToSound.normalized());
	Mix_SetPanning(channel, MAX_VOLUME_INT * (1.0f + dot), MAX_VOLUME_INT * (1.0f - dot));
}

void Sound::Update(GameCamera *camera) {
	Update(camera->GetPosition(), camera->GetViewVector(), camera->GetUpVector());
}

void Sound::Pause() {
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

// *****
// MUSIC
// *****

Music::Music(char *filename, float maxVolume) {
	music = Mix_LoadMUS(filename);
	if (!music) {
		printf("Could not open %s as a music file\n", filename);
	}
	Mix_VolumeMusic(maxVolume * MAX_VOLUME_INT);
	channel = -1;
}

Music::~Music() {
	Mix_HaltMusic();
	Mix_FreeMusic(music);
}

void Music::Play() {
	channel = Mix_PlayMusic(music, 0);
	if (channel == -1) {
		printf("Could not allocate channel for music\n");
	}
}

void Music::Loop() {
	channel = Mix_PlayMusic(music, -1);
	if (channel == -1) {
		printf("Could not allocate channel for music\n");
	}
}

void Music::Pause() {
	Mix_HaltMusic();
}

