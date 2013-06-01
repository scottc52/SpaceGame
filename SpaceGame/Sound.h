/* Simple sound class: load up a sound from a file, then 
 * tell it to play.
 */

#ifndef _SOUND_H
#define _SOUND_H

class Sound {
public:
	Sound(char* filename);
	~Sound();
	static int InitializeSounds();
	static void UninitializeSounds();
private:
	//Mix_Chunk *chunk;
};

#endif