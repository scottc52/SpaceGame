#ifndef DEBUG_H
#define DEBUG_H
#include "MMath.h"
void print_Quaternian(const Quaternian &q){ 
 	cout << "Quat -- w: " << q.w << "(x,y,z): (" << q.x << "," 
	<< q.y << "," << q.z << ")" << endl;  	
}

void print_Vec3f(const Vec3f &v){
	cout << "Vec3f -- (x,y,z): (" << v.x << ","
	<< v.y << "," << v.z << ")" << endl;
} 

void print_Vec4f(const Vec4f &v){
	cout << "Vec4f -- (x,y,z,w): (" << v.x << ","
	<< v.y << "," << v.z << "," << v.w <<  ")" << endl;
}

void print_Matrix4(Matrix4 &m){
	cout << "Mat -- {" << endl;
	for (int i = 0; i < 4 ; i++){
		cout << "\t{";	
		for (int j = 0; j<3; j ++){
			cout << m[j][i] << ",";		
		}
		cout << m[3][i] << "}" << endl; 
	}
	cout << "}"<<endl;
}
#endif
