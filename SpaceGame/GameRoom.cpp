/* GameRoom.cpp */

//======================
// Includes
//======================
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>
#include "GameObjectHeaderList.h"
#include "LocationDefines.h"

#include "TextIOHelpers.h"

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
			Vec3f pos (atof(v[1]), atof(v[2]), atof(v[3])); 
			Vec3f view (atof(v[4]), atof(v[5]), atof(v[6]));
			cam->SetPosition(pos);
			cam->SetViewVector(view);  

			if (v.size() >= 10){
				Vec3f up (atof(v[7]), atof(v[8]), atof(v[9]));	
				cam->SetUpVector(up);				
			}
		}
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
		GameLight light;
		light.SetPosition(pos);
		light.SetName(v[1]);
		light.SetAmbient(amb);
		light.SetDiffuse(diff);
		light.SetSpecular(spec);
		r.AddLight(light);  
		break;
			 }
			 // Object / Actor
			 // local_name global_name x y z rotation scale		
	case 'W':{
		GAME_DEBUG_ASSERT(v.size() >= 11);
		float x= atof(v[3]);
		float y= atof(v[4]);
		float z= atof(v[5]);
		float angle = atof(v[6]);
		float nX = atof(v[7]);
		float nY = atof(v[8]);
		float nZ = atof(v[9]);
		float scale = atof(v[10]);
		GameWorldObject wobj;
		Vec3f p(x,y,z);
		Vec4f rot(angle, nX, nY, nZ);
		wobj.SetPosition(p);
		wobj.SetScale(scale);
		wobj.SetRotation(rot);
		wobj.SetName(v[1]);
		wobj.SetMeshFile(newCString(v[2]));
		r.AddWorldObject(wobj); 
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

		GameDoor door;
		Vec3f p(x,y,z); 			
		door.SetPosition(p);
		door.SetScale(scale);
		door.SetName(v[1]);
		door.SetMeshFile(newCString(v[2]));
		r.AddDoor(door);
		break;
			 } 
	case '\0': 
	default: 
		GameDebugger::GetInstance()->WriteDebugMessageToConsole("parseRoomFile: unexpected line type");
		break; 	
	}
}

bool GameRoom::LoadRoom(const char *fname, GameRoom& room){
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



bool GameRoom::WriteRoom(const char *fname, GameRoom& room){	
	ofstream ofs;
	ofs.open(fname);
	if (!ofs.is_open()){return false;}
	bool stat = WriteRoom(ofs, room);
	ofs.close();
	return stat;
}
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
	}*/

	int llen = room.lights.size();
	os << "# Lights: name x y z ra ga ba rd gd bd rs bs gs" << endl; 
	/*for(int i = 0; i < llen; i ++ ){
	os << "L\t";
	writeGameLight(os, room.lights[i]);
	os << endl; 
	}*/

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

//=======================================
//  Other Member function implementations
//=======================================

GameRoom::~GameRoom()
{
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
}

void GameRoom::AddWorldObject(GameWorldObject newWObject)
{
	this->wobjects[newWObject.GetName()] = newWObject;
}

void GameRoom::AddLight(GameLight l){
	this->lights[l.GetName()]=l;
}

GameWorldObject *GameRoom::GetWorldObject(const char *wobjectName)
{

	if(wobjects.find(wobjectName) == wobjects.end()) return NULL;
	else return &(wobjects[wobjectName]);
}

GameLight *GameRoom::GetLight(const char *lightName)
{
	if(lights.find(lightName) == lights.end()) return NULL;
	return &(lights[lightName]);
}

void GameRoom::RemoveWorldObject(const char *wobjectName)
{
	map<string, GameWorldObject>::iterator it = wobjects.find(wobjectName);
	if(it != wobjects.end()) wobjects.erase(it);
}

void GameRoom::PrintWorldObjectNames()
{
	/*int objectSize = this->objects.size();
	for (int i = 0; i < (objectSize - 1); i++)
	{
	cout << this->objects[i]->GetName() << ", ";
	}
	cout << this->objects[objectSize - 1]->GetName() << endl;
	*/
}
