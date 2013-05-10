/*
 * A simple Debugger class, this allows us to begin debugging things at run time.
 * For now, we just print messages, but later, it might be nice to be able to actively 
 * modify variables to ensure they function properly. 
 */
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

#define GAME_DEBUG_OUTPUT_FILE ("GameDebugOutput.txt")
#define GAME_DEBUG_BAD_FILE (-2)
#define GAME_DEBUG_INVALID_ERROR (-1)
#define GAME_DEBUG_SUCCESS (0)


static ofstream debugFile;
static time_t debugTimer;

//TO DO: CREATE LOCKS AND LOCK PER WRITE/PRINT OUT...



/////////////////////////////////////////////////////////
// INITIALIZE DEBUGGER: CALL THIS ONCE PER GAME
/////////////////////////////////////////////////////////
/* returns: success of start up. */
int GameDebugInitialize(){
	debugFile.open(GAME_DEBUG_OUTPUT_FILE);
	//TO DO: INITIALIZE THREAD LOCK
	if(debugFile.is_open()) return GAME_DEBUG_SUCCESS;
	else return GAME_DEBUG_BAD_FILE;
}


/////////////////////////////////////////////////////////
// FINISH DEBUGGER: CALL THIS ONCE PER GAME
/////////////////////////////////////////////////////////
/* returns: success of finishing. */
int GameDebugFinish(){
	debugFile.close();
	if(debugFile.is_open()) return GAME_DEBUG_INVALID_ERROR;
	else return GAME_DEBUG_SUCCESS;
}


/////////////////////////////////////////////////////////
// Prints message to specified debug file
/////////////////////////////////////////////////////////
/* input: Message to write */
void DebugWriteMessageToFile(char* message, int line = -1){
	//TO do: Lock the debugger...
	debugFile<<"------------------------------------"<<endl;
	debugFile<<"@ Line: "<<line<<" "<<message<<endl;
	time(&debugTimer);
	debugFile<<"Time: "<<ctime(&debugTimer)<<endl;
}

/////////////////////////////////////////////////////////
// Prints message to the console
/////////////////////////////////////////////////////////
/* input: Message to write */
void DebugWriteMessageToConsole(char* message, int line = -1){
	//TO do: Lock the debugger...
	cout<<"------------------------------------"<<endl;
	cout<<"@ Line: "<<line<<" "<<message<<endl;
	time(&debugTimer);
	cout<<"Time: "<<ctime(&debugTimer)<<endl;
}