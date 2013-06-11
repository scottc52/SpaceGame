/*
 *  AI.h
 *  Metaballs
 *
 *  Created by Jordan Davidson on 5/27/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

#include "GameActiveObject.h"
using namespace Eigen;

#ifdef _WIN32
#include <gl/glew.h>
#include "gl/GL.h"
#include <GL/glu.h>
#include <gl/freeglut.h>
#else
#ifdef __linux__
#include <GL/glew.h> 
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#else
#include <GL/glew.h>
#include <GLUT/glut.h>
#endif
#endif

#ifndef AI_H
#define AI_H

const int DEFAULT_AI_HEALTH = 100;

class AI : public GameActiveObject
	{
	public:
		AI::AI() { health =	DEFAULT_AI_HEALTH; alive = true; }
		AI::~AI() {};
		
		int AI::getHealth() { return health; }
		int AI::setHealth(int newHealth) { health = newHealth; }
		int AI::doDamage(int damage)
		{
			health -= damage;
			if (health <= 0) 
			{
				health = 0;
				alive = false;
			}
		}
		bool AI::isAlive() { return alive; }
		float AI::GetSpeed() { return speed; }
		
		virtual Vector3f AI::getLocation() = 0;
		
		virtual void act(GameState *s, double dt )=0; 
		
		virtual void hit(Projectile *p)=0;

		virtual bool intersect(Vector3f &p)=0;

		virtual void render() = 0;
	

	protected:
		int health;
		bool alive;
		float speed;
	};

#endif