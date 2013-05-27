/*
 * The render functionality for the game.  
 *
 */
#ifndef _RENDER_H_
#define _RENDER_H_
#ifdef _WIN32
#include <gl/glew.h>
#include "gl/GL.h"
#include <GL/glu.h>
#include <gl/freeglut.h>
#else
#ifdef __linux__
#include <gl/glew.h> 
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#else
#include <GL/glew.h>
#include <GLUT/glut.h>
#endif
#endif
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


using namespace std;
using namespace Eigen;

#define SHADERS_POSTHIT_VERTEX_FILE "../SpaceGame/shaders/postHit.v.glsl"
#define SHADERS_POSTHIT_FRAGMENT_FILE "../SpaceGame/shaders/postHit.f.glsl"
#define SHADERS_PASS_VERTEX_FILE "../SpaceGame/shaders/pass.v.glsl"
#define SHADERS_FOG_FRAGMENT_FILE "../SpaceGame/shaders/fog.f.glsl"
#define SHADERS_SUM3_FRAGMENT_FILE "../SpaceGame/shaders/sum3.f.glsl"
#define SHADERS_SUM5_FRAGMENT_FILE "../SpaceGame/shaders/sum5.f.glsl"
#define SHADERS_SUMTEXTURES4_FRAGMENT_FILE "../SpaceGame/shaders/sumTextures4.f.glsl"
class Render{
	public:
	static GameState *gameState;	
	//initializer	
	static void GlutInitialize();
	//exported helpers	
	static void initScene();
	static MyMesh squareMesh();
	//glut callbacks
	static void myKeyboard(unsigned char key, int x, int y);  			
	static void myReshape(int w, int h);
	static void myDisplay();
	static void myIdle();

};
#endif _RENDER_H_
