/* GameRoom.cpp */

//======================
// Includes
//======================
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "GameDebug.h"
#include "GameRoom.h"
#include "GameCamera.h"
#include "GameObject.h"
#include "GameLight.h"

using namespace std; 
//=========================
// IO function implementations
//=========================

#define MAX_LINE_LEN 1024
#define DELIM " \r\t\n\f" 

char *newCString(const char *c){
	int len = strlen(c);
	char *n = new char[len + 1];	
	strncpy(n, c, len + 1);
	return n; 
}

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
		//room id		
		case 'N': {
			r.SetName(v[1]);
			break;
		}
		//name of entrance object 
		case 'R':{ 	
			string entrance_name(v[1]);
			break;
		}

		//camera location (3rd person location or FP)	
		case 'C':{
			GameCamera *cam = NULL; 
			
			if (strcmp(v[1], "player")){ // ie not first-person 
				cam = new GameCamera(); 
				Vec3f pos (atof(v[1]),
					atof(v[2]), atof(v[3])); 
				Vec3f view (atof(v[4]),
					atof(v[5]), atof(v[6]));
				cam->SetPosition(pos);
				cam->SetViewVector(view);  
						
				if (v.size() >= 10){
					Vec3f up (
						atof(v[7]), atof(v[8]), atof(v[9]));	
				cam->SetUpVector(up);				
				}
			}
			
			r.SetCamera(cam);	
			break; 
		}
		//Light
		case 'L':{
			GAME_DEBUG_ASSERT(v.size() >= 14); 
			float x = atof(v[2]);
			float y = atof(v[3]);
			float z	= atof(v[4]);
			float ra = atof(v[5]); 
			float ga = atof(v[6]); 
			float ba = atof(v[7]);
			float rd = atof(v[8]); 
			float gd = atof(v[9]); 
			float bd = atof(v[10]); 
			float rs = atof(v[11]);
			float gs = atof(v[12]); 
			float bs = atof(v[13]);
			if (v.size() > 14) // not yet used  
				string info(v[14]); 		
			Vec3f pos(x, y, z);
			Vec4f amb(ra, ga, ba, 1.0f);
			Vec4f spec(rs, gs, bs, 1.0f);
			Vec4f diff(rd, gd, bd, 1.0f);
			GameLight *light = new GameLight();
			light->SetPosition(pos);
			light->SetName(newCString(v[1]));
			light->SetAmbient(amb);
			light->SetDiffuse(diff);
			light->SetSpecular(spec);
			r.AddLight(light);  
			break;
		}
		// Object / Actor
		// local_name global_name x y z rotation scale		
		case 'O':{
			GAME_DEBUG_ASSERT(v.size() >= 7);
			float x= atof(v[3]);
			float y= atof(v[4]);
			float z= atof(v[5]);
			//float rot = atof(v[6]);
			float scale = atof(v[6]);
			GameObject *obj = new GameObject();
			Vec3f p(x,y,z); 			
			obj->SetPosition(p);
			obj->SetScale(scale);
			obj->SetName(newCString(v[1]));
			obj->SetMeshFile(newCString(v[2]));
			r.AddObject(obj);
			break; 
		}
		//load object as a door
		case 'P':{
			GAME_DEBUG_ASSERT(v.size() >= 7 );
			float x= atof(v[3]);
			float y= atof(v[4]);
			float z= atof(v[5]);
			//float rot = atof(v[6]);
			float scale = atof(v[6]);
			
			GameObject *obj = new GameObject();
			Vec3f p(x,y,z); 			
			obj->SetPosition(p);
			obj->SetScale(scale);
			obj->SetName(newCString(v[1]));
			obj->SetMeshFile(newCString(v[2]));
			r.AddObject(obj);						
			break;
		} 
		case '\0': 
		default: 
			GameDebugger::GetInstance()->WriteDebugMessageToConsole("parseRoomFile: unexpected line type");
			break; 	
	}
}

bool GameRoom::LoadRoom(char *fname, GameRoom& room){
	std::ifstream myfile (fname, std::ifstream::in);
	if (!myfile.good()){
		myfile.close();
		return false;
	}
	char line[MAX_LINE_LEN]; 
	vector<char *> parsed; 
	while (myfile.good()){
		myfile.getline(line, MAX_LINE_LEN);
		split(line, DELIM, parsed);
		if (parsed.size() == 0) continue;
		parseRoomLine(parsed, room);      
	}

	myfile.close(); 
	return true; 
}

bool writeGameObject(ostream &os, GameObject *o){
	os << o->GetName() << "\t" << o->GetMeshFile();
	Vec3f pos = o->GetPosition();
	os << "\t" << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\t";
	os << "\t" << o->GetScale();
	return true;
}	

bool GameRoom::WriteRoom(char *fname, GameRoom& room){
	int olen = room.objects.size(); 	
	ofstream os(fname, ofstream::out); 	
	for(int i = 0; i < olen; i++){
		os << "O\t";
		writeGameObject(os, room.objects[i]);
		os << endl;  
	} 
	os.close();
	return false;	
}

//=======================================
//  Other Member function implementations
//=======================================

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
