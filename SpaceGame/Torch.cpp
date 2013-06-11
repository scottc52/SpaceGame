/**
Torch.cpp
*/

#include "Torch.h"

void JParticle::update(double dt){
	if (isDead()) return;
	pos+= vel * dt/1000.0;
	timeAlive += dt; 
} 

void JParticle::render(){
	
	double a = (life - timeAlive)/life;
	double a2 = a * a;
	double a3 = a2 * a;  
	glColor4f(color[0]*a, color[1]*a2, color[2]*a3, color[3] * a2);
	glVertex3f(pos.x(), pos.y(), pos.z()); 
}

void Torch::render(){
	list<JParticle>::iterator it = ps.begin();
	glDepthMask( GL_FALSE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_POINT_SMOOTH );
	glPointSize( 20.0f);
	glBegin(GL_POINTS);

	while(it != ps.end()){
		(*it).render();
		it ++;
	} 
	glEnd();
	glDepthMask(GL_TRUE);
	glDisable( GL_BLEND ); 
}

double Torch::life(double theta, double u, double v){
	double a = sin (u/5 + theta);
	double b = sin(u*v + theta/5);
	double c = cos(v/4 + theta);
	double l = (a*a + 1.0*b*b*b + c*c + 1.0) / 4.0;
	l *= 1500;
	return l;     
}
#define PI ((double)3.14159)
#define PPS (30.0) 
void Torch::update(double dt){
	ps.remove_if(jPIsDead);
	list<JParticle>::iterator it = ps.begin(); 
	while (it!=ps.end()){
		(*it).update(dt);
		it++;
	}
	int U = density * width;
	int V = density * height;
	double du = width / ((double)U);
	double dv = height / ((double)U);
	Vector3f minU = center - strafe*(width / 2.0);
	Vector3f minV = center - norm*(height / 2.0);
	Vector3f velocity = up * 3; 
	base += dt;
	int idx = 0; 
	int skip = PPS / dt;
	skip = (skip)? skip: 1.0;
	double theta = base / 100.0;
	for (int u = 0; u < U; u ++){
		for (int v = 0; v < V; v++){
			Vector3f pos( (minU + strafe * (du * (u + v % 2) + du * cos(theta/4 + u)))  + minV + norm * (dv * v + dv * sin(theta/3 + v)) ); 
			ps.push_back(JParticle(pos, velocity, life(theta, u*du*PI, u*du*PI))); 
		}
	} 
}