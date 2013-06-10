/*
 *  Projectile.h
 *  Metaballs
 *
 *  Created by Jordan Davidson on 5/27/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

/* TEMPORARY PROJECTILE CLASS */

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
#include <stdlib.h>
using namespace Eigen;


#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>

#ifndef Projectile_h
#define Projectile_h

#define MAX_PROJECTILE_DISTANCE 40
#define STACKS 20
#define SLICES 20

class Projectile
{	
public:
	Projectile(Vector3f center, Vector3f direction, float radius, float speed);
	~Projectile();
	
	Vector3f Projectile::getCenter() { return center; };
	Vector3f Projectile::getDirection() { return direction; };
	float Projectile::getRadius() { return radius; };
	float Projectile::getSpeed() { return speed; };
	
	void move();
	bool hasCollided() { return collided; };
	bool isActive() { return active; };
	
private:	
	Vector3f center; // The center of the Projectile
	Vector3f direction; // The normalized direction of the Projectile
	float radius; // The radius of the Projectile
	float speed; // The speed of the Projectile
	
	bool collided; // Whether the projectile has collided with an object
	bool active; // Whether the projectile is still active
	
	Vector3f normalize(Vector3f& Vector3f);
	void drawSphere(Vector3f center, float radius);
	bool collisionDetected(Vector3f v);
};

#endif