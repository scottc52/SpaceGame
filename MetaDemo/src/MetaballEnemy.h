/*
 *  MetaballEnemy.h
 *  Metaballs
 *
 *  Created by Jordan Davidson on 5/3/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

#include <stdlib.h>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
#include "AI.h"
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef MetaballEnemy_h
#define MetaballEnemy_h

const int DEFAULT_NUM_BALL_BLOBS = 10;
const float DEFAULT_BALL_RADIUS = 1.f;

class MetaballEnemy : public AI
{
public:
	MetaballEnemy(Eigen::Vector3f center, int numBlobs = DEFAULT_NUM_BALL_BLOBS, float radius = DEFAULT_BALL_RADIUS);
	~MetaballEnemy();
	
	int MetaballEnemy::getNumBlobs() { return numBlobs; };
	Eigen::Vector3f MetaballEnemy::getCenter();
	float MetaballEnemy::getRadius() { return radius; };
	int MetaballEnemy::getBlobSpeed(int index) { return this->blobs[index].speed; };
	void MetaballEnemy::setBlobSpeed(int index, float newSpeed) { this->blobs[index].speed = newSpeed; };
	void setAllBlobsSpeed(float newSpeed);
	
	Eigen::Vector3f getLocation();
	Eigen::Vector3f getDirection();
	
	void MetaballEnemy::update()
	{
		checkForCollision();
		checkToMove();
		checkToChangeOrientation();
		checkToFire();
		checkToUpdate();
	};
	
	void render();
	
private:
	typedef struct Vertex
		{
			float x;
			float y;
			float z;
		} Vertex;
	
	typedef struct VertexWithFieldStrength
		{
			float x;
			float y;
			float z;
			float strength;
		} VertexWithFieldStrength;
	
	typedef struct Index
		{
			int x;
			int y;
			int z;
		} Index;
	
	typedef struct FieldStrength
		{
			float strength;
			bool computed;
		} FieldStrength;
	
	typedef struct Normal
		{
			Vertex normal;
			bool computed;
		} Normal;
	
	typedef struct BallBlob
		{
			Vertex center;
			float speed;
			Vertex direction;
			bool pastCenter;
		} BallBlob;
	
	Vertex center; // Center of the Blob Creature
	BallBlob *blobs; // The blobs in the Blob Creature
	int numBlobs; // The number of blobs in the Blob Creature
	float radius; // The farthest distance the outer edge of a blob can get from the center of the Blob Creature
	float maxBlobRadius; // The farthest distance the outer edge of a blob can get from its center
	
	bool *added; // Notes whether a vertex has previously been added to an array of vectors that have or will
				 // be computed in the Marching Cubes algorithm
	FieldStrength *fieldStrengths; // Stores the field strengths of vertices accessed by the Marching Cubes
								   // algorithm
	Index *neighbors; // An array of indices into the vertex array representing the vectors that have or will
					  // be computed in the Marching Cubes algorithm
	
	int addedSize3D; // Size of the added array (Saves later computation)
	int addedSize2D; // Size of 2 dimensions of the added array (Saves later computation)
	int strengthSize3D; // Size of the field strength array (Saves later computation)
	int strengthSize2D; // Size of the field strength array (Saves later computation)
	int neighborsSize; // Current size of the nieghbors array
	float increment; // Distance in the x, y, and z direction of one vertex of a cube from the same vertex in
					 // a neighboring cube (Saves computation)
	float fieldStrengthConstantSquared; // Squared constant used in the field strength equation
										// (Saves computation)
	float fieldStrengthConstantSquaredSquared; // Twice squared constant used in the field strength equation
											   // (Saves computation)
	float fieldStrengthDistanceSquaredThreshold; // Maximum distance squared from a blob's center for it to
												// influence the field strength (Saves computation)
	
	int actionState; //Corresponds to the Blob Creature's current "action state"
	Vertex direction; //Directio in which the Blob Creature is currently moving
	
	float blobMaterialAmbient[4]; // Ambient material property for the blobs of the Blob Creature
	float blobMaterialDiffuse[4]; // Diffuse material property for the blobs of the Blob Creature
	float blobMaterialSpecular[4]; // Specular material property for the blobs of the Blob Creature
	float blobShininess; // Shininess material property for the blobs of the Blob Creature
	
	float centerMaterialAmbient[4]; // Ambient material property for the center sphere of the Blob Creature
	float centerMaterialDiffuse[4]; // Diffuse material property for the center sphere of the Blob Creature
	float centerMaterialSpecular[4]; // Specular material property for the center sphere of the Blob Creature
	float centerShininess; // Shininess material property for the blobs of the Blob Creature
	
	int fireCounter;
	
	void checkForCollision();
	void checkToMove();
	void checkToChangeOrientation();
	void checkToFire();
	void checkToUpdate();
	
	MetaballEnemy::BallBlob *MetaballEnemy::getBlobs() { return blobs; };
	MetaballEnemy::BallBlob MetaballEnemy::getBlob(int index);
	void moveMetaballEnemy();
	void updateActionState();
	void moveToDestination();
	bool collisionDetected(Vertex destination);
	MetaballEnemy::Vertex normalize(Vertex& vertex);
	MetaballEnemy::Vertex generateRandomNormalizedDirection();
	float getMaxBlobRadius();
	float calculateFieldStrength(VertexWithFieldStrength v);
	float getFieldStrength(int x, int y, int z);
	bool fieldStrengthWasComputed(int x, int y, int z);
	void setFieldStrength(float strength, int x, int y, int z);
	MetaballEnemy::Vertex getNormal(Vertex v);
	MetaballEnemy::Index getCubePosition(Vertex v);
	bool wasAdded(Index curPosition);
	void addNeighbors(Index curPosition, int& endNeighborIndex);
	bool renderCube(Index index);
	bool renderTetrahedronIntersections(VertexWithFieldStrength v1, VertexWithFieldStrength v2,
										VertexWithFieldStrength v3, VertexWithFieldStrength v4,
										int v1Index, int v2Index, int v3Index, int v4Index, Normal *normals);
	MetaballEnemy::Vertex getNormal(Normal *normals, int vIndex1, int vIndex2);
	bool normalWasComputed(Normal *normals, int vIndex1, int vIndex2);
	void setNormal(Normal *normals, Vertex normal, int vIndex1, int vIndex2);
};

#endif