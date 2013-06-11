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
#include "CollisionDetection.h"
using namespace std;

#define STATE_FILE_DELIM ('$')

GameState* GameState::m_pinstance = NULL; 

GameState *GameState::GetInstance(){
	if(!m_pinstance) m_pinstance = new GameState(); return m_pinstance;
}

void splitString(string &s, vector<string>& buff){
	buff.clear(); 
	istringstream iss(s);
	string token;
	while (getline(iss, token, ' ')){
		buff.push_back(token);
	}
	return; 
}

/////////////////////////////////////////////////////////////////////////////////////////
// OBJECT UPDATING
/////////////////////////////////////////////////////////////////////////////////////////

bool UpdateObject(ifstream &file, GameRoom& r){
	string line;
	bool delim = false;
	string name;
	getline(file,name);
	if(name[0] == '$') return true;
	GameActiveObject* aobj = r.GetActiveObject(name.c_str());
	if(aobj == NULL){
		ostringstream message;
		message<<"State Machine could not find object of name: ";
		message<<name;
		GameDebugger::GetInstance()->WriteToDebugFile(message.str().c_str());
		return false;
	}
	while(!file.eof()){
		getline(file, line);
		if(line[0] == STATE_FILE_DELIM){
			delim = true;
			break;
		}
		switch(line[0]){
			///////////////Position//////////////
		case 'p':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 4){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad position input");
			}else{
				Vector3f pos(atof(parsed[1].c_str()), atof(parsed[2].c_str()), atof(parsed[3].c_str()));
				aobj->SetPosition(pos);
			}
			break;
				 }
				 ///////////////////Rotation////////////////////////////////
		case 'r':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 5){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
				aobj->SetRotation(Vec4f(atof(parsed[1].c_str()), atof(parsed[2].c_str()), atof(parsed[3].c_str()), atof(parsed[3].c_str())));
			}
			break;
				 }
				 ///////////////Forces//////////////////////////////////
		case 'f':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			/* To do: 
			Set Physics Forces.....

			if(parsed.size() != 5){
			GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
			aobj->SetRotation(Vector4f(atof(parsed[1].c_str()), atof(parsed[2].c_str()), atof(parsed[3].c_str()), atof(parsed[3].c_str())));
			}
			*/
			break;
				 }
				 //////////////////////State/////////////////////////////////////
		case 's':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 2){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad state input");
			}else{
				aobj->SetState(parsed[1].c_str());
			}
			break;
				 }
				 ///////////////////////Delete//////////////////////////
		case 'd':{
			r.RemoveActiveObject(aobj->GetName());
			break;
				 }
		default:{
			GameDebugger::GetInstance()->WriteToDebugFile("Invalid State Machine File entry");
			break;
				}
		}
	}
	if(!delim){
		GameDebugger::GetInstance()->WriteToDebugFile("State Machine improper formatting...");
	}
	return delim;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ITEM UPDATE
///////////////////////////////////////////////////////////////////////////////////////////////////


bool UpdateItem(ifstream &file, GameRoom& r){
	string line;
	bool delim = false;
	string name;
	getline(file,name);
	if(name[0] == '$') return true;
	GameItem* item = r.GetItem(name.c_str());
	if(item == NULL){
		ostringstream message;
		message<<"State Machine could not find object of name: ";
		message<<name;
		GameDebugger::GetInstance()->WriteToDebugFile(message.str().c_str());
		return false;
	}
	while(!file.eof()){
		getline(file, line);
		if(line[0] == STATE_FILE_DELIM){
			delim = true;
			break;
		}
		switch(line[0]){
			///////////////////////Item Property////////////////////////////
		case 'p':{

			break;
				 }
		case 'a':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 2){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
				item->SetAcquired(atoi(parsed[1].c_str()));
			}
			break;
				 }
		case 'u':{
			vector<string> parsed;
			istringstream unparsed(line);
			string token;
			while(getline(unparsed, token, ' ')){
				parsed.push_back(token);
			}
			if(parsed.size() != 2){
				GameDebugger::GetInstance()->WriteToDebugFile("Bad rotation input");
			}else{
				item->SetNumUses(atoi(parsed[1].c_str()));
			}
			break;
				 }
		case 'd':{
			r.RemoveItem(item->GetName());
			break;
				 }
		default:{
			GameDebugger::GetInstance()->WriteToDebugFile("Invalid State Machine File entry");
			break;
				}
		}
	}
	if(!delim){
		GameDebugger::GetInstance()->WriteToDebugFile("State Machine improper formatting...");
	}
	return delim;
}

///////////////////////////////////READ STATE FILE/////////////////////////////////////////////

bool GameState::ReadStateFile(const char *fname){
	GameRoom* r = this->GetRoom();
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
	if(!GameRoom::LoadRoom(roomFile.c_str(), (*r))){
		string message = "Room file: ";
		message += roomFile.c_str();
		message += " while Loading in the state machine, is not a valid Room File\n";
		GameDebugger::GetInstance()->WriteDebugMessageToConsole(message.c_str(), 150);
		return false;
	}
	string line; 
	while (myfile.good()){
		getline(myfile, line);
		switch(line[0]){
		case'O':{
			if(!UpdateObject(myfile, (*r))){
				GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
				return false;
			}
			break;
				}
		case 'P':{
			//	if(!UpdatePlayer(myfile)){
			//		GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
			//		return false;
			//	}
			break;
				 }
		case 'I': {
			if(!UpdateItem(myfile, (*r))){
				GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
				return false;
			}
			break;
				  }
		case 'C':{
			//if(!UpdateCamera(myfile, room)){
			//	GameDebugger::GetInstance()->WriteToDebugFile("Problem in state machine");
			//		return false;
			//	}
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




///////////////////////////////////////////////////////////////////////////////////////////////
//     WRITE STATE
///////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//Particle Systems Manager
////////////////////////////////////////////////////////////////

bool pIsDead(Projectile *p){
	return p->isDead();
}

#define BRANCH_FACTOR 2
#include "TaskQueue.h"
class BulletUpdater : public Task{
private:
	vector<Projectile *> toUpdate;

public:
	double dt; 
	BulletUpdater(){} 
	void AddBullet(Projectile *p){
		toUpdate.push_back(p);
	}
	virtual void run(){
		for(int i = 0; i < toUpdate.size(); i++){
			Projectile *p = toUpdate[i];
			if(p->isDead()){
				continue; 
			}
			if(p->timeAlive() > 3000){
				p->hit(p->getPosition()); 
			}
			p->update(dt);
		}
	}
};

void PSystems::updateAll(double dt){
	int idx = 0;
	BulletUpdater jobs[BRANCH_FACTOR]; 
	monitor.Enter('w');
	list<Projectile *>::iterator it = projectiles.begin();
	while(it != projectiles.end()){
		jobs[idx].AddBullet(*it);
		jobs[idx].dt = dt;
		it ++; 	
		idx ++;
		idx %=BRANCH_FACTOR;  
	} 
	
	for( int i = 0; i < BRANCH_FACTOR; i ++) {
		TaskQueue::defQueue->enqueue(&jobs[i]); 
	}
	for( int i = 0; i < BRANCH_FACTOR; i ++) {
		jobs[i].join(); 
	}
	projectiles.remove_if(pIsDead);
	monitor.Exit('w');
	cout << "finished" << endl;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//  STATE MACIHNE ITERATTION
///////////////////////////////////////////////////////////////////////////////////////////////

#define LOOK_SENSITIVITY ((float)1.0f)
#define MOVE_SENSITIVITY ((float)1.0f)

void GameState::ProcessInput(list<UIEvent *> input, double dt){
	list<UIEvent *>::iterator it = input.begin(); 
	if (it == input.end())
		return;
	Vector2f deltaPos(0,0); 
	Vector2f deltaView(0,0);
	Vector3f nPos = cam->getPivotPoint(); 
	Vector3f up = cam->getUpVector();
	Vector3f dir = cam->getDirection(); 
	Vector3f strafe(dir.cross(up).normalized());  
	while (it != input.end()){
		UIEvent *ev = *it; 
		int action = ev->value;
		if(action == USER_COMMAND_LOOK_UP || action == USER_COMMAND_LOOK_DOWN || 
			action == USER_COMMAND_LOOK_RIGHT || action == USER_COMMAND_LOOK_LEFT){
				deltaView += ev->delta;   
		} else{
			switch (action){
			case (USER_COMMAND_STRAFE_RIGHT): {
				cout << "mR" << endl; 
				deltaPos[0] += 1; 
				break; } 
			case (USER_COMMAND_STRAFE_LEFT):{
				cout << "mL" << endl; 
				deltaPos[0] -= 1; 
				break;}
			case (USER_COMMAND_MOVE_FORWARD):{
				cout << "mF" << endl; 
				deltaPos[1] += 1;
				break;}
			case (USER_COMMAND_MOVE_BACKWARD):{
				cout << "mB" << endl; 
				deltaPos[1] -= 1; 
				break;}
			case (USER_COMMAND_FIRE_WEAPON):{ 
				cout << "firing" << endl;
				Vector3f loc(nPos);
				loc += dir.cross(strafe); //below camera
				if (player.GetActiveWeapon()){
					Projectile *p = player.GetActiveWeapon()->fire(loc, dir); 
					if (p){
						ps->monitor.Enter('w');
							ps->AddBullet(p);
						ps->monitor.Exit('w');
					}
				} else {
					cerr << "error: no active weapon" << endl;
				}
				break;
											}
			case (USER_COMMAND_SWITCH_WEAPON):{ 
				cout << "weapons swapped" << endl; 
				player.SwitchWeapons();
				break;}
			case (USER_COMMAND_TOGGLE_ZOOM):{
				cout << "zooming" << endl; 
				break;}
			case (USER_COMMAND_USE_WEAPON) :{
				cout << "weapon used" << endl;  break;}
			case (USER_COMMAND_JUMP) :{ 
				cout << "Jumping" << endl; break;}
			case (USER_COMMAND_INVENTORY) :{ 
				cout << "INV accessed" << endl; break;}
			case (USER_COMMAND_PAUSE_MENU) :{ 
				cout << "Paused" << endl; 
				if (paused){
					paused = false; 
					PCInputManager::setMouseLock(true);
				}else {
					paused = true; 
					PCInputManager::setMouseLock(false); 
				}
				break;
											}
			case (USER_COMMAND_ACTION_OR_CONFIRM):{ 
				cout << "Enter Pressed" << endl;
				break;}
			}
		}

		delete ev; 
		it++; 
	}


	nPos += strafe * deltaPos[0];
	nPos += dir * deltaPos[1];
	Matrix3f Rphi = AngleAxisf(deltaView[1]/ (double)Render::h * 80.0 / 180.0 * PI * LOOK_SENSITIVITY, strafe).matrix();
	//up = Rphi*up; 
	dir = Rphi * dir;    
	Matrix3f Rtheta = AngleAxisf(deltaView[0]/(double)Render::w * 100.0 / 180.0 * PI * LOOK_SENSITIVITY, up).matrix();
	dir = Rtheta * dir;
	cam->setPivotPoint(nPos);
	//cam->setUpVector(up);
	cam->setDirection(dir);    
}

void GameState::PerformStateActions(list<UIEvent *> input, double dt /*ms*/){
	//If save, handle save

	//If change room, handle change room

	dt/=1000;

	//Player action
	ProcessInput(input, dt);
	UpdateParticleSystems(dt);

	room->monitor.Enter('w');
	vector<GameObject*>objects = room->GetGameObjects();
	for(unsigned int o = 0; o<objects.size(); o++){
		GameObject* obj = objects[o];
		if(obj->objType == CAMERA_TYPE || obj->objType == DOOR_TYPE || obj->objType == WORLD_OBJECT_TYPE || obj->objType == ITEM_TYPE) 
			continue;
		Vector3f newPos = obj->GetPosition() + ConvertToEigen3Vector(obj->velocity*dt);
		obj->SetPosition(newPos);
		float newAngle = obj->GetRotation()[0] + obj->angularVelocity[0]*dt;
		Vec3f axisOfRotation = Vec3f(obj->GetRotation()[1], obj->GetRotation()[2], obj->GetRotation()[3]);
		axisOfRotation += Vec3f(obj->angularVelocity[1], obj->angularVelocity[2], obj->angularVelocity[3])*dt;
		axisOfRotation.normalize();
		obj->SetRotation(Vec4f(newAngle, axisOfRotation[0], axisOfRotation[1], axisOfRotation[2]));
	}
	
	PerformCollisionDetection(room, &(GameState::player), dt);
	
	room->monitor.Exit('w');
	//AI Calls
	//To do: Collision detection, update forces
	//Camera movement 

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
