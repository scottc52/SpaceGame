/*++
Homemade Quaternion Class
--*/

#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <vector>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

using namespace std;
using namespace OpenMesh;
class Quaternion{
public:
	float w, x, y, z;

	/*Constructors.....*/
	Quaternion(){w = 1; x = 0; y = 0; z = 0;}
	Quaternion(const Quaternion& q){w = q.w; x = q.x; y = q.y; z = q.z;}
	Quaternion(double alpha, Vec3d n){w = cos(alpha/2); x = n[0]*sin(alpha/2); y = n[1]*sin(alpha/2); z = n[2]*sin(alpha/2);}
	Quaternion(float alpha, Vec3f n){w = cos(alpha/2); x = n[0]*sin(alpha/2); y = n[1]*sin(alpha/2); z = n[2]*sin(alpha/2);}
	Quaternion(double alpha, double n[3]){w = cos(alpha/2); x = n[0]*sin(alpha/2); y = n[1]*sin(alpha/2); z = n[2]*sin(alpha/2);}
	Quaternion(float alpha, float n[3]){w = cos(alpha/2); x = n[0]*sin(alpha/2); y = n[1]*sin(alpha/2); z = n[2]*sin(alpha/2);}
	Quaternion(float _w, float _x, float _y, float _z){w = _w; x= _x; y = _y; z = _z;}
	Quaternion(double _w, double _x, double _y, double _z){w = _w; x = _x; y = _y; z = _z;}
	
	/* Math operators */
	Quaternion operator+(const Quaternion& q){return Quaternion(w+q.w, x+q.x, y+q.y, z+q.z);}
	Quaternion operator-(const Quaternion& q){return Quaternion(w-q.w, x-q.x, y-q.y, z-q.z);}
	Quaternion operator*(const float f)const{return Quaternion(w*f, x*f, y*f, z*f);}
	Quaternion operator*(const double d){return Quaternion(w*d, x*d, y*d, z*d);}
	Quaternion operator/(const float f){return Quaternion(w/f, x/f, y/f, z/f);}

	Quaternion identity(){return Quaternion();}
	double dot(const Quaternion& q) const{return w*q.w + x*q.x + y*q.y + z*q.z;}
	Quaternion cross(Quaternion q){return Quaternion(w*q.w - x*q.x - y*q.y - z*q.z,
													   w*q.x + x*q.w + y*q.z - z*q.y,
													   w*q.y - x*q.z + y*q.w + z*q.x,
													   w*q.z + x*q.y - y*q.x + z*q.w);}

	Quaternion conjugate(){return Quaternion(w, x*-1, y*-1, z*-1);}

	Vec3f rot(Vec3f& v){
		Quaternion q(w, x, y, z);
		Quaternion vq = Quaternion(0, v[0], v[1], v[2]);
		Quaternion result = Quaternion(q.cross(vq.cross(q.conjugate())));
		return Vec3f(result.x, result.y, result.z);
	}
#endif // _QUATERNION_H
};