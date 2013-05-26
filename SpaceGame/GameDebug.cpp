/*
 *
 * Singleton source code for debug
 *
 */
#include "GameObjectHeaderList.h"

ofstream outFile;
GameDebugger* GameDebugger::m_pinstance = NULL;
time_t timer;

GameDebugger* GameDebugger::GetInstance(){
	//Lock here.
	if(!m_pinstance){
		m_pinstance = new GameDebugger;
	}
	return m_pinstance;
}

bool GameDebugger::OpenDebugFile(){
	//Lock here
	if(outFile.is_open())return true;
	outFile.open(GAME_DEBUG_OUTPUT_FILE);
	return(outFile.is_open());
}

bool GameDebugger::CloseDebugFile(){
	//Lock here
	if(!outFile.is_open())return true;
	outFile.close();
	return(!outFile.is_open());
}

void GameDebugger::WriteDebugMessageToConsole(const char* message, int lineNo){
	//Lock here
	cout<<"-----------------------------------------------------"<<endl;
	cout<<message<<" @ line: "<<lineNo<<endl;
	time(&timer);
	cout<<"*Found at: "<<ctime(&timer)<<endl;
}

void GameDebugger::WriteToDebugFile(const char* message, int lineNo){
	if(!outFile.is_open()) return;
	//Lock here;
	outFile<<"-----------------------------------------------------"<<endl;
	outFile<<message<<" @ line: "<<lineNo<<endl;
	time(&timer);
	outFile<<"*Found at: "<<ctime(&timer)<<endl;
}