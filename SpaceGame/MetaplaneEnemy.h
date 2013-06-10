/*
 *  MetaplaneEnemy.h
 *  Metaballs
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
#include "Projectile.h"
#include "AI.h"
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef MetaplaneEnemy_h
#define MetaplaneEnemy_h

#define DEFAULT_NUM_BLOBS 5
#define DEFAULT_RADIUS 1.f
#define DEFAULT_BLOB_MASS 1
//#define DEFAULT_BLOB_RADIUS_SQUARED 4.f
#define DEFAULT_BLOB_MOVE_SPEED .1f
#define DEFAULT_BLOB_ROTATION_SPEED .5f // Angle in radians
#define DEFAULT_BLOB_LENGTH .5f
//#define DEFAULT_FIELD_STRENGTH_CUTOFF .01f
#define DEFAULT_FIELD_STRENGTH_CUTOFF .15f // Must be positive
//#define DEFAULT_THRESHOLD .5f
#define DEFAULT_THRESHOLD .125f // Keep less than 0.25
#define DEFAULT_NUM_CUBES_PER_DIMENSION 30
#define MIN_FIELD_STRENGTH .01f
#define VERTEX_INTERPOLATION true
#define MIN_DISTANCE_FROM_CENTER -.15f
#define CENTER_SPHERE_RADIUS .1f
#define CENTER_SPHERE_SLICES 25
#define CENTER_SPHERE_STACKS 25
#define PROJECTILE_RADIUS .05f
#define PROJECTILE_SPEED .1f
#define VERTICES_IN_CUBE 8
#define DEFAULT_NEIGHBORS_SIZE 100 // Must be greater than 0
#define NEIGHBORS_INCREASE_FACTOR 2
#define DEFAULT_ACTION_STATE 1
#define PLAYER_DETECTED_ACTION_STATE 2
#define SOUND_DETECTED_ACTION_STATE 3
#define DEFAULT_BLOB_CREATURE_SPEED .01f
#define BLOB_CREATURE_STATIONARY_BOUNDARY 1.f
#define BLOB_CREATURE_MOVE_BOUNDARY 4.f
#define MOVING_ENABLED false
#define FIRE_COUNTER_THRESHOLD 40

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

typedef struct Blob
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
	} Blob;

class MetaplaneEnemy : public AI
	{
	public:
		MetaplaneEnemy(Eigen::Vector3f center, int numBlobs = DEFAULT_NUM_BLOBS, float radius = DEFAULT_RADIUS);
		~MetaplaneEnemy();
		
		Blob *MetaplaneEnemy::getBlobs() { return blobs; };
		Blob MetaplaneEnemy::getBlob(int index);
		int MetaplaneEnemy::getNumBlobs() { return numBlobs; };
		Vertex MetaplaneEnemy::getCenter() { return center; };
		float MetaplaneEnemy::getRadius() { return radius; };
		int MetaplaneEnemy::getBlobSpeed(int index) { return this->blobs[index].moveSpeed; };
		void MetaplaneEnemy::setBlobSpeed(int index, float newSpeed) { this->blobs[index].moveSpeed = newSpeed; };
		void setAllBlobsSpeed(float newSpeed);
		
		Vector3f getLocation();
		Vector3f getDirection();
		
		void MetaplaneEnemy::update()
		{
			checkForCollision();
			checkToMove();
			checkToChangeOrientation();
			checkToFire();
			checkToUpdate();
		};
		
		void render();
		
	private:
		Vertex center; // Center of the Blob Creature
		Blob *blobs; // The blobs in the Blob Creature
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
		
		vector<Projectile> projectiles;
		
		int fireCounter;
		
		void checkForCollision();
		void checkToMove();
		void checkToChangeOrientation();
		void checkToFire();
		void checkToUpdate();
		
		void moveMetaplaneEnemy();
		void updateActionState();
		void moveToDestination();
		void moveProjectiles();
		bool collisionDetected(Vertex destination);
		Vertex normalize(Vertex& vertex);
		Vertex generateRandomNormalizedDirection();
		float getMaxBlobRadius();
		float calculateFieldStrength(VertexWithFieldStrength v);
		float getFieldStrength(int x, int y, int z);
		bool fieldStrengthWasComputed(int x, int y, int z);
		void setFieldStrength(float strength, int x, int y, int z);
		Vertex getNormal(Vertex v);
		Index getCubePosition(Vertex v);
		bool wasAdded(Index curPosition);
		void addNeighbors(Index curPosition, int& endNeighborIndex);
		bool renderCube(Index index);
		bool renderTetrahedronIntersections(VertexWithFieldStrength v1, VertexWithFieldStrength v2,
											VertexWithFieldStrength v3, VertexWithFieldStrength v4,
											int v1Index, int v2Index, int v3Index, int v4Index, Normal *normals);
		Vertex getNormal(Normal *normals, int vIndex1, int vIndex2);
		bool normalWasComputed(Normal *normals, int vIndex1, int vIndex2);
		void setNormal(Normal *normals, Vertex normal, int vIndex1, int vIndex2);
		
		Vertex getClosestPoint(Vertex v, Blob b);
		Vertex getClosestPoint(VertexWithFieldStrength v, Blob b);
	};

#endif