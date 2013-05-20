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
	/*
	vector<GameObject *> objects;
	vector<GameLight *> lights;
	vector<GameDoor *> doors;  
	GameCamera *camera; //null means 1st P 	
		*/

	map<string, GameWorldObject> wobjects;
	map<string, GameLight> lights;
	map<string, GameCamera> cameras;
	map<string, GameDoor> doors;
	map<string, GameItem> items;
	map<string, GameActiveObject> aobjects;
	//hash_map<char*, GamePlayer*, hash<char*>, eqstr> players;
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
	GameRoom::GameRoom(char *name)
	{
		strncpy(this->name, name, MAX_NAME_CHARS);
		/*TO DO: Initialize lock*/
	}

	//destructor
	~GameRoom();

	//Get the current Room's name
	char *GameRoom::GetName() {return name;}

	//Redefine the Room's name
	void GameRoom::SetName(char *newName) {strncpy(name, newName, MAX_NAME_CHARS);}

	//Adds the given Object to the Room
	void AddWorldObject(GameWorldObject newObject);

	//Get the Object with the given name in the current Room if one exists
	GameWorldObject* GetWorldObject(char *wobjectName);

	//Get the light with the given name in the current Room if one exists
	GameLight* GetLight(char* lightName);

	//Remove the Object with the given name in the current Room if it exists
	void RemoveWorldObject(char *wobjectName);

	//Prints the names of very Object in the Room in sorted order
	/*void PrintObjectNames();*/
	
	//Setter / Getter for doors
	void AddDoor(GameDoor door){doors[door.GetName()] = door;}
	unsigned int NumDoors(){return doors.size();}
	GameDoor *GetDoor(char* doorName){
		if(doors.find(doorName) != doors.end()) return &(doors[doorName]);
		return NULL;
	}
	bool RemoveDoor(GameDoor *door){}/*not yet implemented // return false;} */
	void PrintWorldObjectNames();

	//setter / Getter for camera
	GameCamera *GetCurrentCamera(){return currentCamera;}
	void SetCurrentCamera(char *cName){
		currentCamera = &(cameras[cName]);
	}	

	//Setter / Getter for lights
	void AddLight(GameLight l);

	/* 	Loads a room object from a file in the room format 
	described in documentation. */ 
	static bool LoadRoom(const char *fname, GameRoom &room);

	/* 	Writes a file in the specified room format 
	see documentation for further details*/ 
	static bool WriteRoom(const char *fname, GameRoom &room);
	static bool GameRoom::WriteRoom(ostream &os, GameRoom &room);

	map<string, GameWorldObject>::iterator GetRoomWorldObjectsIterator(){
		return wobjects.begin();
	}

	map<string, GameWorldObject>::iterator GetRoomWorldObjectsEnd(){
		return wobjects.end();
	}

	map<string, GameLight>::iterator GetRoomLightsIterator(){
		return lights.begin();
	}

	map<string, GameLight>::iterator GetRoomLightsEnd(){
		return lights.end();
	}

	map<string, GameCamera>::iterator GetRoomCamerasIterator(){
		return cameras.begin();
	}

	map<string,GameCamera>::iterator GetRoomCamerasEnd(){
		return cameras.end();
	}

};

#endif _GAMEROOM_H_