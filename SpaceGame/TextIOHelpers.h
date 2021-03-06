/**
* Stream IO Helper functions
* I was on the fence about adding them into the classes
* So I put them here for now. They all conform to the interface
* write<ClassName>(ostream &os, <ClassName> o)
* vectors are passed by value for compatibility with setter/getters
* all other classes are passed as pointers
*/ 

#ifndef TEXT_IO_HELPERS_H_
#define TEXT_IO_HELPERS_H_

#include "GameObjectHeaderList.h"
#include <iostream>
#include <string>
using namespace std;

inline string pathCat(const char *root, const char *child){
	return string (root) + string("/") + string(child);  
}

inline bool writeVector3f(ostream &os, Vector3f pos, bool ifo = false){
	if(ifo) os <<"#Vector3f" << endl <<"#\tx\ty\tz\t" << endl;  	
	os << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\t";
	return true;
}

inline bool writeVector3f(ostream &os, Vec4f pos, bool ifo = false){
	if(ifo) os << "#Vector4f as Vector3f" << endl <<"#\tx\ty\tz\t" << endl; 
	os << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\t";
	return true;
}

inline bool writeGameObject(ostream &os, GameObject *o, bool ifo = false){
	if (ifo) os <<"# Game Object" << endl << "#\tname\tfilename\tx\ty\tz\tangle\taxisX\taxisY\taxisZ\tscale" << endl; 
	const char *name = o->GetName();
	name = (name)? name : "NULL"; 
	const char *mesh_name = o->GetMeshFile();
	mesh_name = (mesh_name)? mesh_name : "NULL";  
	os << name << "\t" << mesh_name << "\t";
	writeVector3f(os, o->GetPosition());
	Vec4f rotation = o->GetRotation();
	os<<rotation[0]<<"\t";
	os<<rotation[1]<<"\t";
	os<<rotation[2]<<"\t";
	os<<rotation[3]<<"\t";
	os << o->GetScale() << "\t";
	return true;
}	

inline bool writeGameLight(ostream &os, GameLight *l){
	os << l->GetName() << "\t";
	writeVector3f(os, l->GetPosition()); 
	writeVector3f(os, l->GetAmbient());
	writeVector3f(os, l->GetDiffuse());
	writeVector3f(os, l->GetSpecular()); 
	return true; 
}

inline bool writeGameCamera(ostream &os, GameCamera *c){
	writeVector3f(os, c->GetPosition());
	writeVector3f(os, c->GetViewVector());
	writeVector3f(os, c->GetUpVector()); 	
	return true; 
}

#endif
