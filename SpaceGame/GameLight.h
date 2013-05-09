/* 
 * A basic game light class.  Subclass of GameObject. 
 */

#include "GameObject.h"

class GameLight : public GameObject{
private:
	Vec4f ambient;
	Vec4f diffuse;
	Vec4f specular;
public:
	Vec4f GameLight::GetAmbient(){return ambient;}
	void GameLight::SetAmbient(Vec4f &a){ambient = a;}
	Vec4f GameLight::GetDiffuse(){return diffuse;}
	void GameLight::SetDiffuse(Vec4f &d){diffuse = d;}
	Vec4f GameLight::GetSpecular(){return specular;}
	void GameLight::SetSpecular(Vec4f &s){specular = s;}
};