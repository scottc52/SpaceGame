/* Simple sound class: load up a sound from a file, then 
 * tell it to play.
 */

#ifndef _SOUND_H
#define _SOUND_H

class Sound {
public:
	Sound(char* filename);
	~Sound();

	void Play();

	static int InitializeSounds();
	static void UninitializeSounds();
private:
	int channel;
	Mix_Chunk *chunk;
};

#endif