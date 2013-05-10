/* GameRoom.cpp */

//======================
// Includes
//======================
#include <fstream>
#include <cstring>
#include <vector>
#include "GameDebug.h"
#include "GameRoom.h"
#include "GameCamera.h"

using namespace std; 
//=========================
// IO function implementations
//=========================

#define MAX_LINE_LEN 1024
#define DELIM " \r\t\n\f" 

void split(char *s, const char *delim, vector<char *> &buff){
	buff.clear(); 
	char *tmp = strtok(s, delim);
	while (tmp != NULL){
		buff.push_back(tmp);
		tmp = strtok (NULL, delim);
	}
	return; 
}

//Heavy lifting for reading info from lines
void parseRoomLine(vector<char *> &v, GameRoom &r){
	char* line_type = v[0];
	
	//comment
	if (line_type[0] == '#')
		return; 

	if (v.size() < 2){
		GameDebugger::GetInstance()->WriteDebugMessageToConsole("parseRoomFile: line");  
	}
	switch (line_type[0]) {
		//name of entrance object 
		case 'R':{ 	
			string entrance_name(v[1]);
			break;
		}

		//camera location (3rd person location or FP)	
		case 'C':{
			GameCamera cam; 
			 
			if (strcmp(v[1], "player")){

			} else { 
				Vec3f pos (atof(v[1]),
					atof(v[2]), atof(v[3])
				); 
				Vec3f view (atof(v[4]),
					atof(v[5]), atof(v[6])
				); 
				if (v.size() >= 10){
					Vec3f up (
						atof(v[7]), atof(v[8]), atof(v[9])
					);	
				}  
			}

			break; 
		}
		//Light
		case 'L':{
			string light_name(v[1]); 
			float x = atof(v[2]);
			float y = atof(v[3]);
			float z	= atof(v[4]);
			float rs = atof(v[5]); 
			float gs = atof(v[6]); 
			float bs = atof(v[7]); 
			float rd = atof(v[8]);
			float gd = atof(v[9]); 
			float bd = atof(v[10]); 
			string info(v[11]); 		

			break;
		}
		// Object / Actor
		// local_name global_name x y z rotation scale		
		case 'O':{
			string object_name(v[1]);
			string object_fname(v[2]);
			float x= atof(v[3]);
			float y= atof(v[4]);
			float z= atof(v[5]);
			float rot = atof(v[6]);
			float scale = atof(v[7]);
		
			break; 
		}
		//load object as a door
		case 'P':{
			string object_name(v[1]);
			string object_fname(v[2]);
			float x= atof(v[3]);
			float y= atof(v[4]);
			float z= atof(v[5]);
			float rot = atof(v[6]);
			float scale = atof(v[7]);
	
			break;
		} 
		case '\0':
		default: 
			GameDebugger::GetInstance()->WriteDebugMessageToConsole("parseRoomFile: unexpected line type");
			break; 	
	}
}

void GameRoom::load_room(char *fname, GameRoom& room){
	std::ifstream myfile (fname, std::ifstream::in);

	char line[MAX_LINE_LEN]; 
	vector<char *> parsed; 
	while (myfile.good()){
		myfile.getline(line, MAX_LINE_LEN);
		split(line, DELIM, parsed);
		if (parsed.size() == 0) continue;
		parseRoomLine(parsed, room);      
	}

	myfile.close(); 

}

void GameRoom::write_room(char *fname, GameRoom& room){

}

GameRoom::~GameRoom()
{
	delete this->name;
	int objectSize = this->objects.size();
	for (int i = 0; i < objectSize; i++)
	{
		delete this->objects[i];
	}
}

void GameRoom::AddObject(GameObject *newObject)
{
	GameObject *object = new GameObject(newObject);
	this->objects.push_back(object);
	
	sort(this->objects.begin(), this->objects.end(), ObjectComparator);
}

GameObject *GameRoom::GetObject(char *objectName)
{
	int index = binaryObjectSearch(this->objects, objectName);
	if (index != OBJECT_NOT_FOUND)
	{
		return this->objects[index];
	}
	else
	{
		return NULL;
	}
}

void GameRoom::RemoveObject(char *objectName)
{
	int index = binaryObjectSearch(this->objects, objectName);
	if (index != OBJECT_NOT_FOUND)
	{
		delete this->objects[index];
		this->objects.erase(this->objects.begin() + index);
	}
	
	sort(this->objects.begin(), this->objects.end(), ObjectComparator);
}

void GameRoom::PrintObjectNames()
{
	int objectSize = this->objects.size();
	for (int i = 0; i < (objectSize - 1); i++)
	{
		cout << this->objects[i]->GetName() << ", ";
	}
	cout << this->objects[objectSize - 1]->GetName() << endl;
}

bool GameRoom::ObjectComparator(GameObject *o1, GameObject *o2)
{
	return (strcmp(o1->GetName(), o2->GetName()) < 0);
}


int GameRoom::binaryObjectSearch(vector<GameObject *> objects, char *objectName)
{
	int imin = 0;
	int imax = objects.size() - 1;
	
	// continue searching while [imin,imax] is not empty
	while (imax >= imin)
	{
		/* calculate the midpoint for roughly equal partition */
		int imid = midpoint(imin, imax);
		// determine which subarray to search
		if (strcmp(objects[imid]->GetName(), objectName) < 0)
		{
			// change min index to search upper subarray
			imin = imid + 1;
		}
		else if (strcmp(objects[imid]->GetName(), objectName) > 0)
		{
			// change max index to search lower subarray
			imax = imid - 1;
		}
		else
		{
			// key found at index imid
			return imid;
		}
	}
	// key not found
	return OBJECT_NOT_FOUND;
}