/**
Mesh.h
*/
#ifndef _MESH_H 
#define _MESH_H
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h> 
#include <vector>
#include <cmath>
#include "MMath.h"
#include "EdgeMap.h"
#include "TaskQueue.h" 
using namespace std;

class Face;
class Vertex;
class Mesh;

class Vertex{
private:
	
public:
	vector <unsigned int> faces; 
	Vec3f v;
	Vertex(GLfloat x = 0, GLfloat y= 0, GLfloat z=0){
		v = Vec3f(x, y, z);
		faces = vector<unsigned int>();
	}
	Vertex (const Vec3f &v1){
		v = v1;
		faces = vector<unsigned int>();
	}
	void addFace(unsigned int idx){
		faces.push_back(idx);
	}
}; 

class Face{
private:
	void init(){
		for (int i = 0; i< 3; i++){normals[i]=-1;tex[i] = -1; }
	}
	public:
		int vertices[3];
		int normals[3];
		int tex[3];
		Face(int a, int b, int c){
			vertices[0]=a;	
			vertices[1]=b;
			vertices[2]=c;
			init();
		}
		Face(){};
		void draw(bool flat = false);    	 
};

class SubdivideTask; 
class EdgeMap;

class Mesh{
	private:
		Vec3f computeNormal(Vec3f v); 
		Vec3f evenSmooth(Vertex *v);
		Vec3f oddSmooth(int f1, int f2, int v1, int v2); 
		Vec3f triangleNormal(const Face *f);
		Vec3f generateNormal(Vertex *v);
		void drawFace(Face *f, bool flat=true);
		void addNewFaces(Face *f,  EdgeMap em, Mesh *n, vector<bool> &init);
		Mesh *subdivide(int min, int max, EdgeMap &em);
	protected:
		vector<Face> *faces;
		vector<Vertex> *vertices;
		vector<Vec3f> *normals;
		vector<Vec2f> *texcoords; 
	public: 
		Mesh(){
			vertices = new vector<Vertex>();
			faces = new vector<Face>();
			texcoords = new vector<Vec2f>();
			normals = new vector<Vec3f>();  
		}
		virtual ~Mesh(){
			delete(vertices);
			delete(faces); 
			delete(normals);
			delete(texcoords); 
		}

		//accessors
		inline Face *getFace(int i){return &faces->at(i);}
		inline Vertex *getVertex(int i){return &vertices->at(i);}
		inline int numFaces(){return (int)faces->size();}
		inline int numVertices(){return (int)vertices->size();} 
		friend class SubdivideTask; 
		Mesh *subdivide();
		Mesh *parallelSubdivide(TaskQueue* tq, int branch = 4);
		virtual void draw(bool flat = false);
		void drawWireframe();
		unsigned int addFace(const int *vertices, const int *normals = NULL, const int *t = NULL);
		unsigned int addVertex(const Vec3f &f){ vertices->push_back(Vertex(f)); return vertices->size() -1;}
		unsigned int addFace(Face &f){faces->push_back(f); return faces->size()-1;}
		unsigned int addVertex(Vertex &v){vertices->push_back(v); return vertices->size()-1;}
		unsigned int addNormal(Vec3f &n){ normals->push_back(n); return normals->size()-1;}
		unsigned int addTex(Vec2f &t){texcoords->push_back(t); return texcoords->size()-1;}
		void computeNormals(); 
		//Rigid body assumption
		void computeDeltas(GLfloat normalize = -1.0f);
		static Mesh *loadObj(const char *fname);
};
#endif
