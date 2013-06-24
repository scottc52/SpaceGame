/* 
 * Functionality for room builder
 *
 */

#include "RoomBuilder.h"
#include "GameObjectHeaderList.h"

/* Room setup command via the console....*/
void ConsoleCreateRoom(){
	char response;
	//cout<<"Would you like to create a new room? (y for yes) ";
	//cin>>response;
	//cin.clear();
	//cin.ignore(10000, '\n');
	//cout<<endl;
	response = 'n';
	while(response == 'y'){
		cout<<"Please name your room (max 32 chars): ";
		char roomName[33];
		cin>>roomName;
		cin.clear();
		cin.ignore(10000, '\n');
		GameRoom newRoom = GameRoom(roomName);
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
					char oName[10001];
					cin>>oName;
					cin.clear();
					cin.ignore(10000, '\n');
					float x, y, z;
					cout<<"x y z? ";
					cin>>x>>y>>z;
					cout<<endl<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;
					cin.clear();
					cin.ignore(10000, '\n');
					float angle;
					float qX, qY, qZ;
					cout<<"quat rotation: angle, axis ";
					cin>>angle>>qX>>qY>>qZ;
					cout<<endl<<"angle: "<<angle<<" axis: <"<<qX<<", "<<qY<<", "<<qZ<<">"<<endl;
					cin.clear();
					cin.ignore(10000, '\n');
					cout<<"Mesh name: ";
					char meshName[10001];
					cin>>meshName;
					cin.clear();
					cin.ignore(10000, '\n');
					//To do: Texture.
					//To do: Additional data.
					cout<<"Scale: ";
					float scale;
					cin>>scale;
					cin.clear();
					cin.ignore(10000, '\n');
					GameWorldObject wobj = GameWorldObject();
					wobj.SetName(oName);
					wobj.SetMeshFile(meshName);
					Vec4f rot(angle, qX, qY, qZ);
					wobj.SetRotation(rot);
					Vector3f p(x,y,z);
					wobj.SetPosition(p);
					wobj.SetScale(scale);
					//To do: Controller adds object into buffer?
					//To do: Room includes pointer to object from controller...
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
					Vector3f p(x,y,z);
					Vector3f u(uX, uY, uZ);
					Vector3f v(vX, vY, vZ); 
					cam.SetPosition(p);
					cam.SetUpVector(u);
					cam.SetViewVector(v);
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
