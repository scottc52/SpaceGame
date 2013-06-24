#ifndef PROJECTILE_PARTICLES_H
#define PROJECTILE_PARTICLES_H
//forward declare
class Projectile; 
class Slug;
class SmokyBullet;
class Ball; 

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

#include <cassert>
#include <iostream>

#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <list>

#include <time.h>
#include <math.h>
#include "Mesh.h"
#include "Sound.h"
#include "Camera.h"


#define PI 3.14159265358979323846

#define MAX_TIME (4000)

//// EigenIncludes
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;


//prjectile interface
class Projectile{
public: 	
	virtual ~Projectile(){}
	bool drawCollision;
	vector<Vec3f>boundingBox;
	Projectile(): damage(0), mass(1.0), hitted(false), typeString(""), owner(""), health(0.0), firesOwnBullets(false){
		drawCollision = false;
		boundingBox.push_back(Vec3f(-0.05, -0.05, -0.05));
		boundingBox.push_back(Vec3f(0.05, -0.05, -0.05));
		boundingBox.push_back(Vec3f(-0.05, 0.05, -0.05));
		boundingBox.push_back(Vec3f(0.05, 0.05, -0.05));
		boundingBox.push_back(Vec3f(-0.05, -0.05, 0.05));
		boundingBox.push_back(Vec3f(-0.05, 0.05, 0.05));
		boundingBox.push_back(Vec3f(0.05, -0.05, 0.05));
		boundingBox.push_back(Vec3f(-0.05, -0.05, 0.05));
	} 
	virtual void hit(Vector3f loc)=0;
	virtual void display(Vector3f cam, bool glow) =0;
	virtual bool isDead() = 0; 
	virtual void update(double dt) =0; 
	virtual double timeAlive() =0;
	virtual Vector3f &getPosition()=0; 
	virtual Vector3f &getVelocity()=0; 
	bool hitted; 
	double damage;
	double mass; 
	//Scott
	static Camera *cam;
	double health;
	double hitRadius;
	string typeString;
	string owner;
	bool firesOwnBullets;
};


class ProjectileEnemies{
public:
		static vector<Projectile*> *enemies;
		static void deleteDead();
};


class NavShot : public Projectile{
private: 
	Vector3f position;
	Vector3f velocity; 
	float r, g, b, a;
	double pTimeAlive;
	double pTimeSinceRedraw;  
	static Sound *sound;
	double ttl; 
	double hitTimePrediction;
	Vector3f hitPositionPrediction;
public:
	static bool isPlayerMoving;
	static Vector3f targetPlayerPos;
	static MyMesh *room;
	NavShot(Vector3f &pos1, Vector3f &velocity1, float r1 = 0.9f, float g1 = 0.7f,  float b1 = 0.4f, float a1 = 0.5f): Projectile(){
		r = r1; 
		g = g1;
		b = b1;
		a = a1;
		position = pos1;
		velocity = velocity1;
		pTimeAlive = 0;
		pTimeSinceRedraw = 0; 
		ttl = -1;
		typeString = "NavShot";
		owner = "player";
		sound =NULL;
		#ifndef __linux__
		if(!sound){sound = new Sound("sounds/laser2.wav");}
		sound->Play();
		#endif
		calcHit(position, velocity, room);
	}
	void calcHit(Vector3f pos, Vector3f vel, MyMesh* room){
		hitTimePrediction = -1.0;
		for (MyMesh::FaceIter it = room->faces_begin(); it != room->faces_end(); ++it) {
			//assuming triangular meshes
			MyMesh::HalfedgeHandle it2 = room->halfedge_handle(it.handle());
			Vector3f ps[4];
			for(int v = 0; v< 4; v++){
				MyMesh::VertexHandle v_handle = room->to_vertex_handle(it2);
				Vec3f p = room->point(v_handle);
				ps[v] = Vector3f(p[0], p[1], p[2]);
				it2 = room->next_halfedge_handle(it2);
			}
			//calculate intersection and time
			bool intersect = false;
			double tInt = -1.0;
			Vector3f normal = (ps[0]-ps[1]).cross(ps[2]-ps[1]).normalized();
			double A = normal[0];
			double B = normal[1];
			double C = normal[2];
			double D = 0 - A*ps[0][0] - B*ps[0][1] - C*ps[0][2];
			
			if((A*vel[0] + B*vel[1] + C*vel[2]) != 0){
				tInt = -(A*pos[0] + B*pos[1] + C*pos[2] + D) / (A*vel[0] + B*vel[1] + C*vel[2]);
				if(tInt > 0.00000001){
					intersect = true;
				}else{ tInt = -1.0;}
			}
		
			Vector3f posInt = pos + vel*tInt;
			//cout << tInt << "\n";
			
			//check that intersection is withtin plane
			int numVert = (ps[0]==ps[3])? 3: 4;
			if(intersect){
				Vector3f mid = (ps[0]+ps[1]+ps[2]+ps[3])/4.0;
				for(int v = 0; v< numVert; v++){
					Vector3f v1 = ps[(v+1)%4] - ps[v];
					Vector3f v2 = mid - ps[v];
					Vector3f vInt = posInt - ps[v];
					if(v1.cross(v2).dot( v1.cross(vInt) ) <= 0.0 ){
						intersect = false;
						tInt = -1.0;
					}
				}
			}

			//update if necessary
			tInt = tInt * 1000;
			if(intersect && (hitTimePrediction<0 || tInt < hitTimePrediction)){
				hitTimePrediction = tInt;
				//do some complex hit position calculation
				float delta = 0.0002;
				if(vel.dot(normal)>0.0){ delta = -delta;}
				hitPositionPrediction = posInt + normal * delta;
				//cin.ignore(1);
			}
		}
	}
	void hit(Vector3f loc){
		ttl = timeAlive() + 300;
		//pTimeAlive += 2*MAX_TIME;
		cout<<"hit" <<endl;
		hitted = true;
		return;
	}
	void display(Vector3f cam, bool glow = false){
		/*
		if (!isDead()){
			glBegin(GL_LINE);
			glColor4f(r, g, b, a);
			glVertex3f(position[0], position[1], position[2]); 
			Vector3f tmp = position - velocity * ((double)pTimeSinceRedraw)/1000.0; 
			glVertex3f(tmp[0], tmp[1], tmp[2]);
			pTimeSinceRedraw = 0;
			glEnd(); 
		}
		*/
		if (!isDead()){
			glColor4f(r, g, b, a);
			glPushMatrix();
			glTranslatef(position(0), position(1), position(2));
			glutSolidSphere(0.01, 10, 10);
			glPopMatrix();
		}
	}
	bool isDead(){ return (pTimeAlive > MAX_TIME || (ttl > 0 && pTimeAlive > ttl) || hitted); }
	void update(double dt){
		if (isDead() ) {
			if (sound) {
				//sound->UninitializeSounds();
				//sound->Pause();
				//delete sound;
				//sound = NULL;
			}
		}
		Vector3f delta = velocity * ((dt)/1000.0);
		position += delta; 
		pTimeAlive += dt;
		pTimeSinceRedraw += dt;

		//nav stuff
		if(!isDead() && pTimeAlive > hitTimePrediction && hitTimePrediction>0.0){
			isPlayerMoving = true;
			targetPlayerPos = hitPositionPrediction;
			hit(position);
		}
	}
	double timeAlive(){return pTimeAlive;}
	Vector3f &getPosition(){return position;}
	Vector3f &getVelocity(){return velocity;}  
};

class Slug : public Projectile{
private: 
	Vector3f position;
	Vector3f velocity; 
	float r, g, b, a;
	double pTimeAlive;
	double pTimeSinceRedraw;  
	static Sound *sound;
	double ttl; 
public:
	Slug(Vector3f &position, Vector3f &vel, float r1 = 0.9f, float g1 = 0.7f,  float b1 = 0.4f, float a1 = 0.5f);
	void hit(Vector3f loc); 
	void display(Vector3f cam, bool glow = false);
	bool isDead();
	void update(double dt); 
	double timeAlive();
	Vector3f &getPosition();
	Vector3f &getVelocity(){return velocity;}  
};

class Ball : public Projectile{
private:
	float radius;
	int stacks;
	int slices;
	Vector3f position;
	Vector3f velocity;
	float r, g, b, a;
	double pTimeAlive;
public:
	Ball(Vector3f &position, Vector3f &vel, float radius, int stacks = 10, int slices = 10,
		float r1 = 0.15f, float g1 = 0.7f, float b1 = 0.15f, float a1 = 0.5f);
	void hit(Vector3f loc);
	void display(Vector3f cam, bool glow = false);
	bool isDead();
	void update(double dt); 
	double timeAlive() { return pTimeAlive; }
	vector<Vec3f> boundingBox;
	Vector3f &getPosition() { return position; }
	Vector3f &getVelocity() {return velocity; }
};

class EnemyChaser : public Projectile{
private:
	float radius;
	int stacks;
	int slices;
	Vector3f position;
	Vector3f velocity;
	Vector3f accel;
	float r, g, b, a;
	double pTimeAlive;
	double hitColorCounter;
public:
	EnemyChaser(Vector3f &position, Vector3f &vel, float radius, int stacks = 20, int slices = 20,
		float r1 = 0.15f, float g1 = 0.7f, float b1 = 0.15f, float a1 = 0.5f);
	void hit(Vector3f loc);
	void display(Vector3f cam, bool glow = false);
	bool isDead();
	void update(double dt); 
	double timeAlive() { return pTimeAlive; }
	vector<Vec3f> boundingBox;
	Vector3f &getPosition() { return position; }
	Vector3f &getVelocity() {return velocity; }
};

class EnemyChaserBig : public Projectile{
private:
	float radius;
	int stacks;
	int slices;
	Vector3f position;
	Vector3f velocity;
	float r, g, b, a;
	double pTimeAlive;
public:
	EnemyChaserBig(Vector3f &position, Vector3f &vel, float radius, int stacks = 10, int slices = 10,
		float r1 = 0.15f, float g1 = 0.7f, float b1 = 0.15f, float a1 = 0.5f);
	void hit(Vector3f loc);
	void display(Vector3f cam, bool glow = false);
	bool isDead();
	void update(double dt); 
	double timeAlive() { return pTimeAlive; }
	vector<Vec3f> boundingBox;
	Vector3f &getPosition() { return position; }
	Vector3f &getVelocity() {return velocity; }
};

class EnemyShooter : public Projectile{
private:
	float radius;
	int stacks;
	int slices;
	Vector3f position;
	Vector3f velocity;
	float r, g, b, a;
	double pTimeAlive;
public:
	EnemyShooter(Vector3f &position, Vector3f &vel, float radius, int stacks = 10, int slices = 10,
		float r1 = 0.15f, float g1 = 0.7f, float b1 = 0.15f, float a1 = 0.5f);
	void hit(Vector3f loc);
	void display(Vector3f cam, bool glow = false);
	bool isDead();
	void update(double dt); 
	double timeAlive() { return pTimeAlive; }
	vector<Vec3f> boundingBox;
	Vector3f &getPosition() { return position; }
	Vector3f &getVelocity() {return velocity; }
};


class Particle{
public:
	Vector3f location;
	Vector3f velocity; // units per second
	Vector3f acceleration; // units per second^2
	int life; //miliseconds
	int lifeMax;

	//parameters for drawing
	float size;
	float color[4];

	Particle();
	Particle(int lif, Vector3f loc, Vector3f vel, Vector3f accel);
	void setLook(float psize, float *pcolor);
	void setAccel(Vector3f newAccel);
	void update(double dt);
	void display(bool drawingGlowPass, bool particleGlow);
	bool isDead();

	float sizeAttenuation();
	float alphaAttenuation();
};



class SimpleParticleSystem{
public:
	SimpleParticleSystem();
	float pCarryOver; // carry over rate values that get rounded off

	//emitter properties
	list<Particle> particles;
	Vector3f location;
	Vector3f velocity;
	Vector3f acceleration;

	//emitted particle properties
	float rate; // particles per second
	double pLife;
	float pSpeed;
	Vector3f pAccel;
	float pColor[4];
	float pSize;
	float pColorWig;

	//emittor velocity offset
	Vector3f pVelOffset;

	void setAccel(Vector3f newAccel);
	void update(double dt);
	void display(bool drawingGlowPass, bool particleGlow);
	bool isDead();
};

class SmokyBullet : public Projectile{
public:
	//bullet properties
	SimpleParticleSystem pSystem;
	Vector3f location;
	Vector3f velocity;
	Vector3f acceleration;
	//time is all in milliseconds
	double t; //time bullet's been alive; so we can implement killing stray bullets in the future
	double hitT;//time hit. -1 if not hit yet.
	bool hitB;
	int hitLife; //time staying alive after hit

	int deathT; // time at which stray bullets die.

	bool glow;

	//movement
	Vector3f up;
	float offset1;
	float tScale1;
	float mag1;
	float offset2;
	float tScale2;
	float mag2;
	float hitWig;

	double timeAlive(){
		return t; 
	}

	SmokyBullet();
	SmokyBullet(Vector3f loc, Vector3f vel, float c0, float c1, float c2, float c3);
	void setAccel(Vector3f newAccel);
	void update(double dt);
	Vector3f &getPosition(){
		return location;
	}
	//if hit. call hit() first, then update()
	void hit(Vector3f hitLocation);
	void display(Vector3f camera, bool drawGlow);
	bool isDead();
	Vector3f &getVelocity(){ return velocity;} 
private:
	static Sound *sound1;
	static Sound *sound2;
};

#endif PROJECTILE_PARTICLES_H
