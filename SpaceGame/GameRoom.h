/*GameRoom.h*/

/*Author: Jordan Davidson
 * Space Game
 */
 ////////////////////////////////////////////////////////////////////////////////
 // GAME ROOM CLASS
 ////////////////////////////////////////////////////////////////////////////////
/* This class is the basic class for rooms in the game. */
#ifdef _WIN32
#include "windows.h"
#endif
#include "GameObject.h"
#include "GameLight.h"
#include "GameCamera.h"
#include <vector>

#define OBJECT_NOT_FOUND -1

using namespace std; 
class GameRoom{
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
	void AddObject(GameObject *newObject);
	
	//Get the Object with the given name in the current Room if one exists
	GameObject *GetObject(char *objectName);
	
	//Remove the Object with the given name in the current Room if it exists
	void RemoveObject(char *objectName);
	
	//Prints the names of very Object in the Room in sorted order
	void PrintObjectNames();
	
	//setter / Getter for camera
	GameCamera *GetCamera(){return camera;}
	void SetCamera(GameCamera *c){camera = c;}	
	
	//Setter / Getter for lights
	void AddLight(GameLight *l){lights.push_back(l);}
	  
	/* 	Loads a room object from a file in the room format 
		described in documentation. */ 
	static bool LoadRoom(char *fname, GameRoom &room);

	/* 	Writes a file in the specified room format 
		see documentation for further details*/ 
	static bool WriteRoom(const char *fname, GameRoom &room);
	static bool GameRoom::WriteRoom(ostream &os, GameRoom &room);
private:
	char name[MAX_NAME_CHARS];
	vector<GameObject *> objects;
	vector<GameLight *> lights; 
	GameCamera *camera; //null means 1st P 	
		
	// Comparator for Objects
	static bool ObjectComparator(GameObject *o1, GameObject *o2);
	
	// Returns the index of the Object specified by the given name in the given vector
	// Adapted from code found on Wikipedia (en.wikipedia.org/wiki/Binary_search_algorithm);
	int binaryObjectSearch(vector<GameObject *> bjects, char *objectName);
	
	// Returns the midpoint of the given indices
	int GameRoom::midpoint(int imin, int imax) {return (imin + imax) / 2;}
};

