#include "CMSpline.h"
#include "Mesh.h"
#include "MMath.h"
#include "Animation.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h> 
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

void Animation::drawFrame(unsigned long time){
	
	
	glPushMatrix();
	//transform	
	
	if (centroids != NULL){
		int cs = (time / centroidResolution%centroids->size());
	GLfloat cu = ((float)(time % centroidResolution)) / 1000.0; 
		VCMSpline *s = &((*centroids)[cs]);
		Vec3f nctr = s->interpolate(cu);
		glTranslatef(nctr.x, nctr.y, nctr.z); 
	}

	if (orientation != NULL){
		int rs = (time / rotationResolution%orientation->size());
		GLfloat ru = ((float)(time % rotationResolution))/ 1000.0;
		Quaternian *q1, *q2;
		q1 = &((*orientation)[rs]);
		if (rs ==(signed int) orientation->size()-1)
			rs=-1;
		q2 = &((*orientation)[rs+1]);
		Matrix4 tmp = (Quaternian::SLERP(*q1, *q2, ru)
			.generateRotationMatrix());
		glMultMatrixf(&(tmp.data[0][0]));
		 
	}

	obj->draw(true);  
	glPopMatrix();     	
} 

#define LINE_SZ (256)
#define WS "\f\r\t\n "
Mesh *parseMeshLine(char *line){
	char *s = strtok(line, WS);
	Mesh *m  = Mesh::loadObj(s);
	return m;
}

void parseVec(char *line, vector<Vec3f> &v){
	char *s = strtok(line, WS);
	Vec3f v1; 	
	v1.x = (s)?atof(s):0;
	s = strtok(NULL, WS); 
	v1.y = (s)?atof(s):0;
	s = strtok(NULL, WS);
	v1.z = (s)?atof(s):0; 
	v.push_back(v1);
}

void parseQua(char *line, vector<Quaternian>* vec){
	char *s = strtok(line, WS);
	GLfloat angle = (s)?atof(s):0; 
	Vec3f v; 
	s = strtok(NULL, WS);	
	v.x = (s)?atof(s):0;
	s = strtok(NULL, WS); 
	v.y = (s)?atof(s):0;
	s = strtok(NULL, WS);
	v.z = (s)?atof(s):0; 
	vec->push_back(Quaternian::generateRotation(angle, v));
}

Animation *Animation::loadFile(char *fname){
	ifstream file(fname, ifstream::in); 
	char line[LINE_SZ];
	vector<Quaternian> *rot = new vector<Quaternian>();
	vector<Vec3f> pts; 
	Mesh *mesh = NULL; 
	vector<VCMSpline> *v = NULL;; 
	while (file.good()){
		file.getline(line, LINE_SZ);
		char *ptr = &line[2]; 
		switch (line[0]){
			case '#':continue;
			case 'C': parseVec(ptr, pts); 
				break;
			case 'O': parseQua(ptr, rot); 
				break;
			case 'M': mesh = parseMeshLine(ptr); 
				break; 
			case 'T': 
				break; 
		}
	}
	if (rot->size() < 1){
		delete (rot); rot = NULL;
	}
	if(pts.size() > 1){ 
		v = VCMSpline::generateSplineCurve(pts);
	}
	return new Animation(mesh, v, rot);	
}

