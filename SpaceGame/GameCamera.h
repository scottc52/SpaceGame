/* 
 * A basic camera object subclass.  Inherits all properties from GameObject
 */
#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_
#include "GameObject.h"

class GameCamera : public GameObject{
private:
	Vec3f upVec;
	Vec3f viewVec;
public:
	GameCamera::GameCamera(){}
	GameCamera::~GameCamera(){}
	Vec3f GameCamera::GetUpVector(){return upVec;}
	void GameCamera::SetUpVector(Vec3f &v){upVec = v;}
	Vec3f GameCamera::GetViewVector(){return viewVec;}
	void GameCamera::SetViewVector(Vec3f &v){viewVec = v;}
};
#endif _GAMECAMERA_H_