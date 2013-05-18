/**
* Stream IO Helper functions
* I was on the fence about adding them into the classes
* So I put them here for now. They all conform to the interface
* write<ClassName>(ostream &os, <ClassName> o)
* vectors are passed by value for compatibility with setter/getters
* all other classes are passed as pointers
*/ 

#ifndef _TEXT_IO_HELPERS_H_
#define _TEXT_IO_HELPERS_H_

#include "GameObject.h"
#include "GameLight.h"
#include "GameCamera.h"
#include "GameDoor.h"
#include <iostream>
using namespace std; 
bool writeVec3f(ostream &os, Vec3f pos, bool ifo = false){
	if(ifo) os <<"#Vec3f" << endl <<"#\tx\ty\tz\t" << endl;  	
	os << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\t";
	return true;
}

bool writeVec3f(ostream &os, Vec4f pos, bool ifo = false){
	if(ifo) os << "#Vec4f as Vec3f" << endl <<"#\tx\ty\tz\t" << endl; 
	os << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\t";
	return true;
}

bool writeGameObject(ostream &os, GameObject *o, bool ifo = false){
	if (ifo) os <<"# Game Object" << endl << "#\tname\tfilename\tx\ty\tz\tangle\taxisX\taxisY\taxisZ\tscale" << endl; 
	char *name = o->GetName();
	name = (name)? name : "NULL"; 
	char *mesh_name = o->GetMeshFile();
	mesh_name = (mesh_name)? mesh_name : "NULL";  
	os << name << "\t" << mesh_name << "\t";
	writeVec3f(os, o->GetPosition());
	Quaternionf rotation = o->GetRotation();
	os<<rotation.w<<"\t";
	os<<rotation.x<<"\t";
	os<<rotation.y<<"\t";
	os<<rotation.z<<"\t";
	os << o->GetScale() << "\t";
	return true;
}	

bool writeGameLight(ostream &os, GameLight *l){
	os << l->GetName() << "\t";
	writeVec3f(os, l->GetPosition()); 
	writeVec3f(os, l->GetAmbient());
	writeVec3f(os, l->GetDiffuse());
	writeVec3f(os, l->GetSpecular()); 
	return true; 
}

bool writeGameCamera(ostream &os, GameCamera *c){
	writeVec3f(os, c->GetPosition());
	writeVec3f(os, c->GetViewVector());
	writeVec3f(os, c->GetUpVector()); 	
	return true; 
}

#endif
