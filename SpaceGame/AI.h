/*
 *  AI.h
 *  Metaballs
 *
 *  Created by Jordan Davidson on 5/27/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
using namespace Eigen;

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
#include <GLUT/glut.h>
#endif
#endif

#ifndef AI_H
#define AI_H

class AI
{
public:
	AI::AI() { speed = 0; alive = true; };
	AI::~AI() {};
	
	float AI::getSpeed() { return speed; };
	bool AI::isAlive() { return alive; };
	
	virtual Vector3f AI::getLocation() = 0;
	virtual Vector3f AI::getDirection() = 0;
	
	void update()
	{
		checkForCollision();
		checkToMove();
		checkToChangeOrientation();
		checkToFire();
		checkToUpdate();
	};
	
	virtual void checkForCollision() = 0;
	virtual void checkToMove() = 0;
	virtual void checkToChangeOrientation() = 0;
	virtual void checkToFire() = 0;
	virtual void checkToUpdate() = 0;
	
	virtual void render() = 0;
	
protected:	
	float speed;
	bool alive;
};

#endif