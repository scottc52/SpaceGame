
/*Author: Austin Hulse
 * Space Game
 */
 ////////////////////////////////////////////////////////////////////////////////
 // GAME OBJECT CLASS
 ////////////////////////////////////////////////////////////////////////////////

/* This class is the basic class for objects in the game.  Subclasses of this 
 * class include: ActiveObject, Camera, Character, Player, etc.
 *
 */
#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_
#ifdef _WIN32
#include "windows.h"
#endif
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
#include "Mesh.h"
#include "LocationDefines.h"

using namespace Eigen;

#define MAX_NAME_CHARS 40

//Abstract class GameObject
class GameObject{
private:
	//Pointer to the GameMesh stored elsewhere
	MyMesh* meshptr;
	Vec3f position;

	//In the form of quaternion data!
	Vec4f rotation;
	float scale;
	bool isModified;
	char name[MAX_NAME_CHARS];
	string meshFile;

	//TEXTURE POINTER txtptr;
	
	//Thread Lock (mutex_t ?) object lock;
	//Sound link sound??  Play sound indirectly?
	
	void setName(const char *c){
		int len = strlen(c);
		len = (len < MAX_NAME_CHARS-1)? len : MAX_NAME_CHARS-1; 
		strncpy(this->name, c, len);
		this->name[len] = '\0';  
	} 
public:

	//default constructor
	GameObject::GameObject(const char *n = "\0")
	{
		setName(n);		
		isModified = false;
		position = Vec3f(0,0,0);
		scale = 1.f;
		/*TO DO: Initialize lock*/
	}
	
	//constructor with Object
	GameObject::GameObject(GameObject* object)
	{
		setName(object->GetName());  		
		isModified = object->IsModified();
		position = object->GetPosition();
		scale = object->GetScale();
		meshFile = object->GetMeshFile();
		rotation = object->GetRotation();
		meshptr = object->GetMesh();
		/*TO DO: Initialize lock*/
	}
	
	//destructor
	GameObject::~GameObject(){}

	//Get a POINTER to the current Object's mesh
	MyMesh* GameObject::GetMesh(){return meshptr;}

	//Redefine the object's mesh
	void GameObject::SetMesh(MyMesh* NewMesh){meshptr = NewMesh;}

	//Getters and Setters//////////////////////////////////
	Vec3f GameObject::GetPosition(){return position;}
	void GameObject::MovePositon(Vec3f &delta){position += delta;}
	void GameObject::MovePositon(float x, float y, float z)
	{
		position[0] = position[0] + x;
		position[1] = position[1] + y;
		position[2] = position[2] + z;
	}
	void GameObject::SetPosition(Vec3f &newPos){position = newPos;}
	Vec4f GameObject::GetRotation(){return rotation;}
	void GameObject::SetRotation(Vec4f &newRotation){rotation = newRotation;}
	//void GameObject::RotateByQuaternion(Quaternionf &deltaQ){rotation = rotation + deltaQ;}
	float GameObject::GetScale(){return scale;}
	void GameObject::SetScale(float &newScale){scale = newScale;}
	bool GameObject::IsModified(){return isModified;}
	char* GameObject::GetName(){return name;}
	void GameObject::SetName(char* n){setName(n);}
	const char* GameObject::GetMeshFile(){return meshFile.c_str();}
	void GameObject::SetMeshFile(char* f){
		string meshFileName = GAME_DATA_MESH_FOLDER;
		meshFileName = meshFileName + f;
		meshFile.clear();
		meshFile = meshFileName;
	}

	/////////////////////////////////////////////////////////////////////////////////

	//Bools in case of error when locking...
	bool GameObject::Lock(){/*ToDo: write function for lock*/}
	bool GameObject::Unlock(){/*ToDo:: write function for unlock*/}

	//TO DO: Create linker function to link sound object to object.
	//TO DO: Create a getter/setter for an objects textures
};
#endif _GAMEOBJECT_H_
