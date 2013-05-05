#ifndef ANIMATION_H
#define ANIMATION_H
#include "MMath.h"
#include "Mesh.h"
#include "CMSpline.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>  

#define FPSeg 60.0f
#define FPS60 ((GLfloat)60.0f)
#define mSPSeg60 ((GLfloat)(1.0f/FPS60 * FPSeg)*1000.0f)
#define mSPF60 ((GLfloat)(1.0f/FPS60)*1000.0f)
class Animation{
public:
	Mesh *obj; 
	Animation *next;
	vector<VCMSpline> *centroids;
	vector<Quaternian> *orientation;
	int rotationResolution; //millis per segment
	int centroidResolution; //millis per segment 
	Animation():obj(NULL), next(NULL){}
	Animation(Mesh *obj1, vector<VCMSpline> *c=NULL, vector<Quaternian> *ori=NULL,
		GLfloat rR=mSPSeg60, GLfloat cR=mSPSeg60):
		obj(obj1), next(NULL), centroids(c), orientation(ori), 
		rotationResolution(rR), centroidResolution(cR){}; 
	
	//time from the begining of the animation to the
	//time of the moment we want to render
	void drawFrame(unsigned long time);
	static Animation *loadFile(char *fname);  
};


#endif
