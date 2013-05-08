#include "MMath.h" 
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <cmath>

/*===============================
Vec4f Vec3f Vec2f definitions
=================================*/

Matrix4 Quaternian::generateRotationMatrix(){
	GLfloat buffer[][4] =
	{{1.0f-2.0f*y*y -2.0f*z*z, 2.0f*x*y - 2.0f*w*z, 	2.0f*x*z + 2.0f*w*y,0	},
	{ 2.0f*x*y + 2.0f*w*z, 	1.0f-2.0f*x*x - 2.0f*z*z, 	2.0f*y*z - 2.0f*w*x, 0},
	{ 2.0f*x*z - 2.0f*w*y, 	2.0f*w*x + 2.0f*y*z,	1.0f -2.0f*x*x - 2.0f*y*y, 0},
	{ 0, 0, 0, 1}};
	return Matrix4(&buffer[0][0]);
}

GLfloat&  Vec3f::operator[] (const int idx)	{
	switch(idx){
		case 0: return x; 
		case 1: return y;
		case 2: return z;
		default: throw "Array Index out of Bounds";
	}
}

GLfloat& Vec2f::operator[] (const int idx) {
	switch (idx){
		case 0: return x;
		case 1: return y;
		default: throw "Array Index out of Bounds";
	}
}
/*
Matrix Functions
*/

void Matrix4::setRow(int ridx, const GLfloat *r){
	if (ridx > 3 || ridx < 0)
		throw "Matrix index exception";
		
	for(int i = 0; i < 4; i ++)
		data[i][ridx] = *(r+i);
}

Matrix4& Matrix4::operator*=(const Matrix4 &r){
	for (int i = 0; i < 4; i ++){			
		GLfloat nrow[] = {0,0,0,0}; 
			
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < 4;k++)
				nrow[j]+=data[k][i] * r.data[j][k]; 
		}
		setRow(i, nrow); 
	}
	return *this;
}

/**copy and pasted formula from 
	*http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
	*/
GLfloat Matrix4::det(){
		 GLfloat value;
		value =
   data[0][3]*data[1][2]*data[2][1]*data[3][0] - data[0][2]*data[1][3]*data[2][1]*data[3][0] - data[0][3]*data[1][1]*data[2][2]*data[3][0] + data[0][1]*data[1][3]*data[2][2]*data[3][0]+
   data[0][2]*data[1][1]*data[2][3]*data[3][0] - data[0][1]*data[1][2]*data[2][3]*data[3][0] - data[0][3]*data[1][2]*data[2][0]*data[3][1] + data[0][2]*data[1][3]*data[2][0]*data[3][1]+
   data[0][3]*data[1][0]*data[2][2]*data[3][1] - data[0][0]*data[1][3]*data[2][2]*data[3][1] - data[0][2]*data[1][0]*data[2][3]*data[3][1] + data[0][0]*data[1][2]*data[2][3]*data[3][1]+
   data[0][3]*data[1][1]*data[2][0]*data[3][2] - data[0][1]*data[1][3]*data[2][0]*data[3][2] - data[0][3]*data[1][0]*data[2][1]*data[3][2] + data[0][0]*data[1][3]*data[2][1]*data[3][2]+
   data[0][1]*data[1][0]*data[2][3]*data[3][2] - data[0][0]*data[1][1]*data[2][3]*data[3][2] - data[0][2]*data[1][1]*data[2][0]*data[3][3] + data[0][1]*data[1][2]*data[2][0]*data[3][3]+
   data[0][2]*data[1][0]*data[2][1]*data[3][3] - data[0][0]*data[1][2]*data[2][1]*data[3][3] - data[0][1]*data[1][0]*data[2][2]*data[3][3] + data[0][0]*data[1][1]*data[2][2]*data[3][3];
   return value;
	}


