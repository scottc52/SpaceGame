#ifndef _COLLISION_DETECTION_H_
#define _COLLISION_DETECTION_H_
#include "GameRoom.h"
#include "GamePlayer.h"
#include "GameCamera.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

void ComputeBoundingBoxesAndPhysicsConstants(vector<GameObject*>& objects);

void UpdateCoords(vector<Vec3f>& v, Vec3f position, Vec3f velocity, Vec4f rotation, Vec4f wvelocity, float dt, float scale = 1.f, bool pers = false, float inter = 1.f, float xScale = 0, float yScale = 0, float zNear = 1, float zFar = 1);

void ResolveOverlappingObjects(GameRoom* room, GamePlayer* player, double dt);

void PerformCollisionDetection(GameRoom* room, GamePlayer* player, double dt, float xScale = 0, float yScale = 0, float zNear = 1, float zFar = 1);


#endif _COLLISION_DETECTION_H_