/*
* GameState.cpp
* Thanks to GameRoom.cpp for the method models.
*/
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>
#include <string>
#include "GameObjectHeaderList.h"
#include "LocationDefines.h"

using namespace std;

#define STATE_FILE_DELIM ('$')

void splitString(string &s, vector<string>& buff){
	buff.clear(); 
	istringstream iss(s);
	string token;
	while (getline(iss, token, ' ')){
		buff.push_back(token);
	}
	return; 
}

/////////////////////////////////////////////////////////////////////////////////////////
// OBJECT UPDATING
/////////////////////////////////////////////////////////////////////////////////////////

bool UpdateObject(ifstream &file, GameRoom& r){
	string line;
	bool delim = false;
	string name;
	getline(file,name);
	if(name[0] == '$') return true;
	GameActiveObject* aobj = r.GetActiveObject(name.c_str());
	if(aobj == NULL){
		ostringstream message;
		message<<"State Machine could not find object of name: ";
		message<<name;
		GameDebugger::GetInstance()->WriteToDebugFile(message.str().c_str());
		return false;
	}
	while(!file.eof()){
		getline(file, line);
		if(line[0] == STATE_FILE_DELIM){
			delim = true;
			break;
		}
		switch(line[0]){
			///////////////Position//////////////
		case 'p':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 4){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad position input");
			}else{
				aobj->SetPosition(Vec3f(atof(parsed[1].c_str()), atof(parsed[2].c_str()), atof(parsed[3].c_str())));
			}
			break;
				 }
				 ///////////////////Rotation////////////////////////////////
		case 'r':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 5){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
				aobj->SetRotation(Vec4f(atof(parsed[1].c_str()), atof(parsed[2].c_str()), atof(parsed[3].c_str()), atof(parsed[3].c_str())));
			}
			break;
				 }
				 ///////////////Forces//////////////////////////////////
		case 'f':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			/* To do: 
			Set Physics Forces.....

			if(parsed.size() != 5){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
				aobj->SetRotation(Vec4f(atof(parsed[1].c_str()), atof(parsed[2].c_str()), atof(parsed[3].c_str()), atof(parsed[3].c_str())));
			}
			*/
			break;
				 }
				 //////////////////////State/////////////////////////////////////
		case 's':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 2){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad state input");
			}else{
				aobj->SetState(parsed[1].c_str());
			}
			break;
				 }
				 ///////////////////////Delete//////////////////////////
		case 'd':{
			r.RemoveActiveObject(aobj->GetName());
			break;
				 }
		default:{
			GameDebugger::GetInstance()->WriteToDebugFile("Invalid State Machine File entry");
			break;
				}
		}
	}
	if(!delim){
		GameDebugger::GetInstance()->WriteToDebugFile("State Machine improper formatting...");
	}
	return delim;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ITEM UPDATE
///////////////////////////////////////////////////////////////////////////////////////////////////


bool UpdateItem(ifstream &file, GameRoom& r){
	string line;
	bool delim = false;
	string name;
	getline(file,name);
	if(name[0] == '$') return true;
	GameItem* item = r.GetItem(name.c_str());
	if(item == NULL){
		ostringstream message;
		message<<"State Machine could not find object of name: ";
		message<<name;
		GameDebugger::GetInstance()->WriteToDebugFile(message.str().c_str());
		return false;
	}
	while(!file.eof()){
		getline(file, line);
		if(line[0] == STATE_FILE_DELIM){
			delim = true;
			break;
		}
		switch(line[0]){
			///////////////////////Item Property////////////////////////////
		case 'p':{
			
			break;
				 }
		case 'a':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 2){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
				item->SetAcquired(atoi(parsed[1].c_str()));
			}
			break;
				 }
		case 'u':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 2){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
				item->SetNumUses(atoi(parsed[1].c_str()));
			}
			break;
				 }
		case 'd':{
			r.RemoveItem(item->GetName());
			break;
				 }
		default:{
			GameDebugger::GetInstance()->WriteToDebugFile("Invalid State Machine File entry");
			break;
				}
		}
	}
	if(!delim){
		GameDebugger::GetInstance()->WriteToDebugFile("State Machine improper formatting...");
	}
	return delim;
}

///////////////////////////////////READ STATE FILE/////////////////////////////////////////////

bool GameState::ReadStateFile(const char *fname){
	GameRoom* r = this->GetRoom();
	ifstream myfile;
	myfile.open(fname);
	if (!myfile.good()){
		myfile.close();
		return false;
	}
	string roomFileLine;
	getline(myfile, roomFileLine);
	string roomFile = GAME_DATA_STATES_FOLDER;
	roomFile += roomFileLine;
	//Load in the room
	if(!GameRoom::LoadRoom(roomFile.c_str(), (*r))){
		string message = "Room file: ";
		message += roomFile.c_str();
		message += " while Loading in the state machine, is not a valid Room File\n";
		GameDebugger::GetInstance()->WriteDebugMessageToConsole(message.c_str(), 150);
		return false;
	}
	string line; 
	while (myfile.good()){
		getline(myfile, line);
		switch(line[0]){
		case'O':{
			if(!UpdateObject(myfile, (*r))){
				GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
				return false;
			}
			break;
				}
		case 'P':{
		//	if(!UpdatePlayer(myfile)){
		//		GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
		//		return false;
		//	}
			break;
				 }
		case 'I': {
			if(!UpdateItem(myfile, (*r))){
				GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
				return false;
			}
			break;
				  }
		case 'C':{
			//if(!UpdateCamera(myfile, room)){
			//	GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
		//		return false;
		//	}
			break;
				 }
		default:
			GameDebugger::GetInstance()->WriteDebugMessageToConsole("parseRoomFile: unexpected line type");
			break; 
		}
	}

	myfile.close(); 
	return true; 
}


/*
bool GameRoom::WriteRoom(const char *fname, GameRoom& room){	
ofstream ofs;
ofs.open(fname);
if (!ofs.is_open()){return false;}
bool stat = WriteRoom(ofs, room);
ofs.close();
return stat;
}
/*
bool GameRoom::WriteRoom(ostream &os, GameRoom &room){		
int olen = room.wobjects.size(); 
os << "# Automatically generated Room Text file" << endl; 
os << "N\t" << room.GetName()  << endl; 	

//TODO: specialize for doors	
os << "# Objects: O name filename x y z angle x y z scale" << endl; 

//ITERATE THROUGH THE MAP>>>
/*for(int i = 0; i < olen; i++){
os << "O\t";
writeGameObject(os, room.wobjects[i]);
os << endl;  
}

int llen = room.lights.size();
os << "# Lights: name x y z ra ga ba rd gd bd rs bs gs" << endl; 
/*for(int i = 0; i < llen; i ++ ){
os << "L\t";
writeGameLight(os, room.lights[i]);
os << endl; 
}

////////////ITERATE THROUGHT THE MAP INSTEAD

if (room.currentCamera == NULL)
os << "C\tplayer" << endl;  	 
else{
os << "C\t";
writeGameCamera(os, room.currentCamera);
os << endl;  
}
return true;	
}
*/
//=======================================
//  Other Member function implementations
//=======================================

//GameState::~GameState()
//{
/*int objectSize = this->wobjects.size();
for (int i = 0; i < objectSize; i++)
{
delete this->wobjects[i];
}
int lightSize = this->lights.size();
for (int i = 0; i < lightSize; i ++){
delete this->lights[i];
}
//To do: make this a list deletion as well...
*/
//}

//void GameState::PrintWorldObjectNames()
//{
/*int objectSize = this->objects.size();
for (int i = 0; i < (objectSize - 1); i++)
{
cout << this->objects[i]->GetName() << ", ";
}
cout << this->objects[objectSize - 1]->GetName() << endl;
*/
//}