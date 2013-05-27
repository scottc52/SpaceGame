/**
GameTime.h
requires lib rt 
*/

#ifndef GAME_TIME_H
#define GAME_TIME_H

namespace GameTime{ 

#ifdef _WIN32
#include "windows.h"
typedef LARGE_INTEGER GameTimer;
GameTimer GetTime(){
	GameTimer val;
	QueryPerformanceCounter(&val);
	return val;
}

double DiffTimeMS(GameTimer &a, GameTimer &b){
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq); 
	return ((double)(a.QuadPart-b.QuadPart)) / ((double)freq.QuadPart)*((double)1000.0);
	 
}

#endif

#ifdef __linux__

#include <sys/time.h>

typedef timespec GameTimer;

GameTimer GetTime(){
	GameTimer val;
	clock_gettime(CLOCK_MONOTONIC, &val);
	return val;  
}

double DiffTimeMS(GameTimer &a, GameTimer &b){
	return ( ((double) a.tv_sec- b.tv_sec) *1000.0) 
		+((double)(a.tv_nsec - b.tv_nsec))/((double)1000.0)/((double)1000.0) ;
}
#endif

double DiffTimeMS(GameTimer &ref){
	GameTimer cur = GetTime(); 
	return (DiffTimeMS(cur, ref));
}

}
#endif
