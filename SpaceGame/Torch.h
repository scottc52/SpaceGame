/**
* Flame Particles System
*/

#ifndef _TORCH_H
#define _TORCH_H

#include <Eigen/Core>

#include <list>
#include <cmath>
#include <iostream>
using namespace Eigen; 
using namespace std;

struct JParticle{
	Vector3f vel;
	Vector3f pos; 
	double timeAlive; 
	double life; 
	float color[4]; 
	JParticle(Vector3f &pos, Vector3f &vel, double l, float r=0.9f, float g=0.4f, float b=0.3f, float a=0.5f){
		this->vel = vel;
		this->pos = pos; 
		life = l;
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	}

	bool isDead(){
		return timeAlive >= life; 
	}

	void update(double dt){
		if (isDead()) return;
		pos+= vel * dt / 1000.0;
		timeAlive += dt; 
	} 

	void render(){
		//glColor4fv(color);
		//glVertex3f(pos.x(), pos.y(), pos.z()); 
	}

};

class StaticEmitter{
public: 
	virtual void render() =0 ;
	virtual void update(double dt)=0; 
};

inline bool pIsDead(const JParticle &v){
	return v.isDead();
} 

class Torch : public StaticEmitter{
public:
	list<JParticle> ps;
	Vector3f center;
	Vector3f up; 
	Vector3f norm;
	Vector3f strafe;
	double width;
	double height;   
	double density;
	Torch(Vector3f &c, Vector3f &n, Vector3f &u, double w, double h, double d)
	:StaticEmitter(),center(c),up(u), norm(n), width(w), height(h), density(d) 
	{
		strafe = (up.normalized().cross(norm.normalized()));
	} 
	void render(){
		list<JParticle>::iterator it = ps.begin();
		cout<< ps.size() << endl;
		//glBegin(GL_POINTS)
		while(it != ps.end()){
			(*it).render();
		} 
		//glEnd(); 
	}
	double life(double theta, double u, double v){
		double l = ((sin (u/5 + theta) + sin(u*v + theta) + cos(v+theta))+3.0) / 6.0;
		return l;     
	}
	double color(double theta, double u, double v){
		double c = sin(theta) + sin(u) + cos(v);
		return c; 
	} 

	

	void update(double dt){
		ps.remove_if(pIsDead);
		list<JParticle>::iterator it = ps.begin(); 
		while (it!=ps.end()){
			(*it).update(dt);
			it++;
		}
		int U = density * width;
		int V = density * height;
		double du = width / U;
		double dv = height / U;
		Vector3f minU = center - strafe*(width / 2);
		Vector3f minV = center - norm*(height / 2);
		Vector3f velocity = up * 2; 
		double theta = dt / 1000.0;
		for (int u = 0; u < U; u ++){
			for (int v = 0; v < V; v++){
				Vector3f pos( (minU + strafe * du + minV + norm*dv)); 
				ps.push_back(JParticle(pos, velocity, life(theta, u, v))); 
			}
		} 
		cout<< ps.size() << endl;

	}

};

#endif