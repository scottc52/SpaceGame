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
#include "GamePlayer.h"
#include <vector>
#include <pthread.h>
#include <map>
#include "Monitor.h"

#define OBJECT_NOT_FOUND -1

using namespace std; 

/*Vector3f ConvertToEigen3Vector(Vec3f& v);
Vec3f ConvertToOM3Vector(Vector3f& v);
Vector4f ConvertToEigen4Vector(Vec4f& v);
Vec4f ConvertToOM4Vector(Vector4f& v);*/
#define ConvertToEigen3Vector(v) (Vector3f((v)[0], (v)[1], (v)[2]))
#define ConvertToOM3Vector(v) (Vec3f((v).x(), (v).y(), (v).z()))
#define ConvertToEigen4Vector(v) (Vector4f((v)[0], (v)[1], (v)[2], (v)[3]))
#define ConvertToOM4Vector(v) (Vec4f((v).w(), (v).x(), (v).y(), (v).z()))

class GameRoom{
private:
	char name[MAX_NAME_CHARS];

	map<string, GameWorldObject> wobjects;
	map<string, GameLight> lights;
	map<string, GameCamera> cameras;
	map<string, GameDoor> doors;
	map<string, GameItem> items;
	map<string, GameActiveObject> aobjects;
	map<string, GamePlayer> players;
	//hash_map<string, GameChar*> characters;
	GameCamera* currentCamera; //null means 1st P 
	//GamePlayer* player;

public:
	Monitor monitor;
	map<GameObject*, vector<GameObject*> > collisionTier0List;
	map<GameObject*, vector<GameObject*> > collisionTier1List;
	map<GameObject*, vector<GameObject*> > collisionTier2List;

	void ClearCollisions(){
		collisionTier0List.clear();
		collisionTier1List.clear();
		collisionTier2List.clear();
	}
	//default constructor
	GameRoom::GameRoom()
	{
		/*map<GameObject*, vector<GameObject*>> collisionTier0List;
		map<GameObject*, vector<GameObject*>> collisionTier1List;
		map<GameObject*, vector<GameObject*>> collisionTier2List;*/
		strcpy(name, "");
		/*TO DO: Initialize lock*/
	}

	//constructor with name
	GameRoom::GameRoom(const char *name)
	{
		strncpy(this->name, name, MAX_NAME_CHARS);
		//map<GameObject*, vector<GameObject*>> collisionTier0List;
		//map<GameObject*, vector<GameObject*>> collisionTier1List;
		//map<GameObject*, vector<GameObject*>> collisionTier2List;
		/*TO DO: Initialize lock*/
	}

	//destructor
	~GameRoom();

	//Get the current Room's name
	char *GameRoom::GetName() {return name;}

	//Redefine the Room's name
	void GameRoom::SetName(const char *newName) {strncpy(name, newName, MAX_NAME_CHARS);}

	//Adds the given Object to the Room
	void GameRoom::AddWorldObject(GameWorldObject& newObject);

	void GameRoom::AddItem(GameItem& item);

	//Adds the given Object to the Room
	void GameRoom::AddActiveObject(GameActiveObject& newObject);

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

	//Adds the given Object to the Room
	void GameRoom::AddPlayer(GamePlayer& newPlayer);

	//Get the Object with the given name in the current Room if one exists
	GamePlayer* GameRoom::GetPlayer(const char *pName);

	//Remove the Object with the given name in the current Room if it exists
	void GameRoom::RemovePlayer(const char *pName);


	//Prints the names of very Object in the Room in sorted order
	/*void PrintObjectNames();*/

	//Setter / Getter for doors
	void GameRoom::AddDoor(GameDoor& door){doors[door.GetName()] = door;}
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
	void GameRoom::AddLight(GameLight& l);

	/* 	Loads a room object from a file in the room format 
	described in documentation. */ 
	static bool LoadRoom(const char *fname, GameRoom &room);

	/* 	Writes a file in the specified room format 
	see documentation for further details*/ 
	static bool WriteRoom(const char *fname, GameRoom &room);
	static bool GameRoom::WriteRoom(ostream &os, GameRoom &room);

	vector<GameWorldObject*> GameRoom::GetWorldObjects(){
		vector<GameWorldObject*> v;
		map<string, GameWorldObject>::iterator it = wobjects.begin();
		while(it != wobjects.end()){
			v.push_back(&(it->second));
			it++;
		}
		return v;
	}

	vector<GameActiveObject*> GameRoom::GetActiveObjects(){
		vector<GameActiveObject*> v;
		map<string, GameActiveObject>::iterator it = aobjects.begin();
		while(it != aobjects.end()){
			v.push_back(&(it->second));
			it++;
		}
		return v;
	}

	vector<GameLight*> GameRoom::GetRoomLights(){
		vector<GameLight*> v;
		map<string, GameLight>::iterator it = lights.begin();
		while(it != lights.end()){
			v.push_back(&(it->second));
			it++;
		}
		return v;
	}

	vector<GameItem*> GameRoom::GetItems(){
		vector<GameItem*> v;
		map<string, GameItem>::iterator it = items.begin();
		while(it != items.end()){
			v.push_back(&(it->second));
			it++;
		}
		return v;
	}

	vector<GameDoor*> GameRoom::GetDoors(){
		vector<GameDoor*> v;
		map<string, GameDoor>::iterator it = doors.begin();
		while(it != doors.end()){
			v.push_back(&(it->second));
			it++;
		}
		return v;
	}

	vector<GameCamera*> GameRoom::GetCameras(){
		vector<GameCamera*> v;
		map<string, GameCamera>::iterator it = cameras.begin();
		while(it != cameras.end()){
			v.push_back(&(it->second));
			it++;
		}
		return v;
	}
	vector<GamePlayer*> GameRoom::GetPlayers(){
		vector<GamePlayer*> v;
		map<string, GamePlayer>::iterator it = players.begin();
		while(it != players.end()){
			v.push_back(&(it->second));
			it++;
		}
		return v;
	}
	vector<GameObject*> GameRoom::GetGameObjects(){
		vector<GameObject*>v;
		vector<GameWorldObject*> wobs = GetWorldObjects();
		vector<GameActiveObject*> aobs = GetActiveObjects();
		vector<GamePlayer*> players = GetPlayers();
		vector<GameCamera*> cameras = GetCameras();
		vector<GameDoor*> doors = GetDoors();
		vector<GameItem*> items = GetItems();

		for(unsigned int i = 0; i<wobs.size();i++)	v.push_back((GameObject*)wobs[i]);
		for(unsigned int i = 0; i<aobs.size();i++)  v.push_back((GameObject*)aobs[i]);
		for(unsigned int i = 0; i<players.size();i++)  v.push_back((GameObject*)players[i]);
		for(unsigned int i = 0; i<cameras.size();i++)  v.push_back((GameObject*)cameras[i]);
		for(unsigned int i = 0; i<doors.size();i++)  v.push_back((GameObject*)doors[i]);
		for(unsigned int i = 0; i<items.size();i++)  v.push_back((GameObject*)items[i]);

		return v;
	}

};

#endif _GAMEROOM_H_