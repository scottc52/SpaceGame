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
class Sound {
public:
	Sound(char* filename);
	~Sound();

	void Play();
	void Stop();

	static int InitializeSounds();
	static void UninitializeSounds();
private:
	int channel;
	Mix_Chunk *chunk;
};

#endif
