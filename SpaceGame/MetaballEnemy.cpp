/*
 *  MetaballEnemy.cpp
 *  Metaballs
 *
 *  Created by Jordan Davidson on 5/3/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

#include "MetaballEnemy.h"
#include <ctime>
#include <cstdio>
#include <cassert>
// Initializes the Blob Creature object
MetaballEnemy::MetaballEnemy(Eigen::Vector3f center, int numBlobs, float radius)
{
	this->numBlobs = numBlobs;
	this->center.x = center(0);
	this->center.y = center(1);
	this->center.z = center(2);
	this->radius = radius;
	
	this->increment = this->radius * 2 / DEFAULT_NUM_CUBES_PER_DIMENSION;
	
	float fieldStrengthConstant = 0.707 / (DEFAULT_FIELD_STRENGTH_CUTOFF * this->radius);
	this->fieldStrengthConstantSquared = fieldStrengthConstant * fieldStrengthConstant;
	this->fieldStrengthConstantSquaredSquared = fieldStrengthConstantSquared * fieldStrengthConstantSquared;
	this->fieldStrengthDistanceSquaredThreshold = 0.5f / fieldStrengthConstantSquared;
	
	srand(time(NULL)); //Initializes random number generator
	
	this->blobs = new Blob[numBlobs];
	
	for (int i = 0; i < numBlobs; i++)
	{
		Blob blob;
		
		Vertex tempCenter;
		tempCenter.x = 0.f;
		tempCenter.y = 0.f;
		tempCenter.z = 0.f;
		blob.center = this->center;
		
		blob.speed = DEFAULT_BLOB_SPEED * this->radius;
		blob.direction = generateRandomNormalizedDirection();
		blob.pastCenter = false;
		
		blobs[i] = blob;
	}
	
	this->maxBlobRadius = getMaxBlobRadius();
	
	//initializeVTable();
	
	this->addedSize2D =  DEFAULT_NUM_CUBES_PER_DIMENSION * DEFAULT_NUM_CUBES_PER_DIMENSION;
	
	this->addedSize3D = addedSize2D * DEFAULT_NUM_CUBES_PER_DIMENSION;
	
	this->added = new bool[addedSize3D];
	
	this->strengthSize2D = (DEFAULT_NUM_CUBES_PER_DIMENSION + 1) * (DEFAULT_NUM_CUBES_PER_DIMENSION + 1);
	
	this->strengthSize3D = strengthSize2D * (DEFAULT_NUM_CUBES_PER_DIMENSION + 1);
	
	this->neighborsSize = DEFAULT_NEIGHBORS_SIZE;
	
	this->fieldStrengths = new FieldStrength[strengthSize3D];
	
	this->neighbors = new Index[neighborsSize];
	
	this->direction = generateRandomNormalizedDirection();
	this->speed = DEFAULT_BLOB_CREATURE_SPEED * this->radius;
	
	this->fireCounter = 0;
	
	float tempBlobMaterialAmbient[] = {0.2, 0.2, 0.6, 1.0};
	float tempBlobMaterialDiffuse[]  = {0.2, 0.2, 0.6, 1.0};
	float tempBlobMaterialSpecular[] = {0.8, 0.8, 0.8, 1.0};
	float tempBlobShininess = 8.0;
	
	float tempCenterMaterialAmbient[] = {.2, 0., 0., 1.0};
	float tempCenterMaterialDiffuse[]  = {.2, 0., 0., 1.0};
	float tempCenterMaterialSpecular[] = {.2, 0., 0., 1.0};
	float tempCenterShininess = 8.0;
	
	memcpy(blobMaterialAmbient, tempBlobMaterialAmbient, 4 * sizeof(float));
	memcpy(blobMaterialDiffuse, tempBlobMaterialDiffuse, 4 * sizeof(float));
	memcpy(blobMaterialSpecular, tempBlobMaterialSpecular, 4 * sizeof(float));
	blobShininess = tempBlobShininess;
	
	memcpy(centerMaterialAmbient, tempCenterMaterialAmbient, 4 * sizeof(float));
	memcpy(centerMaterialDiffuse, tempCenterMaterialDiffuse, 4 * sizeof(float));
	memcpy(centerMaterialSpecular, tempCenterMaterialSpecular, 4 * sizeof(float));
	centerShininess = tempCenterShininess;
}

MetaballEnemy::~MetaballEnemy()
{
	delete[] blobs;
	delete[] added;
	delete[] fieldStrengths;	
	delete[] neighbors;
}

Blob MetaballEnemy::getBlob(int index)
{
	if (index > 0 && index < numBlobs)
	{
		return blobs[index];
	}
	
	printf("Error: Blob index out of bounds.");
	
	Blob blob;
	return blob;
}

// Sets the speed of all blobs in the Blob Creature
void MetaballEnemy::setAllBlobsSpeed(float newSpeed)
{
	for (int i = 0; i < numBlobs; i++)
	{
		blobs[i].speed = newSpeed * this->radius;
	}
}

void MetaballEnemy::moveMetaballEnemy()
{
	updateActionState();
	
	if (actionState == DEFAULT_ACTION_STATE)
	{
		this->center.x = this->center.x + (this->direction.x * this->speed);
		this->center.y = this->center.y + (this->direction.y * this->speed);
		this->center.z = this->center.z + (this->direction.z * this->speed);
	}
	else if (actionState == PLAYER_DETECTED_ACTION_STATE)
	{
		//Vector3f playerPos = getPlayerPosition();
		Vector3f playerPos = Vector3f(0, 0, 0); // CHANGE LATER
		Vertex newDirection;
		newDirection.x = playerPos(0) - this->center.x;
		newDirection.y = playerPos(1) - this->center.y;
		newDirection.z = playerPos(2) - this->center.z;
		normalize(newDirection);
		this->direction = newDirection;
	}
	else if (actionState == SOUND_DETECTED_ACTION_STATE)
	{
		//Move Toward Sound TODO
	}
}

void MetaballEnemy::updateActionState()
{
	if (false /*playerIsVisible()*/) // CHANGE LATER
	{
		this->actionState = PLAYER_DETECTED_ACTION_STATE;
	} else {
		this->actionState = DEFAULT_ACTION_STATE;
	}
}

void MetaballEnemy::moveProjectiles()
{
	int size = projectiles.size();
	for (int i = 0; i < size; i++)
	{
		Projectile curProjectile = projectiles[i];
		if (curProjectile.hasCollided())
		{
			projectiles.erase(projectiles.begin() + i);
			i--;
			size--;
		}
		else
		{
			curProjectile.move();
			projectiles[i] = curProjectile;
		}
	}
}

bool MetaballEnemy::collisionDetected(Vertex v)
{	
	if ((v.x - radius) < (-BLOB_CREATURE_MOVE_BOUNDARY * this->radius) ||
		(v.x + radius) > (BLOB_CREATURE_MOVE_BOUNDARY * this->radius) ||
		(v.y - radius) < (-BLOB_CREATURE_MOVE_BOUNDARY * this->radius) ||
		(v.y + radius) > (BLOB_CREATURE_MOVE_BOUNDARY * this->radius) ||
		(v.z - radius) < (-BLOB_CREATURE_MOVE_BOUNDARY * this->radius) ||
		(v.z + radius) > (BLOB_CREATURE_MOVE_BOUNDARY * this->radius))
	{
		return true;
	}
	else
	{
		return false;
	}
}

Vector3f MetaballEnemy::getLocation()
{
	Vector3f location(center.x, center.y, center.z);
	return location;
}

Vector3f MetaballEnemy::getDirection()
{
	Vector3f dir(direction.x, direction.y, direction.z);
	return dir;
}

void MetaballEnemy::checkForCollision()
{
	if (collisionDetected(this->center))
	{
		this->direction = generateRandomNormalizedDirection();
		
		Vertex nextCenter;
		nextCenter.x = this->center.x + this->direction.x * this->speed;
		nextCenter.y = this->center.y + this->direction.y * this->speed;
		nextCenter.z = this->center.z + this->direction.z * this->speed;
		
		while (collisionDetected(nextCenter))
		{
			this->direction = generateRandomNormalizedDirection();
			
			nextCenter.x = this->center.x + this->direction.x * this->speed;
			nextCenter.y = this->center.y + this->direction.y * this->speed;
			nextCenter.z = this->center.z + this->direction.z * this->speed;
		}
	}
}

void MetaballEnemy::checkToMove()
{
	if (MOVING_ENABLED) moveMetaballEnemy();
}

void MetaballEnemy::checkToChangeOrientation()
{
	
}

void MetaballEnemy::checkToFire()
{
	if (fireCounter >= FIRE_COUNTER_THRESHOLD)
	{
		//Vector3f playerPos = getPlayerPosition();
		Vector3f playerPos = Vector3f(0, 0, 0); // CHANGE LATER
		Vector3f projectileCenter;
		projectileCenter(0) = this->center.x;
		projectileCenter(1) = this->center.y;
		projectileCenter(2) = this->center.z;
		Vector3f projectileDirection = playerPos - projectileCenter;
		projectileDirection.normalize();
		
		Projectile newProjectile(projectileCenter, projectileDirection, PROJECTILE_RADIUS,
								 PROJECTILE_SPEED);
		projectiles.push_back(newProjectile);
		
		fireCounter = 0;
	}
	else if (fireCounter < FIRE_COUNTER_THRESHOLD)
	{
		fireCounter++;
	}
	
	moveProjectiles();
}

// NOTE: Assumes (2 * maxBlobRadius) < (maxRadius - MIN_DISTANCE_FROM_CENTER)
void MetaballEnemy::checkToUpdate()
{	
	for (int i = 0; i < numBlobs; i++)
	{
		Blob curBlob = blobs[i];
		
		Vertex direction = curBlob.direction;
		
		float nextX = curBlob.center.x + direction.x * curBlob.speed;
		float nextY = curBlob.center.y + direction.y * curBlob.speed;
		float nextZ = curBlob.center.z + direction.z * curBlob.speed;
		
		float distance = sqrt(pow(nextX, 2) + pow(nextY, 2) + pow(nextZ, 2));
		float maxDistance = distance + this->maxBlobRadius;
		float minDistance = distance - this->maxBlobRadius;
		
		while (maxDistance > this->radius)
		{
			direction = generateRandomNormalizedDirection();
			
			nextX = curBlob.center.x + direction.x * curBlob.speed;
			nextY = curBlob.center.y + direction.y * curBlob.speed;
			nextZ = curBlob.center.z + direction.z * curBlob.speed;
			
			maxDistance = sqrt(pow(nextX, 2) + pow(nextY, 2) + pow(nextZ, 2)) + this->maxBlobRadius;
		}
		
		while (curBlob.pastCenter && (minDistance < (MIN_DISTANCE_FROM_CENTER * this->radius)))
		{
			direction = generateRandomNormalizedDirection();
			
			nextX = curBlob.center.x + direction.x * curBlob.speed;
			nextY = curBlob.center.y + direction.y * curBlob.speed;
			nextZ = curBlob.center.z + direction.z * curBlob.speed;
			
			minDistance = sqrt(pow(nextX, 2) + pow(nextY, 2) + pow(nextZ, 2)) - this->maxBlobRadius;
		}
		
		if (minDistance >= (MIN_DISTANCE_FROM_CENTER * this->radius)) curBlob.pastCenter = true;
		
		curBlob.direction.x = direction.x;
		curBlob.direction.y = direction.y;
		curBlob.direction.z = direction.z;
		
		curBlob.center.x = nextX;
		curBlob.center.y = nextY;
		curBlob.center.z = nextZ;
		
		blobs[i] = curBlob;
	}
}

void MetaballEnemy::render()
{	
	glColor4f(0.5, 0.5, 0.5, 1.0);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, centerMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, centerMaterialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, centerMaterialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &centerShininess);
	
	glPushMatrix();
	glTranslatef(center.x, center.y, center.z);
	glutSolidSphere(CENTER_SPHERE_RADIUS * this->radius, CENTER_SPHERE_SLICES, CENTER_SPHERE_STACKS);
	glPopMatrix();
	
	glColor4f(0.5, 0.5, 0.5, 1.0);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, blobMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blobMaterialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, blobMaterialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &blobShininess);
	
	glBegin(GL_TRIANGLES);
	
	memset(added, 0, addedSize3D * sizeof(added[0]));
	
	int curNeighborIndex = 0;
	int endNeighborIndex = 0;
	
	memset(fieldStrengths, 0, strengthSize3D * sizeof(fieldStrengths[0]));
	
	for (int i = 0; i < numBlobs; i++) {
		Blob curBlob = blobs[i];
	
		Index blobCubePosition = getCubePosition(curBlob.center);
		
		bool isComputed = false;
		bool found = false;
		
		//while (!found)
		for (;blobCubePosition.x < DEFAULT_NUM_CUBES_PER_DIMENSION; blobCubePosition.x ++){
			if (wasAdded(blobCubePosition))
			{
				isComputed = true;
				found = true;
				break;
			}
			if (renderCube(blobCubePosition))
			{
				found = true;
				break;
			}
		}
		
		assert(found);

		if (!isComputed)
		{
			
			addNeighbors(blobCubePosition, endNeighborIndex);
			
			while (curNeighborIndex < endNeighborIndex)
			{
				if (renderCube(neighbors[curNeighborIndex]))
				{
					addNeighbors(neighbors[curNeighborIndex], endNeighborIndex);
				}
				
				curNeighborIndex++;
			}
		}
	}
	
	glEnd();
}

Vertex MetaballEnemy::normalize(Vertex& vertex)
{
	float result = 1 / (sqrt((vertex.x * vertex.x) + (vertex.y * vertex.y) + (vertex.z * vertex.z)));
	vertex.x = vertex.x * result;
	vertex.y = vertex.y * result;
	vertex.z = vertex.z * result;
	
	return vertex;
}


Vertex MetaballEnemy::generateRandomNormalizedDirection()
{
	Vertex tempDirection;
	tempDirection.x = (rand() - (RAND_MAX / 2.f));
	tempDirection.y = (rand() - (RAND_MAX / 2.f));
	tempDirection.z = (rand() - (RAND_MAX / 2.f));
	
	return normalize(tempDirection);
}

//NOTE: Assumes all blobs have the same "mass" (field strengths are equal at the same distance from the center)
float MetaballEnemy::getMaxBlobRadius()
{
	
	return sqrt((fieldStrengthConstantSquared -
				 sqrt(fieldStrengthConstantSquaredSquared -
				 4 * fieldStrengthConstantSquaredSquared * (0.25 - DEFAULT_THRESHOLD / numBlobs))) /
				(2 * fieldStrengthConstantSquaredSquared));
}

float MetaballEnemy::calculateFieldStrength(VertexWithFieldStrength v)
{
	float fieldStrength = 0;
	
	for (int i = 0; i < numBlobs; i++)
	{
		Vertex center = blobs[i].center;
		
		float distanceSquared = (center.x - v.x) * (center.x - v.x) +
								(center.y - v.y) * (center.y - v.y) +
								(center.z - v.z) * (center.z - v.z);
		
		

		if (distanceSquared < fieldStrengthDistanceSquaredThreshold)
		{
			fieldStrength += (fieldStrengthConstantSquaredSquared * distanceSquared * distanceSquared) -
							 (fieldStrengthConstantSquared * distanceSquared) + 0.25f;
		}
	}
	
	return fieldStrength;
}

float MetaballEnemy::getFieldStrength(int x, int y, int z)
{
	return fieldStrengths[x * strengthSize2D + y * DEFAULT_NUM_CUBES_PER_DIMENSION + z].strength;
}

void MetaballEnemy::setFieldStrength(float strength, int x, int y, int z)
{
	fieldStrengths[x * strengthSize2D + y * DEFAULT_NUM_CUBES_PER_DIMENSION + z].strength = strength;
	fieldStrengths[x * strengthSize2D + y * DEFAULT_NUM_CUBES_PER_DIMENSION + z].computed = true;
}

bool MetaballEnemy::fieldStrengthWasComputed(int x, int y, int z)
{
	return fieldStrengths[x * strengthSize2D + y * DEFAULT_NUM_CUBES_PER_DIMENSION + z].computed;
}

Vertex MetaballEnemy::getNormal(Vertex v)
{
	Vertex normal;
	normal.x = 0.f;
	normal.y = 0.f;
	normal.z = 0.f;
	
	for (int i = 0; i < numBlobs; i++)
	{
		float xDif = blobs[i].center.x - v.x;
		float yDif = blobs[i].center.y - v.y;
		float zDif = blobs[i].center.z - v.z;
		
		float distanceSquared = (xDif * xDif) + (yDif * yDif) + (zDif * zDif);
		
		if ((fieldStrengthConstantSquared * distanceSquared) < 0.5f)
		{
			float result = 4 * fieldStrengthConstantSquared *
						 (fieldStrengthConstantSquared * distanceSquared - 0.5f);
			
			normal.x += xDif * result;
			normal.y += yDif * result;
			normal.z += zDif * result;
		}
	}
	
	return normalize(normal);
}
#include <iostream>
Index MetaballEnemy::getCubePosition(Vertex v)
{
	Index position;
	
	position.x = floor((v.x + radius) * DEFAULT_NUM_CUBES_PER_DIMENSION / (radius * 2));
	position.y = floor((v.y + radius) * DEFAULT_NUM_CUBES_PER_DIMENSION / (radius * 2));
	position.z = floor((v.z + radius) * DEFAULT_NUM_CUBES_PER_DIMENSION / (radius * 2));
	
	return position;
}

bool MetaballEnemy::wasAdded(Index curPosition)
{
	return added[curPosition.x * addedSize2D + curPosition.y * DEFAULT_NUM_CUBES_PER_DIMENSION + curPosition.z];
}

void MetaballEnemy::addNeighbors(Index curPosition, int& endNeighborIndex)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			for (int k = -1; k <= 1; k++)
			{
				if ((i != 0 || j != 0 || k != 0) &&
					curPosition.x + i >= 0 && curPosition.x + i < DEFAULT_NUM_CUBES_PER_DIMENSION &&
					curPosition.y + j >= 0 && curPosition.y + j < DEFAULT_NUM_CUBES_PER_DIMENSION &&
					curPosition.z + k >= 0 && curPosition.z + k < DEFAULT_NUM_CUBES_PER_DIMENSION)
				{
					Index neighborPosition;
					neighborPosition.x = curPosition.x + i;
					neighborPosition.y = curPosition.y + j;
					neighborPosition.z = curPosition.z + k;
					
					if (!wasAdded(neighborPosition))
					{
						if (endNeighborIndex >= neighborsSize)
						{
							Index *tempNeighbors = new Index[neighborsSize * NEIGHBORS_INCREASE_FACTOR];
							memcpy(tempNeighbors, neighbors, neighborsSize * sizeof(neighbors[0]));
							delete[] neighbors;
							neighbors = tempNeighbors;
							neighborsSize *= NEIGHBORS_INCREASE_FACTOR;
						}
						neighbors[endNeighborIndex] = neighborPosition;
						endNeighborIndex++;
						added[neighborPosition.x * addedSize2D +
						neighborPosition.y * DEFAULT_NUM_CUBES_PER_DIMENSION + neighborPosition.z] = true;
					}
				}
			}
		}
	}
}

bool MetaballEnemy::renderCube(Index index)
{	
	int x = index.x;
	int y = index.y;
	int z = index.z;
	
	VertexWithFieldStrength v1;
	v1.x = -radius + increment * x;
	v1.y = -radius + increment * y;
	v1.z = -radius + increment * z;
	
	VertexWithFieldStrength v2;
	v2.x = -radius + increment * (x + 1);
	v2.y = -radius + increment * y;
	v2.z = -radius + increment * z;
	
	VertexWithFieldStrength v3;
	v3.x = -radius + increment * x;
	v3.y = -radius + increment * (y + 1);
	v3.z = -radius + increment * z;
	
	VertexWithFieldStrength v4;
	v4.x = -radius + increment * (x + 1);
	v4.y = -radius + increment * (y + 1);
	v4.z = -radius + increment * z;
	
	VertexWithFieldStrength v5;
	v5.x = -radius + increment * x;
	v5.y = -radius + increment * y;
	v5.z = -radius + increment * (z + 1);
	
	VertexWithFieldStrength v6;
	v6.x = -radius + increment * (x + 1);
	v6.y = -radius + increment * y;
	v6.z = -radius + increment * (z + 1);
	
	VertexWithFieldStrength v7;
	v7.x = -radius + increment * x;
	v7.y = -radius + increment * (y + 1);
	v7.z = -radius + increment * (z + 1);
	
	VertexWithFieldStrength v8;
	v8.x = -radius + increment * (x + 1);
	v8.y = -radius + increment * (y + 1);
	v8.z = -radius + increment * (z + 1);
	
	if (!fieldStrengthWasComputed(x, y, z))
	{
		v1.strength = calculateFieldStrength(v1);
		setFieldStrength(v1.strength, x, y, z);
	}
	else
	{
		v1.strength = getFieldStrength(x, y, z);
	}
	
	if (!fieldStrengthWasComputed(x + 1, y, z))
	{
		v2.strength = calculateFieldStrength(v2);
		setFieldStrength(v2.strength, x + 1, y, z);
	}
	else
	{
		v2.strength = getFieldStrength(x + 1, y, z);
	}
	
	if (!fieldStrengthWasComputed(x, y + 1, z))
	{
		v3.strength = calculateFieldStrength(v3);
		setFieldStrength(v3.strength, x, y + 1, z);
	}
	else
	{
		v3.strength = getFieldStrength(x, y + 1, z);
	}
	
	if (!fieldStrengthWasComputed(x + 1, y + 1, z))
	{
		v4.strength = calculateFieldStrength(v4);
		setFieldStrength(v4.strength, x + 1, y + 1, z);
	}
	else
	{
		v4.strength = getFieldStrength(x + 1, y + 1, z);
	}
	
	if (!fieldStrengthWasComputed(x, y, z + 1))
	{
		v5.strength = calculateFieldStrength(v5);
		setFieldStrength(v5.strength, x, y, z + 1);
	}
	else
	{
		v5.strength = getFieldStrength(x, y, z + 1);
	}
	
	if (!fieldStrengthWasComputed(x + 1, y, z + 1))
	{
		v6.strength = calculateFieldStrength(v6);
		setFieldStrength(v6.strength, x + 1, y, z + 1);
	}
	else
	{
		v6.strength = getFieldStrength(x + 1, y, z + 1);
	}
	
	if (!fieldStrengthWasComputed(x, y + 1, z + 1))
	{
		v7.strength = calculateFieldStrength(v7);
		setFieldStrength(v7.strength, x, y + 1, z + 1);
	}
	else
	{
		v7.strength = getFieldStrength(x, y + 1, z + 1);
	}
	
	if (!fieldStrengthWasComputed(x + 1, y + 1, z + 1))
	{
		v8.strength = calculateFieldStrength(v8);
		setFieldStrength(v8.strength, x + 1, y + 1, z + 1);
	}
	else
	{
		v8.strength = getFieldStrength(x + 1, y + 1, z + 1);
	}
	
	Normal *normals = new Normal[VERTICES_IN_CUBE * VERTICES_IN_CUBE]();
	
	bool rendered = renderTetrahedronIntersections(v1, v2, v4, v5, 1, 2, 4, 5, normals);
	if (renderTetrahedronIntersections(v1, v3, v4, v5, 1, 3, 4, 5, normals)) rendered = true;
	if (renderTetrahedronIntersections(v2, v4, v5, v6, 2, 4, 5, 6, normals)) rendered = true;
	if (renderTetrahedronIntersections(v3, v4, v5, v7, 3, 4, 5, 7, normals)) rendered = true;
	if (renderTetrahedronIntersections(v4, v5, v6, v8, 4, 5, 6, 8, normals)) rendered = true;
	if (renderTetrahedronIntersections(v4, v5, v7, v8, 4, 5, 7, 8, normals)) rendered = true;
	
	delete[] normals;
	
	return rendered;
}

bool MetaballEnemy::renderTetrahedronIntersections(VertexWithFieldStrength v1, VertexWithFieldStrength v2,
												  VertexWithFieldStrength v3, VertexWithFieldStrength v4,
												  int v1Index, int v2Index, int v3Index, int v4Index,
												  Normal *normals)
{	
	int numInsideField = 0;
	
	bool v1InsideField = false;
	bool v2InsideField = false;
	bool v3InsideField = false;
	bool v4InsideField = false;
	
	if (v1.strength > DEFAULT_THRESHOLD)
	{
		numInsideField++;
		v1InsideField = true;
	}
	if (v2.strength > DEFAULT_THRESHOLD)
	{
		numInsideField++;
		v2InsideField = true;
	}
	if (v3.strength > DEFAULT_THRESHOLD)
	{
		numInsideField++;
		v3InsideField = true;
	}
	if (v4.strength > DEFAULT_THRESHOLD)
	{
		numInsideField++;
		v4InsideField = true;
	}
	
	VertexWithFieldStrength vInside1;
	VertexWithFieldStrength vInside2;
	VertexWithFieldStrength vInside3;
	
	VertexWithFieldStrength vOutside1;
	VertexWithFieldStrength vOutside2;
	VertexWithFieldStrength vOutside3;
	
	float vOutside1Ratio = .5f;
	float vOutside2Ratio = .5f;
	float vOutside3Ratio = .5f;
	
	float vOutside11Ratio = .5f;
	float vOutside12Ratio = .5f;
	float vOutside21Ratio = .5f;
	float vOutside22Ratio = .5f;
	
	float vInside1Ratio = .5f;
	float vInside2Ratio = .5f;
	float vInside3Ratio = .5f;
	
	int vInside1Index;
	int vInside2Index;
	int vInside3Index;
	
	int vOutside1Index;
	int vOutside2Index;
	int vOutside3Index;
	
	Vertex renderV1;
	Vertex renderV2;
	Vertex renderV3;
	
	Vertex renderN1;
	Vertex renderN2;
	Vertex renderN3;
	
	switch (numInsideField)
	{
		case 1:
			if (v1InsideField)
			{
				vInside1 = v1;
				vOutside1 = v2;
				vOutside2 = v3;
				vOutside3 = v4;
				
				vInside1Index = v1Index;
				vOutside1Index = v2Index;
				vOutside2Index = v3Index;
				vOutside3Index = v4Index;
			} else if (v2InsideField)
			{
				vInside1 = v2;
				vOutside1 = v1;
				vOutside2 = v3;
				vOutside3 = v4;
				
				vInside1Index = v2Index;
				vOutside1Index = v1Index;
				vOutside2Index = v3Index;
				vOutside3Index = v4Index;
			} else if (v3InsideField)
			{
				vInside1 = v3;
				vOutside1 = v1;
				vOutside2 = v2;
				vOutside3 = v4;
				
				vInside1Index = v3Index;
				vOutside1Index = v1Index;
				vOutside2Index = v2Index;
				vOutside3Index = v4Index;
			} else if (v4InsideField)
			{
				vInside1 = v4;
				vOutside1 = v1;
				vOutside2 = v2;
				vOutside3 = v3;
				
				vInside1Index = v4Index;
				vOutside1Index = v1Index;
				vOutside2Index = v2Index;
				vOutside3Index = v3Index;
			}
			
			if (VERTEX_INTERPOLATION)
			{
				vOutside1Ratio = (vInside1.strength - DEFAULT_THRESHOLD) /
								 (vInside1.strength - vOutside1.strength);
				vOutside2Ratio = (vInside1.strength - DEFAULT_THRESHOLD) /
								 (vInside1.strength - vOutside2.strength);
				vOutside3Ratio = (vInside1.strength - DEFAULT_THRESHOLD) /
								 (vInside1.strength - vOutside3.strength);
			}
			
			renderV1.x = (vOutside1.x - vInside1.x) * vOutside1Ratio + vInside1.x;
			renderV1.y = (vOutside1.y - vInside1.y) * vOutside1Ratio + vInside1.y;
			renderV1.z = (vOutside1.z - vInside1.z) * vOutside1Ratio + vInside1.z;
			
			renderV2.x = (vOutside2.x - vInside1.x) * vOutside2Ratio + vInside1.x;
			renderV2.y = (vOutside2.y - vInside1.y) * vOutside2Ratio + vInside1.y;
			renderV2.z = (vOutside2.z - vInside1.z) * vOutside2Ratio + vInside1.z;
			
			renderV3.x = (vOutside3.x - vInside1.x) * vOutside3Ratio + vInside1.x;
			renderV3.y = (vOutside3.y - vInside1.y) * vOutside3Ratio + vInside1.y;
			renderV3.z = (vOutside3.z - vInside1.z) * vOutside3Ratio + vInside1.z;
			
			if (!normalWasComputed(normals, vInside1Index, vOutside1Index))
			{
				renderN1 = getNormal(renderV1);
				setNormal(normals, renderN1, vInside1Index, vOutside1Index);
			}
			else
			{
				renderN1 = getNormal(normals, vInside1Index, vOutside1Index);
			}
			
			if (!normalWasComputed(normals, vInside1Index, vOutside2Index))
			{
				renderN2 = getNormal(renderV2);
				setNormal(normals, renderN2, vInside1Index, vOutside2Index);
			}
			else
			{
				renderN2 = getNormal(normals, vInside1Index, vOutside2Index);
			}
			
			if (!normalWasComputed(normals, vInside1Index, vOutside3Index))
			{
				renderN3 = getNormal(renderV3);
				setNormal(normals, renderN3, vInside1Index, vOutside3Index);
			}
			else
			{
				renderN3 = getNormal(normals, vInside1Index, vOutside3Index);
			}
			
			/*renderN1 = getNormal(renderV1);
			renderN2 = getNormal(renderV2);
			renderN3 = getNormal(renderV3);*/
			
			glNormal3f(renderN1.x, renderN1.y, renderN1.z);
			//glNormal3f(renderV1.x - blobCenter.x, renderV1.y - blobCenter.y, renderV1.z - blobCenter.z);
			glVertex3f(renderV1.x + center.x, renderV1.y + center.y, renderV1.z + center.z);
			
			glNormal3f(renderN2.x, renderN2.y, renderN2.z);
			//glNormal3f(renderV2.x - blobCenter.x, renderV2.y - blobCenter.y, renderV2.z - blobCenter.z);
			glVertex3f(renderV2.x + center.x, renderV2.y + center.y, renderV2.z + center.z);
			
			glNormal3f(renderN3.x, renderN3.y, renderN3.z);
			//glNormal3f(renderV3.x - blobCenter.x, renderV3.y - blobCenter.y, renderV3.z - blobCenter.z);
			glVertex3f(renderV3.x + center.x, renderV3.y + center.y, renderV3.z + center.z);
			
			return true;

			break;
			
		case 2:
			if (v1InsideField && v2InsideField)
			{
				vInside1 = v1;
				vInside2 = v2;
				vOutside1 = v3;
				vOutside2 = v4;
				
				vInside1Index = v1Index;
				vInside2Index = v2Index;
				vOutside1Index = v3Index;
				vOutside2Index = v4Index;
			} else if (v1InsideField && v3InsideField)
			{
				vInside1 = v1;
				vInside2 = v3;
				vOutside1 = v2;
				vOutside2 = v4;
				
				vInside1Index = v1Index;
				vInside2Index = v3Index;
				vOutside1Index = v2Index;
				vOutside2Index = v4Index;
			} else if (v1InsideField && v4InsideField)
			{
				vInside1 = v1;
				vInside2 = v4;
				vOutside1 = v2;
				vOutside2 = v3;
				
				vInside1Index = v1Index;
				vInside2Index = v4Index;
				vOutside1Index = v2Index;
				vOutside2Index = v3Index;
			} else if (v2InsideField && v3InsideField)
			{
				vInside1 = v2;
				vInside2 = v3;
				vOutside1 = v1;
				vOutside2 = v4;
				
				vInside1Index = v2Index;
				vInside2Index = v3Index;
				vOutside1Index = v1Index;
				vOutside2Index = v4Index;
			} else if (v2InsideField && v4InsideField)
			{
				vInside1 = v2;
				vInside2 = v4;
				vOutside1 = v1;
				vOutside2 = v3;
				
				vInside1Index = v2Index;
				vInside2Index = v4Index;
				vOutside1Index = v1Index;
				vOutside2Index = v3Index;
			} else
			{
				vInside1 = v3;
				vInside2 = v4;
				vOutside1 = v1;
				vOutside2 = v2;
				
				vInside1Index = v3Index;
				vInside2Index = v4Index;
				vOutside1Index = v1Index;
				vOutside2Index = v2Index;
			}
			
			if (VERTEX_INTERPOLATION)
			{
				vOutside11Ratio = (vInside1.strength - DEFAULT_THRESHOLD) /
									(vInside1.strength - vOutside1.strength);
				vOutside12Ratio = (vInside2.strength - DEFAULT_THRESHOLD) /
									(vInside2.strength - vOutside1.strength);
				vOutside21Ratio = (vInside1.strength - DEFAULT_THRESHOLD) /
									(vInside1.strength - vOutside2.strength);
				vOutside22Ratio = (vInside2.strength - DEFAULT_THRESHOLD) /
									(vInside2.strength - vOutside2.strength);
			}
			
			renderV1.x = (vOutside1.x - vInside1.x) * vOutside11Ratio + vInside1.x;
			renderV1.y = (vOutside1.y - vInside1.y) * vOutside11Ratio + vInside1.y;
			renderV1.z = (vOutside1.z - vInside1.z) * vOutside11Ratio + vInside1.z;
			
			renderV2.x = (vOutside2.x - vInside1.x) * vOutside21Ratio + vInside1.x;
			renderV2.y = (vOutside2.y - vInside1.y) * vOutside21Ratio + vInside1.y;
			renderV2.z = (vOutside2.z - vInside1.z) * vOutside21Ratio + vInside1.z;
			
			renderV3.x = (vOutside1.x - vInside2.x) * vOutside12Ratio + vInside2.x;
			renderV3.y = (vOutside1.y - vInside2.y) * vOutside12Ratio + vInside2.y;
			renderV3.z = (vOutside1.z - vInside2.z) * vOutside12Ratio + vInside2.z;
			
			if (!normalWasComputed(normals, vInside1Index, vOutside1Index))
			{
				renderN1 = getNormal(renderV1);
				setNormal(normals, renderN1, vInside1Index, vOutside1Index);
			}
			else
			{
				renderN1 = getNormal(normals, vInside1Index, vOutside1Index);
			}
			
			if (!normalWasComputed(normals, vInside1Index, vOutside2Index))
			{
				renderN2 = getNormal(renderV2);
				setNormal(normals, renderN2, vInside1Index, vOutside2Index);
			}
			else
			{
				renderN2 = getNormal(normals, vInside1Index, vOutside2Index);
			}
			
			if (!normalWasComputed(normals, vInside2Index, vOutside1Index))
			{
				renderN3 = getNormal(renderV3);
				setNormal(normals, renderN3, vInside2Index, vOutside1Index);
			}
			else
			{
				renderN3 = getNormal(normals, vInside2Index, vOutside1Index);
			}
			
			/*renderN1 = getNormal(renderV1);
			renderN2 = getNormal(renderV2);
			renderN3 = getNormal(renderV3);*/
			
			glNormal3f(renderN1.x, renderN1.y, renderN1.z);
			//glNormal3f(renderV1.x - blobCenter.x, renderV1.y - blobCenter.y, renderV1.z - blobCenter.z);
			glVertex3f(renderV1.x + center.x, renderV1.y + center.y, renderV1.z + center.z);
			
			glNormal3f(renderN2.x, renderN2.y, renderN2.z);
			//glNormal3f(renderV2.x - blobCenter.x, renderV2.y - blobCenter.y, renderV2.z - blobCenter.z);
			glVertex3f(renderV2.x + center.x, renderV2.y + center.y, renderV2.z + center.z);
			
			glNormal3f(renderN3.x, renderN3.y, renderN3.z);
			//glNormal3f(renderV3.x - blobCenter.x, renderV3.y - blobCenter.y, renderV3.z - blobCenter.z);
			glVertex3f(renderV3.x + center.x, renderV3.y + center.y, renderV3.z + center.z);
			
			renderV1 = renderV3; //renderV1 is the previous renderV3
			
			//renderV2 is the same as before
			
			renderV3.x = (vOutside2.x - vInside2.x) * vOutside22Ratio + vInside2.x;
			renderV3.y = (vOutside2.y - vInside2.y) * vOutside22Ratio + vInside2.y;
			renderV3.z = (vOutside2.z - vInside2.z) * vOutside22Ratio + vInside2.z;
			
			renderN1 = renderN3; //renderN1 is the previous renderN3
			//renderN2 is the same as before
			
			if (!normalWasComputed(normals, vInside2Index, vOutside2Index))
			{
				renderN3 = getNormal(renderV3);
				setNormal(normals, renderN3, vInside2Index, vOutside2Index);
			}
			else
			{
				renderN3 = getNormal(normals, vInside2Index, vOutside2Index);
			}
			
			//renderN3 = getNormal(renderV3);
			
			glNormal3f(renderN1.x, renderN1.y, renderN1.z);
			//glNormal3f(renderV1.x - blobCenter.x, renderV1.y - blobCenter.y, renderV1.z - blobCenter.z);
			glVertex3f(renderV1.x + center.x, renderV1.y + center.y, renderV1.z + center.z);
			
			glNormal3f(renderN2.x, renderN2.y, renderN2.z);
			//glNormal3f(renderV2.x - blobCenter.x, renderV2.y - blobCenter.y, renderV2.z - blobCenter.z);
			glVertex3f(renderV2.x + center.x, renderV2.y + center.y, renderV2.z + center.z);
			
			glNormal3f(renderN3.x, renderN3.y, renderN3.z);
			//glNormal3f(renderV3.x - blobCenter.x, renderV3.y - blobCenter.y, renderV3.z - blobCenter.z);
			glVertex3f(renderV3.x + center.x, renderV3.y + center.y, renderV3.z + center.z);
			
			return true;
			
			break;
			
		case 3:
			if (!v1InsideField)
			{
				vOutside1 = v1;
				vInside1 = v2;
				vInside2 = v3;
				vInside3 = v4;
				
				vOutside1Index = v1Index;
				vInside1Index = v2Index;
				vInside2Index = v3Index;
				vInside3Index = v4Index;
			} else if (!v2InsideField)
			{
				vOutside1 = v2;
				vInside1 = v1;
				vInside2 = v3;
				vInside3 = v4;
				
				vOutside1Index = v2Index;
				vInside1Index = v1Index;
				vInside2Index = v3Index;
				vInside3Index = v4Index;
			} else if (!v3InsideField)
			{
				vOutside1 = v3;
				vInside1 = v1;
				vInside2 = v2;
				vInside3 = v4;
				
				vOutside1Index = v3Index;
				vInside1Index = v1Index;
				vInside2Index = v2Index;
				vInside3Index = v4Index;
			} else
			{
				vOutside1 = v4;
				vInside1 = v1;
				vInside2 = v2;
				vInside3 = v3;
				
				vOutside1Index = v4Index;
				vInside1Index = v1Index;
				vInside2Index = v2Index;
				vInside3Index = v3Index;
			}
			
			if (VERTEX_INTERPOLATION)
			{
				vInside1Ratio = (DEFAULT_THRESHOLD - vOutside1.strength) /
								  (vInside1.strength - vOutside1.strength);
				vInside2Ratio = (DEFAULT_THRESHOLD - vOutside1.strength) /
								  (vInside2.strength - vOutside1.strength);
				vInside3Ratio = (DEFAULT_THRESHOLD - vOutside1.strength) /
								  (vInside3.strength - vOutside1.strength);
			}
			
			renderV1.x = (vInside1.x - vOutside1.x) * vInside1Ratio + vOutside1.x;
			renderV1.y = (vInside1.y - vOutside1.y) * vInside1Ratio + vOutside1.y;
			renderV1.z = (vInside1.z - vOutside1.z) * vInside1Ratio + vOutside1.z;
			
			renderV2.x = (vInside2.x - vOutside1.x) * vInside2Ratio + vOutside1.x;
			renderV2.y = (vInside2.y - vOutside1.y) * vInside2Ratio + vOutside1.y;
			renderV2.z = (vInside2.z - vOutside1.z) * vInside2Ratio + vOutside1.z;
			
			renderV3.x = (vInside3.x - vOutside1.x) * vInside3Ratio + vOutside1.x;
			renderV3.y = (vInside3.y - vOutside1.y) * vInside3Ratio + vOutside1.y;
			renderV3.z = (vInside3.z - vOutside1.z) * vInside3Ratio + vOutside1.z;
			
			if (!normalWasComputed(normals, vOutside1Index, vInside1Index))
			{
				renderN1 = getNormal(renderV1);
				setNormal(normals, renderN1, vOutside1Index, vInside1Index);
			}
			else
			{
				renderN1 = getNormal(normals, vOutside1Index, vInside1Index);
			}
			
			if (!normalWasComputed(normals, vOutside1Index, vInside2Index))
			{
				renderN2 = getNormal(renderV2);
				setNormal(normals, renderN2, vOutside1Index, vInside2Index);
			}
			else
			{
				renderN2 = getNormal(normals, vOutside1Index, vInside2Index);
			}
			
			if (!normalWasComputed(normals, vOutside1Index, vInside3Index))
			{
				renderN3 = getNormal(renderV3);
				setNormal(normals, renderN3, vOutside1Index, vInside3Index);
			}
			else
			{
				renderN3 = getNormal(normals, vOutside1Index, vInside3Index);
			}
			
			/*renderN1 = getNormal(renderV1);
			renderN2 = getNormal(renderV2);
			renderN3 = getNormal(renderV3);*/
			
			glNormal3f(renderN1.x, renderN1.y, renderN1.z);
			//glNormal3f(renderV1.x - blobCenter.x, renderV1.y - blobCenter.y, renderV1.z - blobCenter.z);
			glVertex3f(renderV1.x + center.x, renderV1.y + center.y, renderV1.z + center.z);
			
			glNormal3f(renderN2.x, renderN2.y, renderN2.z);
			//glNormal3f(renderV2.x - blobCenter.x, renderV2.y - blobCenter.y, renderV2.z - blobCenter.z);
			glVertex3f(renderV2.x + center.x, renderV2.y + center.y, renderV2.z + center.z);
			
			glNormal3f(renderN3.x, renderN3.y, renderN3.z);
			//glNormal3f(renderV3.x - blobCenter.x, renderV3.y - blobCenter.y, renderV3.z - blobCenter.z);
			glVertex3f(renderV3.x + center.x, renderV3.y + center.y, renderV3.z + center.z);
			
			return true;
			
			break;
	}
	
	return false;
}

Vertex MetaballEnemy::getNormal(Normal *normals, int vIndex1, int vIndex2)
{
	return normals[(vIndex1 - 1) * VERTICES_IN_CUBE + (vIndex2 - 1)].normal;
}

void MetaballEnemy::setNormal(Normal *normals, Vertex normal, int vIndex1, int vIndex2)
{
	normals[(vIndex1 - 1) * VERTICES_IN_CUBE + (vIndex2 - 1)].normal = normal;
	normals[(vIndex1 - 1) * VERTICES_IN_CUBE + (vIndex2 - 1)].computed = true;
	normals[(vIndex2 - 1) * VERTICES_IN_CUBE + (vIndex1 - 1)].normal = normal;
	normals[(vIndex2 - 1) * VERTICES_IN_CUBE + (vIndex1 - 1)].computed = true;
}

bool MetaballEnemy::normalWasComputed(Normal *normals, int vIndex1, int vIndex2)
{
	return normals[(vIndex1 - 1) * VERTICES_IN_CUBE + (vIndex2 - 1)].computed;
}