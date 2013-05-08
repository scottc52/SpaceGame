#include "CMSpline.h" 
#include "MMath.h"

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

static GLfloat data[][4] = {
{0, 	1.0f, 	0, 	0},
{-0.5f, 0, 	0.5f,	0},
{1.0f, 	-2.5f, 	2.0f, 	-0.5f},
{-0.5f, 1.5f, 	-1.5f,	0.5f},
};
static Matrix4 CMBasis(&data[0][0]);

  

GLfloat CMSpline::interpolate(GLfloat u){
	return(c0 + c1 * u + c2 *u *u + c3 *u*u*u); 
}

CMSpline CMSpline::splineFromControl(const Vec4f &ctrl){
	return CMSpline(CMBasis * ctrl); 
}

inline VCMSpline create(const Vec3f &p0, const Vec3f &p1,const Vec3f &p2,const Vec3f &p3){ 
	VCMSpline val;  
	val.x = CMSpline::splineFromControl(Vec4f(p0.x, p1.x, p2.x, p3.x));	
	val.y = CMSpline::splineFromControl(Vec4f(p0.y, p1.y, p2.y, p3.y));	
	val.z = CMSpline::splineFromControl(Vec4f(p0.z, p1.z, p2.z, p3.z));	
	return val; 
}

Vec3f VCMSpline::interpolate(GLfloat u){
	Vec3f vec; 
	vec.x = x.interpolate(u);
	vec.y = y.interpolate(u);
	vec.z = z.interpolate(u);
	return vec;
} 

vector<VCMSpline> *VCMSpline::generateSplineCurve(vector<Vec3f> &controlPoints){
	int n = controlPoints.size();
	//compute endpoints by extending real endpoints 	
	Vec3f p_0 = controlPoints[0] - (controlPoints[1]-controlPoints[0]);
	Vec3f p_n = controlPoints[n-1]+(controlPoints[n-1] - controlPoints[n-2]);
	vector<VCMSpline> *val= new vector<VCMSpline>(controlPoints.size() - 1); 
	(*val)[0] = (create(p_0, controlPoints[0], controlPoints[1], controlPoints[2]));
	for (int i = 1; i < ((int)controlPoints.size())-2; i++){
		(*val)[i] = (create(
			controlPoints[i-1], controlPoints[i], 
			controlPoints[i+1], controlPoints[i+2]));	
	}
	(*val)[n-2] = (create(
		controlPoints[n-3], controlPoints[n-2], 
		controlPoints[n-1], p_n));	
	return(val);	

} 
