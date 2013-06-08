/* Simple sound class: load up a sound from a file, then 
 * tell it to play.
 */

#ifndef _SOUND_H
#define _SOUND_H

#ifdef __linux__
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#else
#include "SDL.h"
#include "SDL_mixer.h"
#endif

#include "GameObject.h"

class Sound {
public:
	Sound(char* filename);
	// Sounds are either attached to objects (e.g. roars from monsters) or positions
	// (e.g. the sound of a collision). So there are separate constructors for these 
	// two cases.
	Sound(char* filename, GameObject *object, float maxVolume=1.0f);
	Sound(char* filename, Vector3f position, float maxVolume=1.0f);
	~Sound();

	void Play();
	// Update sound volume and left/right panning to account for new position.
	void Update(Vector3f playerPosition, Vector3f playerLook, Vector3f playerUp);
	void Stop();

	static int InitializeSounds();
	static void UninitializeSounds();
private:
	// Private constructor to minimize code reuse
	Sound(char* filename, GameObject *object, Vector3f position, float maxVolume);

	int channel;
	Mix_Chunk *chunk;
	float maxVolume;
	GameObject *object;
	Vector3f position;
};

#endif
