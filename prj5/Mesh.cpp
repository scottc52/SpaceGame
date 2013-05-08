/**
Mesh.cpp
*/


#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "Mesh.h"
#include "EdgeMap.h"
#include <set>
#include <map>
#include "MMath.h"

#include "TaskQueue.h"

using namespace std; 
/**
Parsing helper functions
*/
#define WS " \n\r\t"
#define FACE_SZ (3)
vector<string*> *split(const string &s, const char *delim){
	string tmp(s);
	unsigned int pos = tmp.find_first_of(delim);
	vector<string*> *v = new vector<string *>(); 
	while (pos < tmp.size()){
		v->push_back(new string(tmp.substr(0, pos)) );
		tmp = tmp.substr(pos+1, string::npos);
		pos=tmp.find_first_of(delim);
	}
	v->push_back(new string(tmp));
	return v;  
} 

inline Vec3f Mesh::triangleNormal(const Face *f){
	Vec3f v0, v1, v2; 
	v0 = getVertex(f->vertices[0])->v;
	v1 = getVertex(f->vertices[1])->v;
	v2 = getVertex(f->vertices[2])->v;
	Vec3f a = v1 - v0; 
	Vec3f b = v2 - v0; 
	Vec3f c = a.cross(b);
	
	Vec3f d = (c.isZero())? zeroVec3f : c.normalize(); 
	return d;
}

typedef bool (*VComp)(const Vertex *, const Vertex *); 

bool comp(const Vertex *v, const Vertex *v2){
	return ((unsigned long) v) < ((unsigned long) v2);
}

inline Vec3f Mesh::generateNormal(Vertex *v){
	int n = v->faces.size(); 
	Vec3f r(0, 0, 0); 
	for (int i = 0; i < n; i++){
		Face * f = getFace(v->faces[i]);
		r += triangleNormal(f); 		
	}
	r*=(1.0f / (GLfloat)n);
	return r;
}

void Mesh::computeNormals(){
	normals->resize(vertices->size()); 
	for (int i = 0; i < numVertices(); i++){
		Vertex *v = getVertex(i); 
		Vec3f n = generateNormal(v);
		Vec3f *p = &normals->at(i); 
		*p = n;  
	}
	for ( int i= 0; i < numFaces(); i++){
		Face *f = getFace(i); 
		for (int j = 0; j < 3; j ++){
			unsigned int v = f->vertices[j];
			f->normals[j] = v;
		}
	}
}

bool parseFaceEntry(const string *toks, int *vert, int *norm, int *tex){
	for (int i = 0; i < FACE_SZ; i ++){
		vector<string *> *l = split(toks[i], "/");
		if (l->size() < 1){
			cerr<< "Error! f field malformed" << endl;
			return false;  
		}
		vert[i] = atoi(l->at(0)->c_str()) -1;
		if (l->size() <= 1){
			norm[i] = tex[i] = -1;
			continue;
		}
		tex[i] = atoi(l->at(1)->c_str()) - 1;
		if (l->size() <= 2){
			norm[i] = -1; 
		}
		norm[i] = atoi(l->at(2)->c_str()) - 1; 
		delete l; 
		
	} 
	return true;
}

#define MAX_LINE 256
Mesh *Mesh::loadObj(const char *fname){
	ifstream objfile (fname);
	if (!objfile.is_open()){
		cerr << "Error: file " << fname << "does not exist" << endl;
		return (NULL); 
	}
	Mesh *mesh = new Mesh(); 
	
	char line[MAX_LINE];

	while(objfile.good()){
		objfile.getline(line,MAX_LINE);
		 
		if (strlen(line) == 0){
			continue;
		}
		char *type;
		type = strtok(line, WS);
		if (!type || strlen(type) == 0 || type[0] == '#'){
			continue; 
		}
		if(strcmp(type, "v") == 0){
			float x, y, z;  
			x = atof(strtok(NULL, WS));
			y = atof(strtok(NULL, WS));
			z = atof(strtok(NULL, WS));
			mesh->addVertex(Vec3f(x, y, z)); 
		}else if(strcmp(type, "f") == 0){
			string toks[FACE_SZ];
			for (int i = 0; i < FACE_SZ; i ++){
				char *tok = strtok(NULL, WS);
				toks[i] =  string(tok) ; 
			}
			int v[3]; 
			int n[3];
			int t[3];
			parseFaceEntry(toks,v ,n, t);

			mesh->addFace(v, n, t);  
		}else if(strcmp(type, "vt") == 0){
			float x, y;  
			x = atof(strtok(NULL, WS));
			y = atof(strtok(NULL, WS));
			Vec2f tex(x,y);
			mesh->addTex(tex); 
		}else if(strcmp(type, "vn") == 0){
			float x, y, z;  
			x = atof(strtok(NULL, WS));
			y = atof(strtok(NULL, WS));
			z = atof(strtok(NULL, WS));
			Vec3f norm(x,y,z);
			mesh->addNormal(norm); 
		}else{
			cerr << "unsupported syntax " << line << endl;  
		}
	} 
	return mesh; 

} 

void Mesh::drawFace(Face *f, bool flat){
	if (!flat || f->normals[0] == -1){
		Vec3f norm = triangleNormal(f);
		glNormal3f(norm.x, norm.y, norm.z);
	}
	for (int i = 0; i < 3; i ++){
		if (f->normals[i] >= 0){
			Vec3f *n = &normals->at(f->normals[i]); 
			glNormal3f(n->x, n->y, n->z);  
		}	
		
		if(f->tex[i] >= 0){
			Vec2f *t = &texcoords->at(f->tex[i]); 
			glTexCoord2f(t->x, t->y); 
		
		}
		Vertex *v = &vertices->at(f->vertices[i]);

		glVertex3f(v->v.x, v->v.y, v->v.z);
	}  
}

void Mesh::drawWireframe(){
	for (int i = 0; i < numFaces(); i++){
		glBegin(GL_LINE_LOOP);
		Face *f = getFace(i);
		drawFace(f);
		glEnd();
	}
	
}

void Mesh::draw(bool flat){ 
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < numFaces(); i++){
			Face *f = getFace(i);
			drawFace(f, flat);
		}
		glEnd();
} 

unsigned int Mesh::addFace(const int *v, const int *n, const int *t){
	Face f; 
	for (int i = 0; i < FACE_SZ; i ++){ 
		f.vertices[i] = v[i];
		getVertex(v[i])->addFace(faces->size());
		int no = (n && n[0] >=0)? n[i] : -1;
		int te = (t && t[0] >=0)? t[i] : -1; 
		f.normals[i] = no; 
		f.tex[i] = te; 
	}
	faces->push_back(f);
	return faces->size() -1;
} 

//==============================
//subdivision -- parallel
//==============================

class SubdivideTask : public Task{
public: 
	Mesh *old;
	EdgeMap *wingedEdge; 
	int min, max;
	Mesh *next;
	SubdivideTask(): Task(){}
	SubdivideTask(Mesh *old1, EdgeMap *we, int min1, int max1):
	  Task(), old(old1), wingedEdge(we), min(min1), max(max1){}
	
	void run(){
		next = old->subdivide(min, max, *wingedEdge); 
	}

	int getType(){
		return TASK_GENERAL;
	}
	~SubdivideTask(){}
};

void mergeHelper(Mesh *base, Mesh *add, vector<bool> &isSet){
	int baseFNum = base->numFaces(); 
	vector<bool> touched(isSet.size(), false); 
	for (int i = 0; i < add->numFaces(); i++){
		Face *f = add->getFace(i);
		base->addFace(*f);	
		for (int j=0; j < 3; j++){
			int v = f->vertices[j];
			if (touched[v]){
				continue; 
			}
			touched[v] = true; 
			vector<unsigned int> *fv2 = &add->getVertex(v)->faces; 
			for (int k = 0; k < (int) fv2->size(); k ++)
				fv2->at(k) += baseFNum;
			if(isSet[v]){
				vector<unsigned int> *fv = &base->getVertex(v)->faces;
				fv->insert(fv->end(), fv2->begin(), fv2->end());
				fv2->clear(); 
			}else{
				*base->getVertex(v) = *add->getVertex(v);
				isSet[v] = true;  
			}

		}  
	}
}

Mesh *merge(SubdivideTask *arr, int len){
	Mesh *val = arr[0].next; 
	vector<bool> isSet(val->numVertices(), false); 
	for (int i = 0; i < val->numFaces(); i++){
		Face *f = val->getFace(i);
		for (int j=0; j < 3; j++){
			int v = f->vertices[j];
			isSet[v] = true;
		}  
	}
	for (int i = 1; i < len; i++){
		Mesh *next =  arr[i].next;
		mergeHelper(val, next, isSet);
		delete next; 
	}  
	return val;
} 

Mesh *Mesh::parallelSubdivide(TaskQueue *tq, int branch){
	int n = numFaces();
	int b = branch;
	cout<< " building winged edges " << endl;
	EdgeMap *em = new EdgeMap(numVertices()); 
	em->putAll(faces); 
	cout << " forking " << endl;    
	while (n / b < 5 && b > 1){
		b /= 2;
	}
	if (branch != b){
		cerr<<"Warning: branch factor reduced from " << branch << " to " << b << endl;
	} 
	int range = n / b + 1; 
	//SubdivideTask tasks[b];
	SubdivideTask *tasks = new SubdivideTask[b];
	for (int i = 0; i < b; i++){
		int min = i * range; 
		if (min >= n) {b=i; break;} 
		int max = ((i + 1) *range);
		if (max > n) max = n;  
		
		tasks[i] = SubdivideTask(this, em, min, max); 
		tq->enqueue(&tasks[i]);
	} 

	for (int i = 0; i < b; i++){
		tasks[i].join(); 
	}
	cout << "merging" << endl;
	Mesh* val = merge(tasks, b);
	delete em;
	delete[] tasks;
	return val;   
}

//==============================
//subdivision -- basic
//==============================

Mesh *Mesh::subdivide(){
	EdgeMap em(vertices->size());
	em.putAll(faces);
	return subdivide (0, numFaces(), em);

}

Mesh *Mesh::subdivide(int min, int max, EdgeMap &em){
	Mesh *n = new Mesh();
	vector<Vertex> *v = n->vertices; 
	int N = numVertices() + em.size();
	v->resize(N);
	vector<bool> init(N, false);
	int range = max - min;    
	for(int i = 0; i < range; i++){
		Face *f = getFace(i+min);
		addNewFaces(f, em, n, init);
	}
	return n;
} 

inline Vec3f Mesh::evenSmooth(Vertex *old){
	set<Vertex *> V;
	int F = old->faces.size(); 
	for (int i = 0; i < F; i++){
		Face *f = getFace(old->faces.at(i)); 
		for(int j = 0; j < FACE_SZ; j ++){
			V.insert(getVertex(f->vertices[j]));
		}
	}
	V.erase(old);
	int n = V.size(); 
	GLfloat beta = (n == 3)? 3.0f/16.0f : 3.0f/8.0f/((GLfloat)n); 
	set<Vertex *>::iterator vit = V.begin();
	Vec3f pnt =  old->v * (GLfloat)(1.0f - (beta * ((GLfloat)n)));
	
	for (;vit != V.end(); vit ++){
		Vertex *neighbor = *vit; 
		pnt += ((neighbor->v * beta)); 
	}
	return pnt;
}

inline Vec3f Mesh::oddSmooth(int f1, int f2, int v1, int v2){
	Vec3f a = getVertex(v1)->v; 
	Vec3f b = getVertex(v2)->v;
	Face *fa = getFace(f1);
	  
	Face *fb = (f2>=0)? getFace(f2) : fa;
	Vec3f v;  
	GLfloat connected = 3.0f / 8.0f;
	GLfloat wing = 1.0f / 8.0f; 
	for (int i = 0; i < 3 ; i ++){
		int tmp = fa->vertices[i];
		if (tmp != v1 && tmp != v2)
			v+= (getVertex(tmp)->v * wing); 
		tmp = fb -> vertices[i]; 
		if (tmp != v1 && tmp != v2){
			v+= (getVertex(tmp)->v * wing);
		}  
	}
	v+=a*connected;
	v+=b*connected; 
	return v;
}

void Mesh::addNewFaces(Face *f, EdgeMap em, Mesh *n, vector<bool> &init){
	
	int ea = (f->vertices)[0];
	int eb = (f->vertices)[1];
	int ec = ((int*)(f->vertices))[2];
	
	if(!init[ea]){
		*(n->getVertex(ea)) = Vertex(evenSmooth(getVertex(ea))) ;
		init[ea] = true;
	}
	if(!init[eb]){
		*(n->getVertex(eb)) = Vertex(evenSmooth(getVertex(eb))) ;
		init[eb] = true;
	}
	if(!init[ec]){
		*(n->getVertex(ec)) = Vertex(evenSmooth(getVertex(ec))) ;
		init[ec] = true;
	}

	Edge_Value abf = em.getEntry(ea, eb);
	Edge_Value bcf = em.getEntry(eb, ec);
	Edge_Value acf = em.getEntry(ea, ec);

	int ab = abf.mid;
	int bc = bcf.mid;
	int ac = acf.mid;

	if(!init[ab]){
		*n->getVertex(ab) = Vertex(oddSmooth(abf.l, abf.r, ea, eb)); 
		init[ab] = true;
	}
	if(!init[bc]){
		*n->getVertex(bc) = Vertex(oddSmooth(bcf.l, bcf.r, eb, ec)); 
		init[bc]=true;
	}
	if(!init[ac]){	
		*n->getVertex(ac) = Vertex(oddSmooth(acf.l, acf.r, ea, ec));
		init[ac] =true; 
	}
			

	Face tmp(ea, ab, ac);
	int f1 = n->addFace(tmp);
	tmp = Face(ab, eb, bc);
	int f2 = n->addFace(tmp);
	tmp = Face(ec, ac, bc);
	int f3 = n->addFace(tmp); 
	tmp = Face(ab, bc, ac);
	int f4 = n->addFace(tmp);

	n->getVertex(ea)->addFace(f1);
	n->getVertex(eb)->addFace(f2);
	n->getVertex(ec)->addFace(f3);
	n->getVertex(ab)->addFace(f1);
	n->getVertex(ab)->addFace(f2);
	n->getVertex(ab)->addFace(f4);
	n->getVertex(ac)->addFace(f1);
	n->getVertex(ac)->addFace(f3);
	n->getVertex(ac)->addFace(f4);
	n->getVertex(bc)->addFace(f2);
	n->getVertex(bc)->addFace(f3);
	n->getVertex(bc)->addFace(f4);
	
}

//two passes over the list of vertices
//compute the center of mass then compute deltas
void Mesh::computeDeltas(GLfloat len){
	Vec3f centroid;
	for(int i = 0; i < numVertices(); i++){
		Vertex *v = getVertex(i);
		centroid+=v->v; 
	}
	centroid *= 1.0f/((GLfloat)vertices->size());
	GLfloat max = -1.0f; 
	for(int i = 0; i < numVertices(); i++){
		Vertex *v = getVertex(i);
		v->v-=centroid;
		if(len != -1.0f){
			GLfloat mag = v->v.norm();
			max=(mag > max)? mag :max;
		}	  
	}
	if(len != -1.0f){ 
		GLfloat scale = 1.0f / max  * len;

		for (int i = 0; i  < numVertices(); i++){
			Vertex *v = getVertex(i);
			v->v*=scale;
		}   
	}
}
