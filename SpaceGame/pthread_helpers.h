
#ifndef _PTHREAD_HELPER_H
#define _PTHREAD_HELPER_H

#ifdef __linux__
#include "unistd.h"
inline void pthread_helper_sleep(int ms){
	usleep(ms*1000);
}
#define SLEEP
#endif
#ifdef _WIN32
#include <windows.h>
inline void pthread_helper_sleep(int ms){
	Sleep(ms);
}
 
#define SLEEP
#endif

#ifndef SLEEP
inline void pthread_helper_sleep(int ms){
	return; 
}
#endif
#endif 