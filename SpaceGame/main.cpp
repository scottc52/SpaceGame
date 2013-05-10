//////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////

/* Include windows only for windows users :P
*/
#include "windows.h"
#include <gl/glew.h>
#include "gl/GL.h"
#include <gl/freeglut.h>
#include <cassert>
#include <iostream>
#include "Mesh.h"
#include "GameDebug.h"
#include "GameWorldObject.h"
#include "GameCamera.h"
#include "GameDoor.h"
#include "GameItem.h"
#include "GameLight.h"

using namespace std;

/* Room setup command via the console....*/
void ConsoleCreateRoom(){
	char response;
	cout<<"Would you like to create a new room? (y for yes) ";
	cin>>response;
	cin.clear();
	cin.ignore(10000, '\n');
	cout<<endl;
	while(response == 'y'){
		cout<<"Please name your room (max 32 chars): ";
		char roomName[33];
		cin>>roomName;
		cin.clear();
		cin.ignore(10000, '\n');
		//To do: Create Room object.
		bool loop = true;
		char createChar;
		while(loop){
			cout<<endl<<"Object (o), Camera (c), Light (l), Door [Portal] (p), Save (s), Delete Item (d) ";
			cin>>createChar;
			cin.clear();
			cin.ignore(10000, '\n');
			switch (createChar){
			case 'o':
				{
					cout<<"Name: ";
					char name[10001];
					cin>>name;
					cin.clear();
					cin.ignore(10000, '\n');
					float x, y, z;
					cout<<"x y z? ";
					cin>>x>>y>>z;
					cout<<endl<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;
					cin.clear();
					cin.ignore(10000, '\n');
					float angle;
					float nX, nY, nZ;
					cout<<"quat rotation: angle, axis ";
					cin>>angle>>nX>>nY>>nZ;
					cout<<endl<<"angle: "<<angle<<" axis: <"<<nX<<", "<<nY<<", "<<nZ<<">"<<endl;
					cin.clear();
					cin.ignore(10000, '\n');
					cout<<"Mesh name: ";
					char meshName[10001];
					cin>>meshName;
					cin.clear();
					cin.ignore(10000, '\n');
					//To do: Texture.
					//To do: Additional data.
					//NOTE: For now, just create WorldObject, but later will make correct object....
					cout<<"Scale: ";
					float scale;
					cin>>scale;
					cin.clear();
					cin.ignore(10000, '\n');
					GameWorldObject wobj;
					wobj.SetName(name);
					wobj.SetMeshFile(meshName);
					wobj.SetRotation(Quaternion(angle, Vec3f(nX, nY, nZ)));
					wobj.SetPosition(Vec3f(x, y, z));
					wobj.SetScale(scale);
					//To do: use Room callback to add to room.
					break;
				}
			case 'c':
				{
					cout<<"Name: ";
					char name[10001];
					cin>>name;
					cin.clear();
					cin.ignore(10000, '\n');
					float x, y, z;
					cout<<"x y z? ";
					cin>>x>>y>>z;
					cout<<endl<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;
					cin.clear();
					cin.ignore(10000, '\n');
					float uX, uY, uZ;
					cout<<"Up vector: ";
					cin>>uX>>uY>>uZ;
					cin.clear();
					cin.ignore(10000, '\n');
					cout<<endl<<"Camera up vec: <"<<uX<<", "<<uY<<", "<<uZ<<">"<<endl;
					float vX, vY, vZ;
					cout<<"View vector: ";
					cin>>vX>>vY>>vZ;
					cin.clear();
					cin.ignore(10000, '\n');
					cout<<endl<<"Camera view vec: <"<<vX<<", "<<vY<<", "<<vZ<<">"<<endl;
					//To do: Additional data.
					GameCamera cam;
					cam.SetName(name);
					cam.SetPosition(Vec3f(x,y,z));
					cam.SetUpVector(Vec3f(uX,uY,uZ));
					cam.SetViewVector(Vec3f(vX,vY,vZ));
					//To do: use Room callback to add to room.
					break;
				}
			case 'l':
				{
					break;
				}
			case 'p':
				{
					break;
				}
			case 's':
				{
					break;
				}
			case 'd':
				{
					break;
				}
			default:
				loop = false;
				break;
			}
		}
		cout<<"Make another room? (y for yes) ";
		cin>>response;
	}
}


int main(int _argc, char** _argv){
	int err = GameDebugInitialize();
	assert(err == 0);
	err = GameDebugFinish();
	ConsoleCreateRoom();
	assert(err == 0);
	cin.ignore(1);
	return 0;
}