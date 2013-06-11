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
//map<GameObject*, vector<GameObject*>> collisionTier0List;
//map<GameObject*, vector<GameObject*>> collisionTier1List;
//map<GameObject*, vector<GameObject*>> collisionTier2List;

using namespace std; 
//=========================
// IO function implementations
//=========================

#define MAX_LINE_LEN 1024
#define DELIM " \r\t\n\f" 


/*Vector3f ConvertToEigen3Vector(Vec3f& v){return Vector3f(v[0], v[1], v[2]);}
Vec3f ConvertToOM3Vector(Vector3f& v){return Vec3f(v.x(), v.y(), v.z());}
Vector4f ConvertToEigen4Vector(Vec4f& v){return Vector4f(v[1], v[2], v[3], v[0]);}
Vec4f ConvertToOM4Vector(Vector4f& v){return Vec4f(v.w(), v.x(), v.y(), v.z());}*/

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
			Vector3f pos (atof(v[1]), atof(v[2]), atof(v[3])); 
			Vector3f view (atof(v[4]), atof(v[5]), atof(v[6]));
			cam->SetPosition(pos);
			cam->SetViewVector(view);  

			if (v.size() >= 10){
				Vector3f up (atof(v[7]), atof(v[8]), atof(v[9]));	
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
		Vector3f pos(x, y, z);
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
		Vector3f p(x,y,z);
		Vector4f rot(nX, nY, nZ, angle);
		wobj.SetPosition(p);
		wobj.SetScale(scale);
		wobj.SetRotation(ConvertToOM4Vector(rot));
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
		Vector3f p(x,y,z); 			
		door.SetPosition(p);
		door.SetScale(scale);
		door.SetName(v[1]);
		door.SetMeshFile(newCString(v[2]));
		r.AddDoor(door);
		break;
			 } 
	case 'M':{
		GAME_DEBUG_ASSERT(v.size() >= 11);
		float x= atof(v[2]);
		float y= atof(v[3]);
		float z= atof(v[4]);
		float angle = atof(v[5]);
		float nX = atof(v[6]);
		float nY = atof(v[7]);
		float nZ = atof(v[8]);
		float scale = atof(v[9]);
		GamePlayer player;
		Vector3f p(x,y,z);
		Vector4f rot(nX, nY, nZ, angle);
		player.SetPosition(p);
		player.SetScale(scale);
		player.SetMass(atof(v[10]));
		player.SetRotation(ConvertToOM4Vector(rot));
		player.SetName(v[1]);
		r.AddPlayer(player); 
		break;
			 }
			 //Active Object
	case 'A':{
		GAME_DEBUG_ASSERT(v.size() >= 11);
		float x= atof(v[3]);
		float y= atof(v[4]);
		float z= atof(v[5]);
		float angle = atof(v[6]);
		float nX = atof(v[7]);
		float nY = atof(v[8]);
		float nZ = atof(v[9]);
		float scale = atof(v[10]);
		GameActiveObject aobj;
		Vector3f p(x,y,z);
		Vector4f rot(nX, nY, nZ, angle);
		aobj.SetPosition(p);
		aobj.SetScale(scale);
		aobj.SetRotation(ConvertToOM4Vector(rot));
		aobj.SetName(v[1]);
		aobj.SetMeshFile(newCString(v[2]));
		if(v.size() > 11){
			GAME_DEBUG_ASSERT(v.size() >=14);
			float vx = atof(v[11]);
			float vy = atof(v[12]);
			float vz = atof(v[13]);
			aobj.velocity = Vec3f(vx, vy, vz);
		}
		if(v.size() >= 14){
			GAME_DEBUG_ASSERT(v.size() >=18);
			float avw = atof(v[14]);
			float avx = atof(v[15]);
			float avy = atof(v[16]);
			float avz = atof(v[17]);
			aobj.angularVelocity = Vec4f(avw, avx, avy, avz);
		}
		if(v.size() >=19){
			aobj.mass = atof(v[18]);
		}
		r.AddActiveObject(aobj); 
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
void GameRoom::AddActiveObject(GameActiveObject& newWObject)
{
	string key(newWObject.GetName());
	(this->aobjects)[key] = newWObject;
}

void GameRoom::AddItem(GameItem& newWObject)
{
	string key(newWObject.GetName());
	(this->items)[key] = newWObject;
}


void GameRoom::AddWorldObject(GameWorldObject& newWObject)
{
	string key(newWObject.GetName());
	(this->wobjects)[key] = newWObject;
}

void GameRoom::AddLight(GameLight& l){
	(this->lights)[l.GetName()]=l;
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


//Adds the given Object to the Room
void GameRoom::AddPlayer(GamePlayer& newPlayer){
	this->players[(string)newPlayer.GetName()] = newPlayer;
}

//Get the Object with the given name in the current Room if one exists
GamePlayer* GameRoom::GetPlayer(const char *pName){
	if(players.find((string)pName) == players.end()) return NULL;
	return &(players[(string)pName]);
}

//Remove the Object with the given name in the current Room if it exists
void GameRoom::RemovePlayer(const char *pName){
	map<string, GamePlayer>::iterator it = players.find(pName);
	if(it != players.end()) players.erase(it);
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
