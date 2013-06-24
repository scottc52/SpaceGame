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
public:
	static GameState *gameState;	
	//initializer	
	static void performCollision();
};
