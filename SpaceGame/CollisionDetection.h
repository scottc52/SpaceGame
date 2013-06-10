#ifndef _COLLISION_DETECTION_H_
#define _COLLISION_DETECTION_H_
#include "GameRoom.h"
#include "GamePlayer.h"
#include "GameCamera.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

void ResolveOverlappingObjects(GameRoom* room, GamePlayer* player, double dt);

void PerformCollisionDetection(GameRoom* room, GamePlayer* player, double dt);


#endif _COLLISION_DETECTION_H_