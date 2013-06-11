
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
#include <string>


using namespace std;
using namespace Eigen;

#define MAX_NAME_CHARS 40

class Projectile;

/////////////////////////////////////////////////////////
// TYPEDEFINES
//////////////////////////////////////////////////////

#define PLAYER_TYPE (0)
#define CAMERA_TYPE (1)
#define ACTIVE_OBJECT_TYPE (2)
#define WORLD_OBJECT_TYPE (3)
#define ITEM_TYPE (4)
#define DOOR_TYPE (5)
#define LIGHT_TYPE (6)



struct CollisionData{
	CollisionData(){}
	~CollisionData(){}
	Vec3f pointOfContact;
	Vec3f contactNormal;
};


//Abstract class GameObject
class GameObject{
private:
	//Pointer to the GameMesh stored elsewhere
	MyMesh* meshptr;
	Vector3f position;

	//In the form of quaternion data!
	Vec4f rotation;
	float scale;
	bool isModified;
	char name[MAX_NAME_CHARS];
	string meshFile;
	//Thread Lock (mutex_t ?) object lock;
	//Sound link sound??  Play sound indirectly?

	void setName(const char *c){
		int len = strlen(c);
		len = (len < MAX_NAME_CHARS-1)? len : MAX_NAME_CHARS-1; 
		strncpy(this->name, c, len);
		this->name[len] = '\0';  
	}
	
/*	void GameObject::UpdateCoords(vector<Vec3f>& v, Vec3f position, Vec3f velocity, Vec4f rotation, Vec4f wvelocity, float dt, float scale, bool pers, float inter, float xScale, float yScale, float zFar, float zNear){
		float angle = rotation[0];
		angle += wvelocity[0]*dt*inter;
		Vector3f newPos = Vector3f(position[0] + velocity[0]*dt*inter, position[1] + velocity[1]*dt*inter, position[2] + velocity[2]*dt*inter);
		Vector3f axis(rotation[1], rotation[2], rotation[3]);
		axis.normalize();
		Translation3f move =Translation3f(newPos.x(), newPos.y(), newPos.z());
		angle = angle*2*M_PI/360;
		AngleAxisf turn = AngleAxisf(angle, axis);
		Matrix4f pmat;
		if(pers){
			pmat << xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, -(zFar+zNear)/(zFar-zNear), -1,
			0, 0, -2*zNear*zFar/(zFar-zNear), 0;
		}else{
			pmat = Matrix4f::Identity();
		}
		for(unsigned int boxV = 0; boxV < v.size(); boxV++){
			Vector3f p = Vector3f(v[boxV][0], v[boxV][1], v[boxV][2]);
			/*Vector4f pt(p.x(), p.y(), p.z(), 1);
			 pt = pmat*pt;
			 p = Vector3f(pt.x(), pt.y(), pt.z());*/
/*			p = scale* p;
			if(axis.norm() > 0)
				p = turn* p;
			p = move* p;
			v[boxV] = Vec3f(p.x(), p.y(), p.z());
		}
	}*/
public:
	vector<Vec3f> boundingBox;
	float radius;
	vector<Vec3f> meshBox;
	map<GameObject*, CollisionData> tier2CollisionData;
	map<GameObject*, CollisionData> tier1CollisionData;
	map<GameObject*, CollisionData> tier0CollisionData;
	vector<Projectile*> collidedProjectiles;
	map<Projectile*, CollisionData> projectileCollisionData; 
	bool drawCollision;
	MyMesh* decimatedMeshPtr;
	bool glowing; 
	void ClearCollisionData(){
		tier1CollisionData.clear();
		tier2CollisionData.clear();
		tier0CollisionData.clear();
		collidedProjectiles.clear();
 		projectileCollisionData.clear(); 
	}

	int objType;
	Vec3f velocity;
	Vec4f angularVelocity;
	Vec3f CenterOfMass;
	//Moment of Inertia
	float mass;
	unsigned int CollisionTierNum;
	float outSideCollisionScale;

	//default constructor
	GameObject::GameObject(const char *n = "\0")
	{
		glowing = false; 
		drawCollision = false;
		radius = 1.f;
		setName(n);		
		isModified = false;
		position = Vector3f(0,0,0);
		scale = 1.f;
		CollisionTierNum = 2;
		outSideCollisionScale = 1.3;
		angularVelocity = Vec4f(1,0,0,0);
		velocity = Vec3f(0,0,0);
		/*TO DO: Initialize lock*/
	}

	//constructor with Object
	GameObject::GameObject(GameObject* object)
	{
		glowing = false;
		drawCollision = object->drawCollision;
		objType = object->objType;
		velocity = object->velocity;
		CollisionTierNum = object->CollisionTierNum;
		setName(object->GetName());  		
		isModified = object->IsModified();
		position = object->GetPosition();
		scale = object->GetScale();
		meshFile = object->GetMeshFile();
		rotation = object->GetRotation();
		meshptr = object->GetMesh();
		objType = object->objType;
		angularVelocity = object->angularVelocity;
		/*TO DO: Initialize lock*/
	}

	//destructor
	virtual GameObject::~GameObject(){}

	//Get a POINTER to the current Object's mesh
	MyMesh* GameObject::GetMesh(){return meshptr;}

	//Redefine the object's mesh
	void GameObject::SetMesh(MyMesh* NewMesh){meshptr = NewMesh;}

	//Getters and Setters//////////////////////////////////
	Vector3f GameObject::GetPosition(){return position;}
	void GameObject::MovePositon(Vector3f &delta){position += delta;}
	void GameObject::MovePositon(float x, float y, float z)
	{
		position[0] = position[0] + x;
		position[1] = position[1] + y;
		position[2] = position[2] + z;
	}
	void GameObject::SetPosition(Vector3f &newPos){position = newPos;}
	Vec4f &GameObject::GetRotation(){return rotation;}
	void GameObject::SetRotation(Vec4f newRotation){rotation = newRotation;}
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

	float GameObject::GetMass() { return mass; };

	//void GameObject::SetMass(float newMass) { mass = newMass; };
	float GameObject::SetMass(float newMass) { mass = newMass; };
	
	bool GameObject::IsInBoundingBox(Vec3f v)
	{
		
		int size = boundingBox.size();
		if (size == 0) return false;
		
		float minX = boundingBox[0][0];
		float maxX = boundingBox[0][0];
		float minY = boundingBox[0][1];
		float maxY = boundingBox[0][1];
		float minZ = boundingBox[0][2];
		float maxZ = boundingBox[0][2];
		
		for (int i = 1; i < size; i++)
		{
			if (boundingBox[i][0] < minX) minX = boundingBox[i][0];
			if (boundingBox[i][0] > maxX) maxX = boundingBox[i][0];
			
			if (boundingBox[i][0] < minY) minY = boundingBox[i][1];
			if (boundingBox[i][0] > maxY) maxY = boundingBox[i][1];
			
			if (boundingBox[i][0] < minZ) minZ = boundingBox[i][2];
			if (boundingBox[i][0] > maxZ) maxZ = boundingBox[i][2];
		}
		
		if (v[0] < minX || v[0] > maxX || v[1] < minY || v[1] > maxY || v[2] < minZ || v[2] > maxZ)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////

	//Bools in case of error when locking...
	bool GameObject::Lock(){/*ToDo: write function for lock*/}
	bool GameObject::Unlock(){/*ToDo:: write function for unlock*/}

	//TO DO: Create linker function to link sound object to object.
	//TO DO: Create a getter/setter for an objects textures
};
#endif _GAMEOBJECT_H_
