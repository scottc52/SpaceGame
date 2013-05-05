#ifndef CMSPLINE_H
#define CMSPLINE_H
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "MMath.h"
#include <vector>

class CMSpline{
public:
	GLfloat c0, c1, c2, c3; 
	CMSpline(GLfloat c01 = 0,GLfloat c11 = 0,GLfloat c21 = 0,GLfloat c31 = 0):c0(c01),c1(c11),c2(c21),c3(c31){}
	CMSpline(const Vec4f &C): c0(C.x), c1(C.y), c2(C.z), c3(C.w){}
	static CMSpline splineFromControl(const Vec4f &ctrl); 
	GLfloat interpolate(GLfloat u);
	
};

class VCMSpline{
public: 
	CMSpline x, y, z;
	VCMSpline(){}
	VCMSpline(const CMSpline &x1, const CMSpline &y1, const CMSpline &z1):x(x1), y(y1), z(z1){}		 
	static vector<VCMSpline> *generateSplineCurve(vector<Vec3f> &controlPoints);
	Vec3f interpolate(GLfloat u);    
};

#endif
