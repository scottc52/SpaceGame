
#include "projectile_particles.h"
//particles using std::list (probably faster than vector, since it has a very high turn-over rate?)

//global variables
Vector3f particleCameraPos;
float distScale = 0.1; // we could pass in field-of-view to calculate this. But probably not worth it.

//Particle class
	Particle::Particle(){}
	Particle::Particle(int lif, Vector3f loc, Vector3f vel, Vector3f accel){
		life = lif;
		lifeMax = lif;
		location = loc;
		velocity = vel;
		acceleration = accel;
	}
	void Particle::setLook(float psize, float *pcolor){
		size = psize;
		color[0] = pcolor[0];
		color[1] = pcolor[1];
		color[2] = pcolor[2];
		color[3] = pcolor[3];
	}
	void Particle::setAccel(Vector3f newAccel){
		acceleration = newAccel;
	}
	void Particle::update(double dt){
		float dts = ((float)dt)/1000;
		location = location + velocity * dts;
		velocity = velocity + acceleration *dts;
		life = life - dt;
		if(location[0] != location[0]){
			cout << "the fuck?";
		}
	}
	void Particle::display(bool drawingGlowPass, bool particleGlow){
		if(life<0) return;
		//draw dot
		glPointSize(size*sizeAttenuation());
		if(drawingGlowPass && !particleGlow){
			//mask glow
			glColor4f(0, 0, 0, color[3]*alphaAttenuation());
		}else{
			glColor4f(color[0], color[1], color[2], color[3]*alphaAttenuation());
		}
		glBegin(GL_POINTS);
			glVertex3f(location[0], location[1], location[2]);
		glEnd();

		//cout << "point drawn: \n" << location << "\n";
	}
	bool Particle::isDead(){
		return life < 0;
	}
	float Particle::sizeAttenuation(){
		return 1.0; // using GL_POINT_DISTANCE_ATTENUATION instead
		/*
		float dist2 = (particleCameraPos - location).squaredNorm();
		return 1.0f/(1.0f+dist2*distScale);
		*/
	}
	float Particle::alphaAttenuation(){
		return ((float)life)/lifeMax;
	}


//helper function
bool isParticleDead(Particle p){
	return p.isDead();
}

//SimpleParticleSystem class
	SimpleParticleSystem::SimpleParticleSystem(){
		location = Vector3f(0.0 , 0.0, 0.0);
		velocity = Vector3f(0.0 , 0.0, 0.0);
		acceleration = Vector3f(0.0 , 0.0, 0.0);
		pCarryOver = 0.0;
	}
	void SimpleParticleSystem::setAccel(Vector3f newAccel){
		acceleration = newAccel;
	}
	void SimpleParticleSystem::update(double dt){
		float dts = ((float)dt)/1000; // time passed in seconds
		location = location + velocity * dts;
		velocity = velocity + acceleration *dts;
		//update all particles
		for(list<Particle>::iterator it = particles.begin(); it != particles.end(); ++it){
			(*it).update(dt);
			/*
			Vector3f newAccel = Vector3f(0, 0, 0);
			(*it).setAccel(newAccel);
			*/
		}
		particles.remove_if(isParticleDead);
		//emit new particles
		float numNewf = rate * dts + pCarryOver;
		pCarryOver = numNewf - floor(numNewf);
		int numNew = floor(numNewf); 
		for(int i = 0; i < numNew; i++){
			//Particle(int lif, Vector3f loc, Vector3f vel, Vector3f accel)
			Vector3f pVelocity = Vector3f(rand(), rand(), rand())/((float)RAND_MAX) - Vector3f(0.5, 0.5, 0.5);
			pVelocity.normalize();
			float speedWig = (float)rand()/(float)RAND_MAX;
			pVelocity = pVelocity * pSpeed * speedWig + pVelOffset;

			Particle newP = Particle(pLife, location, pVelocity, pAccel); //TODO -set correct properties
			Vector3f pWigColorV = Vector3f(pColor[0], pColor[1], pColor[2]);
			pWigColorV = pWigColorV + (Vector3f(rand(), rand(), rand())/((float)RAND_MAX)*2 - Vector3f(1,1,1)) * pColorWig;
			float pWigColor[4] = {pWigColorV[0], pWigColorV[1], pWigColorV[2], pColor[3]};
			newP.setLook(pSize, pWigColor);
			particles.push_back(newP);
		}

	}
	void SimpleParticleSystem::display(bool drawingGlowPass, bool particleGlow){
		//draw all particles
		glEnable( GL_POINT_SMOOTH );
		for(list<Particle>::iterator it = particles.begin(); it != particles.end(); ++it){
			(*it).display(drawingGlowPass, particleGlow);
		}
	}
	bool SimpleParticleSystem::isDead(){
		return (particles.size() <=0);
	}


 //SmokyBullet class
	SmokyBullet::SmokyBullet(){
		deathT = -1;
		t = 0;
		sound = new Sound("sounds/laser.wav");
		sound->Play();
	} //default constructor doesn't set anything
	SmokyBullet::SmokyBullet(Vector3f loc,Vector3f vel, float c0, float c1, float c2, float c3){
		sound = new Sound("sounds/laser.wav");
		sound->Play();
		//emitter has no velocity and acceleration of it's own.
		//its location is decided by the bullet
		location = loc;
		velocity = vel;
		acceleration = Vector3f(0.0, 0.0, 0.0);
		hitB = false;
		hitT = -1;
		t=0;
		//determine up vector, assume it doesn't need to change
		Vector3f yVec = Vector3f(0, 1, 0);
		if(yVec.dot(velocity.normalized()) > 0.1){
			up = yVec;
		}else{
			up = Vector3f(1, 0, 0);
		}

		deathT = 1000000;

		//now set properties that determine movement of the effect
		glow = true;
		hitLife = 1000;
		offset1 = 0;
		tScale1 = 0.06;
		mag1 = 0.03;
		offset2 = 0; // PI/2;
		tScale2 = 0.03;
		mag2 = 0.05;
		hitWig = 0.03;

		//set emittor properties for particlesc++ arr
			pSystem.rate = 200; // particles per second
			pSystem.pLife = 500;
			pSystem.pSpeed = 0.3;
			pSystem.pAccel = Vector3f(0.0, 0.0, 0.0);
			pSystem.pColor[0] = c0;
			pSystem.pColor[1] = c1;
			pSystem.pColor[2] = c2;
			pSystem.pColor[3] = c3;
			pSystem.pColorWig = 0.5;
			pSystem.pSize = 15.0;

	}

	void SmokyBullet::setAccel(Vector3f newAccel){
		acceleration = newAccel;
	}

	void SmokyBullet::update(double dt){
		if(isDead()){ 
			if (sound) {
				delete sound;
				sound = NULL;
			}
			return; 
		}
		t = t+dt;
		float dts = ((float)dt)/1000; // time passed in seconds
		location = location + velocity * dts;
		velocity = velocity + acceleration *dts;
		//update location of emitter and emitter properties
		if(!hitB){
			//flying!
			Vector3f orth1 = velocity.cross(up).normalized();
			Vector3f orth2 = velocity.cross(orth1).normalized();
			pSystem.location = location + (orth1*mag1*cos(t*tScale1 + offset1)) + (orth2*mag2*sin(t*tScale2 + offset1));
			pSystem.pVelOffset = velocity * 0.0; 
		}else{
			if(hitT <= hitLife){
				if(hitT < 0){
					hitT = t;
				}
				//exploding!
				float wigScale = hitWig;
				Vector3f locWiggle = Vector3f(rand(), rand(), rand())/((float)RAND_MAX) - Vector3f(0.5, 0.5, 0.5);
				locWiggle = locWiggle.normalized() * (float)rand()/(float)RAND_MAX;
				pSystem.location = location + locWiggle * wigScale;
				pSystem.pVelOffset = velocity * 0.0;
			}else{
				pSystem.rate =0;
			}
		}
		pSystem.update(dt);
		//cout << "numParticles: " << pSystem.particles.size() << "\n";
		//cout << "bullet location: \n" << location << "\n";
		//cout << "bullet velocity: \n" << velocity << "\n";
	}

	//if hit. call hit() first, then update()
	void SmokyBullet::hit(Vector3f hitLocation){
		hitB = true;
		location = hitLocation;
		velocity = Vector3f(0.0, 0.0, 0.0);
		acceleration = Vector3f(0.0, 0.0, 0.0);
		//change emittor properties
		pSystem.rate = pSystem.rate * 100;
		pSystem.pSpeed = pSystem.pSpeed * 10;
		pSystem.pSize = pSystem.pSize * 2;
		//cout << "hit location: \n" << location << "\n";
	}

	void SmokyBullet::display(Vector3f cameraPos, bool drawGlow){
		if(!isDead()){
			particleCameraPos = cameraPos;
			pSystem.display(drawGlow, glow);
		}
		//cout << "bullet draw\n";
	}
	bool SmokyBullet::isDead(){
		return (hitT > hitLife && pSystem.isDead()) || (t > deathT);
	}

//Slug

Slug::Slug(Vector3f &pos1, Vector3f &velocity1,  float r1, float g1, float b1, float a1) : Projectile(){
	r = r1; 
	g = g1;
	b = b1;
	a = a1;
	position = pos1;
	velocity = velocity1;
	pTimeAlive = 0;
	pTimeSinceRedraw = 0; 

	sound = new Sound("sounds/laser.wav");
	sound->Play();
}

void Slug::update(double dt){
	if (isDead()) {
		if (sound) {
			delete sound;
			sound = NULL;
		}
	}
	Vector3f delta = velocity * ((dt)/1000.0);
	position += delta; 
	pTimeAlive += dt;
	pTimeSinceRedraw += dt;
}

void Slug::hit(Vector3f loc)
{
	return;
}

void Slug::display(Vector3f cam, bool glow){
	if (!isDead()){
		glBegin(GL_LINES);
		glColor4f(r, g, b, a);
		glVertex3f(position[0], position[1], position[2]); 
		Vector3f tmp = position - velocity * ((double)pTimeSinceRedraw)/1000.0; 
		glVertex3f(tmp[0], tmp[1], tmp[2]);
		pTimeSinceRedraw = 0;
		glEnd(); 
	}
}
#define MAX_TIME (4000)
bool Slug::isDead(){
	return (pTimeAlive > MAX_TIME); 
}

double Slug::timeAlive(){
	return pTimeAlive;
}

Vector3f &Slug::getPosition(){
	return position;
}
