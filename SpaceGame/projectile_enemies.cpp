#include "projectile_particles.h"
#include <stdio.h> 
#include <math.h> 
//particles using std::list (probably faster than vector, since it has a very high turn-over rate?)

using namespace Eigen;

/*
class EnemyChaser : public Projectile{
private:
	float radius;
	int stacks;
	int slices;
	Vector3f position;
	Vector3f velocity;
	float r, g, b, a;
	double pTimeAlive;
public:
	EnemyChaser(Vector3f &position, Vector3f &vel, float radius, int stacks = 10, int slices = 10,
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
*/

Camera* Projectile::cam = NULL;

static vector<Projectile*> en;
vector<Projectile*>* ProjectileEnemies::enemies = &en;
bool isEDead(Projectile* p){
	return p->isDead();
}
void ProjectileEnemies::deleteDead(){
	enemies->erase( std::remove_if(enemies->begin(), enemies->end(), isEDead), enemies->end() );
}


EnemyChaser::EnemyChaser(Vector3f &pos, Vector3f &vel, float radius, int stacks, int slices, float r1, float g1, float b1, float a1)
	: Projectile()
{
	r = r1; 
	g = g1;
	b = b1;
	a = a1;
	this->radius = radius;
	this->stacks = stacks;
	this->slices = slices;
	position = pos;
	velocity = vel;
	pTimeAlive = 0;
	accel = Vector3f(0, 0, 0);
	typeString = "Enemy";
	health = 100.0;
	damage = 0.1;
	hitColorCounter = 0.0;
	hitRadius = radius;
	ProjectileEnemies::enemies->push_back(this);
}

void EnemyChaser::hit(Vector3f loc)
{
	//hitted = true;
	hitColorCounter = 500;
	return;
}

void EnemyChaser::display(Vector3f cam, bool glow){
	if (!isDead()){
		glColor4f(r, g, b, a);
		if(hitColorCounter>0.0){glColor4f(1.0, 0.0, 0.0, a);}
		glPushMatrix();
		glTranslatef(position(0), position(1), position(2));
		glutSolidSphere(radius, slices, stacks);
		//glutWireSphere(radius, slices, stacks);
		glPopMatrix();
	}
}

void EnemyChaser::update(double dt){
	if(isDead()){
		return;
	}
	Vector3f delta = velocity * (((double)dt)/1000.0);
	position += delta; 
	pTimeAlive += dt;

	//accelerate towards camera
	accel = (cam->getPivotPoint() - position).normalized();
	for(vector<Projectile*>::iterator it = ProjectileEnemies::enemies->begin(); it!=ProjectileEnemies::enemies->end(); ++it){
		if(position != (*it)->getPosition()){
			if( ((*it)->getPosition()-position).norm() < ((*it)->hitRadius+hitRadius) * 1.5  ){
				accel +=  -((*it)->getPosition()-position).normalized() * 3.3;
			}
		}
	}
	accel = accel.normalized() * 0.5; //limit acceleration

	if( accel[0]!=accel[0] || accel[1]!=accel[1] || accel[2]!=accel[2]  ){
		accel = Vector3f(0, 0, 0);
	}
	//velocity = velocity + accel * (((double)dt)/1000.0);
	velocity = accel;

	//hitcolor
	hitColorCounter = hitColorCounter -dt;
}

bool EnemyChaser::isDead(){
	return (health <= 0.0);
}


//hit function of enemies is special -> doesn't do anything except temporarily change color


EnemyChaserBig::EnemyChaserBig(Vector3f &pos, Vector3f &vel, float radius, int stacks, int slices, float r1, float g1, float b1, float a1)
	: Projectile()
{
	r = r1; 
	g = g1;
	b = b1;
	a = a1;
	this->radius = radius;
	this->stacks = stacks;
	this->slices = slices;
	position = pos;
	velocity = vel;
	pTimeAlive = 0;
	accel = Vector3f(0, 0, 0);
	typeString = "Enemy";
	health = 400.0;
	damage = 0.3;
	hitColorCounter = 0.0;
	hitRadius = radius;
	ProjectileEnemies::enemies->push_back(this);
}

void EnemyChaserBig::hit(Vector3f loc)
{
	//hitted = true;
	hitColorCounter = 500;
	return;
}

void EnemyChaserBig::display(Vector3f cam, bool glow){
	if (!isDead()){
		glColor4f(r, g, b, a);
		if(hitColorCounter>0.0){glColor4f(1.0, 0.0, 0.0, a);}
		glPushMatrix();
		glTranslatef(position(0), position(1), position(2));
		glutSolidSphere(radius, slices, stacks);
		//glutWireSphere(radius, slices, stacks);
		glPopMatrix();
	}
}

void EnemyChaserBig::update(double dt){
	if(isDead()){
		return;
	}
	Vector3f delta = velocity * (((double)dt)/1000.0);
	position += delta; 
	pTimeAlive += dt;

	//accelerate towards camera
	accel = (cam->getPivotPoint() - position).normalized();
	for(vector<Projectile*>::iterator it = ProjectileEnemies::enemies->begin(); it!=ProjectileEnemies::enemies->end(); ++it){
		if(position != (*it)->getPosition()){
			if( ((*it)->getPosition()-position).norm() < ((*it)->hitRadius+hitRadius) * 1.5  ){
				accel +=  -((*it)->getPosition()-position).normalized() * 3.3;
			}
		}
	}
	accel = accel.normalized() * 1.5; //limit acceleration

	if( accel[0]!=accel[0] || accel[1]!=accel[1] || accel[2]!=accel[2]  ){
		accel = Vector3f(0, 0, 0);
	}
	//velocity = velocity + accel * (((double)dt)/1000.0);
	velocity = accel;

	//hitcolor
	hitColorCounter = hitColorCounter -dt;
}

bool EnemyChaserBig::isDead(){
	return (health <= 0.0);
}











//
//
//firing enemy
EnemyShooter::EnemyShooter(Vector3f &pos, Vector3f &vel, float radius, int stacks, int slices, float r1, float g1, float b1, float a1)
	: Projectile()
{
	r = r1; 
	g = g1;
	b = b1;
	a = a1;
	this->radius = radius;
	this->stacks = stacks;
	this->slices = slices;
	position = pos;
	velocity = vel;
	pTimeAlive = 0;
	accel = Vector3f(0, 0, 0);
	typeString = "Enemy";
	health = 100.0;
	damage = 0.1;
	hitColorCounter = 0.0;
	hitRadius = radius;
	fireBulletCountDown = 10000.0 * (rand()%100000 / 100000.0) ;
	firesOwnBullets = true;
	ProjectileEnemies::enemies->push_back(this);
}

void EnemyShooter::hit(Vector3f loc)
{
	//hitted = true;
	hitColorCounter = 500;
	return;
}

void EnemyShooter::display(Vector3f cam, bool glow){
	if (!isDead()){
		glColor4f(r, g, b, a);
		if(hitColorCounter>0.0){glColor4f(1.0, 0.0, 0.0, a);}
		glPushMatrix();
		glTranslatef(position(0), position(1), position(2));
		glutSolidSphere(radius, slices, stacks);
		//glutWireSphere(radius, slices, stacks);
		glPopMatrix();
	}
}

void EnemyShooter::update(double dt){
	if(isDead()){
		return;
	}
	Vector3f delta = velocity * (((double)dt)/1000.0);
	position += delta; 
	pTimeAlive += dt;
	fireBulletCountDown -= dt;

	//accelerate towards camera
	accel = (cam->getPivotPoint() - position).normalized();
	for(vector<Projectile*>::iterator it = ProjectileEnemies::enemies->begin(); it!=ProjectileEnemies::enemies->end(); ++it){
		if(position != (*it)->getPosition()){
			if( ((*it)->getPosition()-position).norm() < ((*it)->hitRadius+hitRadius) * 1.5  ){
				accel +=  -((*it)->getPosition()-position).normalized() * 3.3;
			}
		}
	}
	accel = accel.normalized() * 0.5; //limit acceleration

	if( accel[0]!=accel[0] || accel[1]!=accel[1] || accel[2]!=accel[2]  ){
		accel = Vector3f(0, 0, 0);
	}
	//velocity = velocity + accel * (((double)dt)/1000.0);
	velocity = accel;

	//hitcolor
	hitColorCounter = hitColorCounter -dt;
}

bool EnemyShooter::isDead(){
	return (health <= 0.0);
}