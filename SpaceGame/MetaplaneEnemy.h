/*
 *  MetaplaneEnemy.h
 *  Metaballs
 *
 *  Created by Jordan Davidson on 6/9/13.
 *  Copyright 2013 Stanford University. All rights reserved.
 *
 */

//#include <Carbon/Carbon.h>

#include <stdlib.h>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>
#include "AI.h"
using namespace std;
#include <cstdio>
#include <iostream>

#ifndef MetaplaneEnemy_h
#define MetaplaneEnemy_h

const int DEFAULT_NUM_PLANE_BLOBS = 5;
const float DEFAULT_PLANE_RADIUS = 1.f;

class MetaplaneEnemy : public AI
	{
	public:
		MetaplaneEnemy(Eigen::Vector3f center, int numBlobs = DEFAULT_NUM_PLANE_BLOBS, float radius = DEFAULT_PLANE_RADIUS);
		~MetaplaneEnemy();
		
		int MetaplaneEnemy::getNumBlobs() { return numBlobs; };
		Eigen::Vector3f MetaplaneEnemy::getCenter();
		float MetaplaneEnemy::getRadius() { return radius; };
		int MetaplaneEnemy::getBlobSpeed(int index) { return this->blobs[index].moveSpeed; };
		void MetaplaneEnemy::setBlobSpeed(int index, float newSpeed) { this->blobs[index].moveSpeed = newSpeed; };
		void setAllBlobsSpeed(float newSpeed);
		
		Eigen::Vector3f getLocation();
		Eigen::Vector3f getDirection();
		
		void MetaplaneEnemy::update();
		
		void render();
		
		Vector3f MetaplaneEnemy::CenterOfMass() {Vector3f v(center.x, center.y, center.z); return v;}
		
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
		
		typedef struct PlaneBlob
			{
				Vertex center;
				float moveSpeed;
				Vertex direction;
				float rotSpeed;
				Vector3f rotAxis;
				Vector3f line1;
				Vector3f line2;
				float length;
				Vertex segmentPoint1;
				Vertex segment1;
				Vertex segmentPoint2;
				Vertex segment2;
				Vertex segmentPoint3;
				float segmentDistanceSquared;
				bool pastCenter;
			} PlaneBlob;
		
		Vertex center; // Center of the PlaneBlob Creature
		PlaneBlob *blobs; // The blobs in the PlaneBlob Creature
		int numBlobs; // The number of blobs in the PlaneBlob Creature
		float radius; // The farthest distance the outer edge of a PlaneBlob can get from the center of the PlaneBlob Creature
		float maxBlobRadius; // The farthest distance the outer edge of a PlaneBlob can get from its center
		
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
		
		int actionState; //Corresponds to the PlaneBlob Creature's current "action state"
		Vertex direction; //Directio in which the PlaneBlob Creature is currently moving
		
		float blobMaterialAmbient[4]; // Ambient material property for the blobs of the PlaneBlob Creature
		float blobMaterialDiffuse[4]; // Diffuse material property for the blobs of the PlaneBlob Creature
		float blobMaterialSpecular[4]; // Specular material property for the blobs of the PlaneBlob Creature
		float blobShininess; // Shininess material property for the blobs of the PlaneBlob Creature
		
		float centerMaterialAmbient[4]; // Ambient material property for the center sphere of the PlaneBlob Creature
		float centerMaterialDiffuse[4]; // Diffuse material property for the center sphere of the PlaneBlob Creature
		float centerMaterialSpecular[4]; // Specular material property for the center sphere of the PlaneBlob Creature
		float centerShininess; // Shininess material property for the blobs of the PlaneBlob Creature
		
		//vector<Projectile> projectiles;
		
		int fireCounter;
		bool hasCollided;
		
		void checkForCollision();
		void checkToMove();
		void checkToChangeOrientation();
		void checkToFire();
		void checkToUpdate();
		
		bool isPlayerVisible();
		
		MetaplaneEnemy::PlaneBlob *MetaplaneEnemy::getBlobs() { return blobs; };
		MetaplaneEnemy::PlaneBlob MetaplaneEnemy::getBlob(int index);
		void moveMetaplaneEnemy();
		void updateActionState();
		void moveToDestination();
		//void moveProjectiles();
		bool collisionDetected(Vertex destination);
		MetaplaneEnemy::Vertex normalize(Vertex& vertex);
		MetaplaneEnemy::Vertex generateRandomNormalizedDirection();
		float getMaxBlobRadius();
		float calculateFieldStrength(VertexWithFieldStrength v);
		float getFieldStrength(int x, int y, int z);
		bool fieldStrengthWasComputed(int x, int y, int z);
		void setFieldStrength(float strength, int x, int y, int z);
		MetaplaneEnemy::Vertex getNormal(Vertex v);
		MetaplaneEnemy::Index getCubePosition(Vertex v);
		bool wasAdded(Index curPosition);
		void addNeighbors(Index curPosition, int& endNeighborIndex);
		bool renderCube(Index index);
		bool renderTetrahedronIntersections(VertexWithFieldStrength v1, VertexWithFieldStrength v2,
											VertexWithFieldStrength v3, VertexWithFieldStrength v4,
											int v1Index, int v2Index, int v3Index, int v4Index, Normal *normals);
		MetaplaneEnemy::Vertex getNormal(Normal *normals, int vIndex1, int vIndex2);
		bool normalWasComputed(Normal *normals, int vIndex1, int vIndex2);
		void setNormal(Normal *normals, Vertex normal, int vIndex1, int vIndex2);
		
		MetaplaneEnemy::Vertex getClosestPoint(Vertex v, PlaneBlob b);
		MetaplaneEnemy::Vertex getClosestPoint(VertexWithFieldStrength v, PlaneBlob b);
	};

#endif