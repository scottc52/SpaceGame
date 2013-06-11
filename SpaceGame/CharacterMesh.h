#ifndef _MESH_H
#define _MESH_H

#include <sstream>
#include <string>
#include <fstream>
#include <exception>
#include <iostream>
#include <vector>
#include <Eigen/Core>

typedef Eigen::Vector3f Point3f;

struct Face {
	Face(int _v1, int _v2, int _v3)
		:v1(_v1), v2(_v2), v3(_v3), 
		 n1(0),   n2(0),   n3(0),
		 t1(0),   t2(0),   t3(0){}

	Face(int _v1, int _v2, int _v3,
		 int _n1, int _n2, int _n3,
		 int _t1, int _t2, int _t3)
		:v1(_v1), v2(_v2), v3(_v3), 
		 n1(_n1), n2(_n2), n3(_n3),
		 t1(_t1), t2(_t2), t3(_t3){}

	//vertex, face, and texture indices
	unsigned int v1, v2, v3,
				 n1, n2, n3,
				 t1, t2, t3;
};

struct TexCoor2D {
	TexCoor2D(float _s, float _t)
		:s(_s), t(_t) {}

	float s, t;
};

struct CharacterMesh {	

public:
	CharacterMesh() {}

	CharacterMesh(const std::string& filename);

	/*
	~Mesh() {
		delete vertexBuf;
		delete indexBuf;
	}
	*/


	std::vector<Point3f> vertices;
	std::vector<Face> faces;
	std::vector<Eigen::Vector3f> normals;
	std::vector<TexCoor2D> texture;

	//unsigned int   vertexDataSize;
	//unsigned char *vertexBuf;
	//unsigned int  *indexBuf;
	//GLuint vertexBufID;
	//GLuint indexBufID;

private:

	void ReadFace(std::stringstream& tokenizer);

};

#endif //_MESH_H
