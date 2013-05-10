/*
 * The render functionality for the game.  
 *
 */
#ifndef _RENDER_H_
#define _RENDER_H_
#include <gl/glew.h>
#include "gl/GL.h"
#include <GL/glu.h>
#include <gl/freeglut.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include <time.h>
#include <math.h>
#include "Mesh.h"

#define PI 3.14159265

//// EigenIncludes
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

void myReshape(int w, int h);
void initScene();
MyMesh squareMesh();
void myDisplay();
void myIdle();
void RenderGlutInitialize();
#endif _RENDER_H_