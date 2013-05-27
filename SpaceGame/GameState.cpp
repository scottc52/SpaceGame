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
//Heavy lifting for reading info from lines
void parseStateLine(vector<string> &v, GameState* sm, GameRoom &room){
	string line_type = v[0];
	
	//comment
	if (line_type[0] == '#')
		return; 
	switch (line_type[0]) {
		//Object update		
	case 'O': {
		string objectName = v[1];
		GameActiveObject* aobj = room.GetActiveObject(v[1].c_str());
		if(aobj == NULL){
			string message = "Could not find object of name: ";
			message += v[1].c_str();
			GameDebugger::GetInstance()->WriteToDebugFile(message.c_str(), 39);
		}else{
			for(size_t i = 0; i < v.size(); i++){
			}
		}
		break;
			  }
			  //name of entrance object 
	/*case 'R':{ 	
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
			 }*/ 
	case '\0':{
		break;
			  }
	default: 
		GameDebugger::GetInstance()->WriteDebugMessageToConsole("parseRoomFile: unexpected line type");
		break; 	
	}
}

bool GameState::ReadStateFile(const char *fname, GameState* sm, GameRoom &room){
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
	if(!GameRoom::LoadRoom(roomFile.c_str(), room)){
		string message = "Room file: ";
		message += roomFile.c_str();
		message += " while Loading in the state machine, is not a valid Room File\n";
		GameDebugger::GetInstance()->WriteDebugMessageToConsole(message.c_str(), 150);
		return false;
	}
	string line; 
	vector<string> parsedObj; 
	while (myfile.good()){
		getline(myfile, line);
		switch(line[0]){
		case'O':{

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
