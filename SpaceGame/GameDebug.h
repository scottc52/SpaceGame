/*
* A simple Debugger class, this allows us to begin debugging things at run time.
* For now, we just print messages, but later, it might be nice to be able to actively 
* modify variables to ensure they function properly. 
*/
#ifndef _GAMEDEBUG_H_
#define _GAMEDEBUG_H_
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

#define GAME_DEBUG_OUTPUT_FILE ("GameDebugOutput.txt")
#define GAME_DEBUG_BAD_FILE (-2)
#define GAME_DEBUG_INVALID_ERROR (-1)
#define GAME_DEBUG_SUCCESS (0)

#define GAME_DEBUG_ASSERT(A) \
if (!(A)){ \
std::cerr << "assertion "<< #A << " failed in " << __FILE__ << " at :" << __LINE__ << std::endl;\
}

class GameDebugger{
private: 
	GameDebugger(){}
	static GameDebugger* m_pinstance;
	GameDebugger(GameDebugger const &debugger){};
	void operator=(GameDebugger const &debugger){};
public:
	static GameDebugger* GameDebugger::GetInstance();
	bool GameDebugger::OpenDebugFile();
	bool GameDebugger::CloseDebugFile();
	void WriteToDebugFile(char* message, int lineNo = 0);
	void WriteDebugMessageToConsole(char* message, int lineNo = 0);
};
#endif _GAMEDEBUG_H_
