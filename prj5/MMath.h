#ifndef MMATH_H
#define MMATH_H

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h> 
#include <cmath>

#define PI (3.1415927f)

using namespace std;
/*
forward declarations
*/
class Matrix4;
class Vec4f;
class Vec2f;
class Vec3f;
class Quaternian;


/*==============================
Helpers
================================*/

inline GLfloat degToRad(GLfloat deg);
inline GLfloat radToDeg(GLfloat rad);
/*======================
Class Defs
======================*/

class Vec2f{
	public: 
	GLfloat x, y;
	Vec2f(const GLfloat &x1, const GLfloat &y1): x(x1), y(y1){} 
	GLfloat &operator[] (const int idx);
};

class Vec3f{
	public:
		GLfloat x,y,z;
		inline Vec3f(const GLfloat &x1 = 0.0f, const GLfloat &y1 = 0.0f,
			const GLfloat &z1 = 0.0f): 
				x(x1), y(y1), z(z1)
		{}  
		GLfloat& operator[] (const int idx);

		#define THRESH (0.0000001f) 	
		inline bool isZero(){
			return (x < THRESH && y < THRESH && z < THRESH);
		}
		inline Vec3f cross(Vec3f &v){
			GLfloat a, b, c; 
			a = y * v.z - z * v.y;
			b = z * v.x - x * v.z;
			c = x * v.y - y * v.x;
			return (Vec3f(a,b,c));
		}

		inline Vec3f& operator-= (const Vec3f &v){
			x-=v.x; y-=v.y; z-=v.z;
			return *this;
		}
		
		inline Vec3f& operator+=(const Vec3f &v){
			x+=v.x; y+= v.y; z += v.z;  
			return *this;  
		}

		inline Vec3f& operator*=(const GLfloat &s){
			x = x*s; y = y*s; z = z*s; 
			return *this; 
		}

		inline GLfloat norm(){
			GLfloat sqrsum = (x*x + y*y + z*z);
			return GLfloat(sqrt(sqrsum));
		}

		inline Vec3f& normalize(){
			(*this)*=(1.0f/this->norm());
			return *this;  
		}  
};

const Vec3f zeroVec3f = Vec3f(0,0,0);

inline GLfloat operator*(const Vec3f &l, const Vec3f &v){
	return l.x*v.x+l.y*v.y+l.z*v.z;
}

inline Vec3f operator-(Vec3f l, const Vec3f &r){
	l-=r;
	return l;
}

inline Vec3f operator*(Vec3f l, const GLfloat &r){
	l *= r;
	return l; 
}

inline Vec3f operator+ (Vec3f l, const Vec3f &r){
	l+=r; 
	return l; 
}

class Vec4f{
	public:
		GLfloat x,y,z,w;
		Vec4f(const GLfloat &x1 = 0.0f, const GLfloat &y1 = 0.0f,
			const GLfloat &z1 = 0.0f, const GLfloat &w1 = 1.0f): 
				x(x1), y(y1), z(z1), w(w1){}
		Vec4f(const Vec3f& v, GLfloat w1 = 1): x(v.x), y(v.y), z(v.z), w(w1){}
		GLfloat& operator[] (const int idx);
		
		inline Vec4f cross(Vec4f &v){
			GLfloat a, b, c; 
			a = y * v.z - z * v.x;
			b = z * v.x - x * v.z;
			c = x * v.y - y * v.x;
			return (Vec4f(a,b,c));
		}

		Vec4f& operator-= (const Vec4f &v){
			x-=v.x; y-=v.y; z-=v.z; w-=v.w;
			return *this;
		}
		
		Vec4f& operator+=(const Vec4f &v){
			x+=v.x; y+= v.y; z += v.z;  
			return *this;  
		}

		Vec4f& operator*=(const GLfloat &s){
			x = x*s; y = y*s; z = z*s;
			return *this; 
		}

		GLfloat norm(){
			GLfloat sqrsum = (x*x + y*y + z*z + w*w);
			return GLfloat(sqrt(sqrsum));
		}

		Vec4f& normalize(){
			(*this)*=(1.0f/this->norm());
			return *this;  
		}  
};

inline GLfloat operator*(const Vec4f &l, const Vec4f &v){
	return l.x*v.x+l.y*v.y+l.z*v.z;
}

inline Vec4f operator-(Vec4f l, const Vec4f &r){
	l-=r;
	return l;
}

inline Vec4f operator*(Vec4f l, const GLfloat &r){
	l *= r;
	return l; 
}

inline Vec4f operator+ (Vec4f l, const Vec4f &r){
	l+=r; 
	return l; 
}


class Quaternian{
private: 
public:
	GLfloat x, y, z, w;
	inline Quaternian(GLfloat x1=0,GLfloat y1=0,GLfloat z1=0,GLfloat w1=0);
	inline Quaternian(const Vec3f &v, GLfloat w1=0):x(v.x), y(v.y), z(v.z), w(w1){} 
	inline Quaternian cross(Quaternian rhs);
	inline GLfloat dot(const Quaternian &rhs);
	static inline Quaternian Identity(){return Quaternian(0,0,0,1.0f);};
	static Quaternian generateRotation(GLfloat angle ,const Vec3f &axis);
	static Quaternian SLERP(Quaternian q1, Quaternian q2, GLfloat u);
	Matrix4 generateRotationMatrix(); 
	inline Quaternian rot(const Vec3f& vector);
	inline Quaternian rot(const Quaternian &vec); 
	inline Quaternian conj(){return Quaternian(-x, -y, -z, w);}
	inline Quaternian neg(){return Quaternian(-x, -y, -z, -w);}	
	inline Vec3f real(); 
	inline Quaternian& operator+=(const Quaternian &rhs); 
	inline Quaternian& normalize();
	inline Quaternian& scale(GLfloat s); 
};

inline Quaternian operator+ (Quaternian l, const Quaternian &r); 

class Matrix4{
public: 
	GLfloat data[4][4]; 

	inline Matrix4(const GLfloat *data);

	Matrix4& operator*=(const Matrix4 &r);

	inline void setRow(int ridx, const GLfloat *r);

	inline GLfloat *operator[](int idx);

	/**copy and pasted formula from 
	*http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
	*/
	GLfloat det();

	Matrix4 inv(){
		throw "Not yet implemented.";
	}

};

inline Matrix4 operator*(Matrix4 lhs, const Matrix4 &rhs);

inline Vec4f operator*(const Matrix4 &lhs, Vec4f rhs);

#include "MMath.inl"

#endif 
