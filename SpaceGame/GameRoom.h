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
#include <vector>

#define OBJECT_NOT_FOUND -1

class GameRoom{
public:
	//default constructor
	GameRoom::GameRoom()
	{
		this->name = new char[MAX_NAME_CHARS + 1];
		this->name[MAX_NAME_CHARS] = '\0';
		strcpy(name, "");
		/*TO DO: Initialize lock*/
	}
	
	//constructor with name
	GameRoom::GameRoom(char *name)
	{
		this->name = new char[MAX_NAME_CHARS + 1];
		this->name[MAX_NAME_CHARS] = '\0';
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
	
	/* 	Loads a room object from a file in the room format 
		described in documentation. */ 
	static void load_room(char *fname, GameRoom &room);

	/* 	Writes a file in the specified room format 
		see documentation for further details*/ 
	static void write_room(char *fname, GameRoom &room);

private:
	char *name;
	vector<GameObject *> objects;
	
	// Comparator for Objects
	static bool ObjectComparator(GameObject *o1, GameObject *o2);
	
	// Returns the index of the Object specified by the given name in the given vector
	// Adapted from code found on Wikipedia (en.wikipedia.org/wiki/Binary_search_algorithm);
	int binaryObjectSearch(vector<GameObject *> objects, char *objectName);
	
	// Returns the midpoint of the given indices
	int GameRoom::midpoint(int imin, int imax) {return (imin + imax) / 2;}
};

