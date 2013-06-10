/*
 *  EnemyProjectile.cpp
 *  Metaballs
 *
 *  Created by Jordan Davidson on 5/27/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

/* TEMPORARY PROJECTILE CLASS */

#include "Projectile.h"

EnemyProjectile::EnemyProjectile(Eigen::Vector3f center, Eigen::Vector3f direction, float radius, float speed)
{
	this->center = center;
	this->direction = direction;
	normalize(this->direction);
	this->radius = radius;
	this->speed = speed;
	
	this->collided = false;
	this->active = true;
}

EnemyProjectile::~EnemyProjectile() {};

void EnemyProjectile::move()
{
	if (active)
	{
		Vector3f newCenter;
		newCenter(0) = center(0) + (direction(0) * speed);
		newCenter(1) = center(1) + (direction(1) * speed);
		newCenter(2) = center[2] + (direction(2) * speed);
		
		if (collisionDetected(newCenter))
		{
			collided = true;
			active = false;
		}
		else
		{
			center = newCenter;
			drawSphere(center, radius);
		}
	}
}

Vector3f EnemyProjectile::normalize(Vector3f& v)
{
	float result = 1 / (sqrt((v(0) * v(0)) + (v(1) * v(1)) + (v(2) * v(2))));
	v(0) = v(0) * result;
	v(1) = v(1) * result;
	v(2) = v(2) * result;
	
	return v;
}

void EnemyProjectile::drawSphere(Vector3f center, float radius)
{
	float centerMaterialAmbient[] = {0., .5, 0., 1.0};
	float centerMaterialDiffuse[]  = {0., .5, 0., 1.0};
	float centerMaterialSpecular[] = {0., .5, 0., 1.0};
	float centerShininess = 8.0;
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, centerMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, centerMaterialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, centerMaterialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &centerShininess);
	
	glPushMatrix();
	glTranslatef(center(0), center(1), center(2));
	glutSolidSphere(radius, SLICES, STACKS);
	glPopMatrix();
}

bool EnemyProjectile::collisionDetected(Vector3f v)
{	
	if ((v(0) - radius) < -40.f || (v(0) + radius) > 40.f ||
		(v(1) - radius) < -40.f || (v(1) + radius) > 40.f ||
		(v(2) - radius) < -40.f || (v(2) + radius) > 40.f)
	{
		return true;
	}
	else
	{
		return false;
	}
}