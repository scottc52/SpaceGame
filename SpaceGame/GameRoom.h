/*GameRoom.h*/

/*Author: Jordan Davidson
* Space Game
*/

#ifndef _GAMEROOM_H_
#define _GAMEROOM_H_
////////////////////////////////////////////////////////////////////////////////
// GAME ROOM CLASS
////////////////////////////////////////////////////////////////////////////////
/* This class is the basic class for rooms in the game. */
#ifdef _WIN32
#include "windows.h"
#endif
#include "GameWorldObject.h"
#include "GameActiveObject.h"
#include "GameLight.h"
#include "GameCamera.h"
#include "GameItem.h"
#include "GameDebug.h"
#include "GameDoor.h"
#include <vector>
#include <map>

#define OBJECT_NOT_FOUND -1

using namespace std; 
class GameRoom{
	private:
	char name[MAX_NAME_CHARS];

	map<string, GameWorldObject> wobjects;
	map<string, GameLight> lights;
	map<string, GameCamera> cameras;
	map<string, GameDoor> doors;
	map<string, GameItem> items;
	map<string, GameActiveObject> aobjects;
	//hash_map<string, GameChar*> characters;
	GameCamera* currentCamera; //null means 1st P 
	//GamePlayer* player;

public:
	//default constructor
	GameRoom::GameRoom()
	{
		strcpy(name, "");
		/*TO DO: Initialize lock*/
	}

	//constructor with name
	GameRoom::GameRoom(const char *name)
	{
		strncpy(this->name, name, MAX_NAME_CHARS);
		/*TO DO: Initialize lock*/
	}

	//destructor
	~GameRoom();

	//Get the current Room's name
	char *GameRoom::GetName() {return name;}

	//Redefine the Room's name
	void GameRoom::SetName(const char *newName) {strncpy(name, newName, MAX_NAME_CHARS);}

	//Adds the given Object to the Room
	void GameRoom::AddWorldObject(GameWorldObject newObject);

	//Get the Object with the given name in the current Room if one exists
	GameWorldObject* GameRoom::GetWorldObject(const char *wobjectName);

	//Get the light with the given name in the current Room if one exists
	GameLight* GameRoom::GetLight(const char* lightName);

	//Remove the Object with the given name in the current Room if it exists
	void GameRoom::RemoveWorldObject(const char *wobjectName);

	void GameRoom::RemoveActiveObject(const char* aobjectName){
		map<string, GameActiveObject>::iterator it = aobjects.find(aobjectName);
		if(it != aobjects.end()) aobjects.erase(it);
	}

	GameActiveObject* GameRoom::GetActiveObject(const char* aobjName){
		if(aobjects.find(aobjName) == aobjects.end()) return NULL;
		else return &(aobjects[aobjName]);
	}

	GameItem* GameRoom::GetItem(const char* iName){
		if(items.find(iName) != items.end()){
			return &(items[iName]);
		}
		return NULL;
	}

	void GameRoom::RemoveItem(const char* iName){
		map<string, GameItem>::iterator it = items.find(iName);
		if(it != items.end()){
			items.erase(it);
		}
	}

	//Prints the names of very Object in the Room in sorted order
	/*void PrintObjectNames();*/
	
	//Setter / Getter for doors
	void GameRoom::AddDoor(GameDoor door){doors[door.GetName()] = door;}
	unsigned int NumDoors(){return doors.size();}
	GameDoor *GetDoor(char* doorName){
		if(doors.find(doorName) != doors.end()) return &(doors[doorName]);
		return NULL;
	}
	void GameRoom::RemoveDoor(const char* dName){
		map<string, GameDoor>::iterator it = doors.find(dName);
		if(it != doors.end()) doors.erase(it);
	}

	GameCamera* GameRoom::GetCamera(const char* cName){
		map<string, GameCamera>::iterator it = cameras.find(cName);
		if(it!= cameras.end()) return &(cameras[cName]);
		return NULL;
	}

	void GameRoom::RemoveCamera(const char* cName){
		map<string, GameCamera>::iterator it = cameras.find(cName);
		if(it != cameras.end()) cameras.erase(it);
	}

	void GameRoom::PrintWorldObjectNames();

	//setter / Getter for camera
	GameCamera* GameRoom::GetCurrentCamera(){return currentCamera;}
	void SetCurrentCamera(char *cName){
		currentCamera = &(cameras[cName]);
	}	

	//Setter / Getter for lights
	void GameRoom::AddLight(GameLight l);

	/* 	Loads a room object from a file in the room format 
	described in documentation. */ 
	static bool LoadRoom(const char *fname, GameRoom &room);

	/* 	Writes a file in the specified room format 
	see documentation for further details*/ 
	static bool WriteRoom(const char *fname, GameRoom &room);
	static bool GameRoom::WriteRoom(ostream &os, GameRoom &room);

	map<string, GameWorldObject>::iterator GameRoom::GetRoomWorldObjectsIterator(){
		return wobjects.begin();
	}

	map<string, GameWorldObject>::iterator GameRoom::GetRoomWorldObjectsEnd(){
		return wobjects.end();
	}

	map<string, GameLight>::iterator GameRoom::GetRoomLightsIterator(){
		return lights.begin();
	}

	map<string, GameLight>::iterator GameRoom::GetRoomLightsEnd(){
		return lights.end();
	}

	map<string, GameCamera>::iterator GameRoom::GetRoomCamerasIterator(){
		return cameras.begin();
	}

	map<string,GameCamera>::iterator GameRoom::GetRoomCamerasEnd(){
		return cameras.end();
	}

};

#endif _GAMEROOM_H_