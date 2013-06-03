/* 
 * A basic game light class.  Subclass of GameObject. 
 */
#ifndef _GAMELIGHT_H_
#define _GAMELIGHT_H_

#include "GameObject.h"

class GameLight : public GameObject{
private:
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
public:
	Vector4f GameLight::GetAmbient(){return ambient;}
	void GameLight::SetAmbient(Vector4f &a){ambient = a;}
	Vector4f GameLight::GetDiffuse(){return diffuse;}
	void GameLight::SetDiffuse(Vector4f &d){diffuse = d;}
	Vector4f GameLight::GetSpecular(){return specular;}
	void GameLight::SetSpecular(Vector4f &s){specular = s;}
	GameLight::GameLight(){};
};
#endif _GAMELIGHT_H_