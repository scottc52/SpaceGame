/* GameRoom.cpp */

//======================
// Includes
//======================
#include <fstream>
#include <cstring>
#include "GameDebug.h"
#include "GameRoom.h"

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
void parseRoomLine(vector<char *> &v, Room &r){
	char* line_type = v[0];
	
	//comment
	if (strcomp(line_type,'#') == 0)
		return; 

	if (v.size() < 2){
		DebugWriteMessageToConsole("parseRoomFile: line");  
	}
	switch(line_type[0]){
		//name of entrance object 
		case 'R': 	
			string entrance_name(v[1]);
			break;

		//camera location (3rd person location or FP)	
		case 'C':
			GameCamera cam; 
			 
			if (strcomp(v[1], "player")){

			} else { 
				Vec3f view (atof(v[1]),
					atof(v[2]), atof(v[3])
				); 
				if (v.size() >= 7){
					Vec3f up (
						atof(v[4]), atof(v[5]), atof(v[6])
					);	
				}  
			}

			break; 

		//Light
		case 'L':
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

		// Object / Actor
		// local_name global_name x y z rotation scale		
		case 'O':
			string object_name(v[1]);
			string object_fname(v[2]);
			float x= atof(v[3]);
			float y= atof(v[4]);
			float z= atof(v[5]);
			float rot = atof(v[6]);
			float scale = atof(v[7]);
		
			break; 

		//load object as a door
		case 'P':
			string object_name(v[1]);
			string object_fname(v[2]);
			float x= atof(v[3]);
			float y= atof(v[4]);
			float z= atof(v[5]);
			float rot = atof(v[6]);
			float scale = atof(v[7]);
	
			break; 
		case '\0':
		default: 
			DebugWriteMessageToConsole("parseRoomFile: unexpected line type");
			break; 	
	}
}

void GameRoom::load_room(char *fname, Room& room){
	std::ifstream myfile (fname, std::ifstream::in);

	char buf[MAX_LINE_LEN]; 
	vector<char *> parsed; 
	while (myfile.good()){
		myfile.getline(buf, MAX_LINE_LEN);
		split(line, DELIM, parsed);
		if (parsed.size() == 0) continue;
		parseRoomLine(parsed, room);      
	}

	myfile.close(); 

}

void GameRoom::write_room(char *fname, Room& room){

}