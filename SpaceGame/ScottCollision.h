

#ifndef _ScottCollision_H_
#define _ScottCollision_H_

#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include <time.h>
#include <math.h>
#include "Mesh.h"
#include "GameState.h"
#include "Render.h"
#include "Sound.h"

#include "SOIL.h"

#define PI 3.14159265358979323846

//// EigenIncludes
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
#include <pthread.h>

using namespace std;
using namespace Eigen;


class SCollision{
private: 
	static bool paused; 
	static Sound *sound;
public:
	static GameState *gameState;	
	//initializer	
	static void performCollision();
};

#endif _ScottCollision_H_