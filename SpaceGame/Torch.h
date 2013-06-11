/**
* Flame Particles System
*/

#ifndef _TORCH_H
#define _TORCH_H

#include <Eigen/Core>

#include <list>
#include <cmath>
#include <iostream>
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

using namespace Eigen; 
using namespace std;

struct JParticle{
	Vector3f vel;
	Vector3f pos; 
	double timeAlive; 
	double life; 
	float color[4]; 
	JParticle(Vector3f &pos, Vector3f &vel, double l, float r=0.7f, float g=0.4f, float b=0.3f, float a=0.3f){
		this->vel = vel;
		this->pos = pos; 
		life = l;
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	}

	bool isDead(){
		return timeAlive >= life; 
	}

	void update(double dt);

	void render();

};

class StaticEmitter{
public: 
	virtual void render() =0 ;
	virtual void update(double dt)=0; 
};

inline bool jPIsDead(JParticle &v){
	return v.isDead();
} 

class Torch : public StaticEmitter{
public:
	list<JParticle> ps;
	Vector3f center;
	Vector3f up; 
	Vector3f norm;
	Vector3f strafe;
	double width;
	double height;   
	double density;
	double base; 
	Torch(Vector3f &c, Vector3f &n, Vector3f &u, double w, double h, double d)
	:StaticEmitter(),center(c),up(u), norm(n), width(w), height(h), density(d) 
	{
		strafe = (up.normalized().cross(norm.normalized()));
		base = 0;
	} 
	
	void render();

	double life(double theta, double u, double v);

	double color(double theta, double u, double v){
		double c = sin(theta) + sin(u) + cos(v);
		return c; 
	} 

	void update(double dt);

};

#endif