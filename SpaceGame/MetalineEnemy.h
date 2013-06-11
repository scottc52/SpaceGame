/*
 *  MetalineEnemy.h
 *  Metalines
 *
 *  Created by Jordan Davidson on 6/9/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

#include <stdlib.h>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
#include "AI.h"
#include <cstdio>
#include <iostream>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef MetalineEnemy_h
#define MetalineEnemy_h

const int DEFAULT_NUM_LINE_BLOBS = 5;
const float DEFAULT_LINE_RADIUS = 1.f;

class MetalineEnemy : public AI
	{
	public:
		MetalineEnemy(Eigen::Vector3f center, int numBlobs = DEFAULT_NUM_LINE_BLOBS, float radius = DEFAULT_LINE_RADIUS);
		~MetalineEnemy();
		
		int MetalineEnemy::getNumBlobs() { return numBlobs; };
		Eigen::Vector3f MetalineEnemy::getCenter();
		float MetalineEnemy::getRadius() { return radius; };
		int MetalineEnemy::getBlobSpeed(int index) { return this->blobs[index].moveSpeed; };
		void MetalineEnemy::setBlobSpeed(int index, float newSpeed) { this->blobs[index].moveSpeed = newSpeed; };
		void setAllBlobsSpeed(float newSpeed);
		
		Eigen::Vector3f getLocation();
		Eigen::Vector3f getDirection();
		
		void MetalineEnemy::update();
		
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
		
		typedef struct LineBlob
			{
				Vertex center;
				float moveSpeed;
				Vertex direction;
				float rotSpeed;
				Vector3f rotAxis;
				Vector3f line;
				float length;
				Vertex segmentPoint1;
				Vertex segment;
				float segmentDistanceSquared;
				bool pastCenter;
			} LineBlob;
		
		Vertex center; // Center of the LineBallBlob Creature
		LineBlob *blobs; // The blobs in the LineBallBlob Creature
		int numBlobs; // The number of blobs in the LineBallBlob Creature
		float radius; // The farthest distance the outer edge of a LineBallBlob can get from the center of the LineBallBlob Creature
		float maxBlobRadius; // The farthest distance the outer edge of a LineBallBlob can get from its center
		
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
		
		int actionState; //Corresponds to the LineBallBlob Creature's current "action state"
		Vertex direction; //Directio in which the LineBallBlob Creature is currently moving
		
		float blobMaterialAmbient[4]; // Ambient material property for the blobs of the LineBallBlob Creature
		float blobMaterialDiffuse[4]; // Diffuse material property for the blobs of the LineBallBlob Creature
		float blobMaterialSpecular[4]; // Specular material property for the blobs of the LineBallBlob Creature
		float blobShininess; // Shininess material property for the blobs of the LineBallBlob Creature
		
		float centerMaterialAmbient[4]; // Ambient material property for the center sphere of the LineBallBlob Creature
		float centerMaterialDiffuse[4]; // Diffuse material property for the center sphere of the LineBallBlob Creature
		float centerMaterialSpecular[4]; // Specular material property for the center sphere of the LineBallBlob Creature
		float centerShininess; // Shininess material property for the blobs of the LineBallBlob Creature
		
		//vector<Projectile> projectiles;
		
		int fireCounter;
		
		void checkForCollision();
		void checkToMove();
		void checkToChangeOrientation();
		void checkToFire();
		void checkToUpdate();
		
		bool hasCollided;
		
		MetalineEnemy::LineBlob *MetalineEnemy::getBlobs() { return blobs; };
		MetalineEnemy::LineBlob MetalineEnemy::getBlob(int index);
		void moveMetalineEnemy();
		void updateActionState();
		void moveToDestination();
		//void moveProjectiles();
		bool collisionDetected(Vertex destination);
		MetalineEnemy::Vertex normalize(Vertex& vertex);
		MetalineEnemy::Vertex generateRandomNormalizedDirection();
		float getMaxBlobRadius();
		float calculateFieldStrength(VertexWithFieldStrength v);
		float getFieldStrength(int x, int y, int z);
		bool fieldStrengthWasComputed(int x, int y, int z);
		void setFieldStrength(float strength, int x, int y, int z);
		MetalineEnemy::Vertex getNormal(Vertex v);
		MetalineEnemy::Index getCubePosition(Vertex v);
		bool wasAdded(Index curPosition);
		void addNeighbors(Index curPosition, int& endNeighborIndex);
		bool renderCube(Index index);
		bool renderTetrahedronIntersections(VertexWithFieldStrength v1, VertexWithFieldStrength v2,
											VertexWithFieldStrength v3, VertexWithFieldStrength v4,
											int v1Index, int v2Index, int v3Index, int v4Index, Normal *normals);
		MetalineEnemy::Vertex getNormal(Normal *normals, int vIndex1, int vIndex2);
		bool normalWasComputed(Normal *normals, int vIndex1, int vIndex2);
		void setNormal(Normal *normals, Vertex normal, int vIndex1, int vIndex2);
		
		MetalineEnemy::Vertex getClosestPoint(Vertex v, LineBlob b);
		MetalineEnemy::Vertex getClosestPoint(VertexWithFieldStrength v, LineBlob b);
	};

#endif