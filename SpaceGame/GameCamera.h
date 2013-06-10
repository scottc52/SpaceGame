/* 
 * A basic camera object subclass.  Inherits all properties from GameObject
 */
#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_
#include "GameObject.h"

class GameCamera : public GameObject{
private:
	Vector3f upVec;
	Vector3f viewVec;
public:
	GameCamera::GameCamera(){objType = CAMERA_TYPE;}
	GameCamera::~GameCamera(){}
	Vector3f GameCamera::GetUpVector(){return upVec;}
	void GameCamera::SetUpVector(Vector3f &v){upVec = v;}
	Vector3f GameCamera::GetViewVector(){return viewVec;}
	void GameCamera::SetViewVector(Vector3f &v){viewVec = v;}
};
#endif _GAMECAMERA_H_