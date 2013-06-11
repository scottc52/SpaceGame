#include "windows.h"
#include <gl/glew.h>
#include "gl/GL.h"
#include <GL/glu.h>
#include <gl/freeglut.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>


#undef min
#undef max
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/reader/OBJReader.hh>
using namespace OpenMesh;

unsigned int numWalls;

//// EigenIncludes
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

using namespace std;
using namespace Eigen;

int win_width, win_height;
double scale;

int currentTime, prevTime, startTime;
bool random = false;
float maxV = 12;
float maxR = 360;
float tier0Scale = 1.3;

#define PHYSICS_DELTA_H (0.001);

int numFrames = 0;
// Stored mouse position for camera rotation, panning, and zoom.
int gPreviousMouseX = -1;
int gPreviousMouseY = -1;
int gMouseButton = -1;
Vec3f mCameraTranslation;
float mCameraAzimuth;
float mCameraElevation;
///////////////////


float tolerance = 1e-4;

void ConstructBox(vector<Vec3f>& v, Vec3f& UR, Vec3f& BL);
void ConstructDrawBox(vector<Vec3f>& v, Vec3f& UR, Vec3f& BL);

typedef PolyMesh_ArrayKernelT<>  MyMesh;
#define LOAD_IN_FILE ("LoadIn.txt")
Vec3f gravity(0,0,0);
struct Object{
	bool isSolid;
	bool isMoveable;
	Vec3f position;
	MyMesh mesh;
	Vec3f velocity;
	Vec3f CenterOfMass;
	Vec4f QRotation;
	float angularVelocity;
	GLdouble mass;
	float elasticity;
	GLfloat color[3];
	vector<Vec3f> BoxPoints;
	vector<Vec3f> BoxEdges;
	vector<Object*> collided0;
	vector<Object*> actualCollision;
	float minInterpolate;
	vector<Vec3f> pointOfContact;
	vector<Vec3f> contactNormal;
	vector<Vec3f> generalForces;
	Vec3f netImpulse;
	float momentOfInertia;
	unsigned int index;
	Vec3f netAImpulse;
	Object(){
		elasticity = 1.f;
		pointOfContact.clear();
		position = Vec3f(0,0,2);
		velocity = Vec3f(0,0,0);
		CenterOfMass = Vec3f(0,0,0);
		netImpulse = Vec3f(0,0,0);
		netAImpulse = Vec3f(0,0,0);
		mass = 0;
		minInterpolate = 1.0;
		isSolid = true;
		isMoveable = true;
		index = 0;
		angularVelocity = 0;
		QRotation = Vec4f(0,1,0,0);
		color[0] = GLfloat(rand()) / GLfloat(RAND_MAX);
		color[1] = GLfloat(rand()) / GLfloat(RAND_MAX);
		color[2] = GLfloat(rand()) / GLfloat(RAND_MAX);
	}
};
vector <Object> meshes;

void ConstructBoxEdges(vector<Vec3f>& v, Vec3f& UR, Vec3f& BL){
	v.push_back(UR);
	v.push_back(Vec3f(BL[0], UR[1], UR[2]));
	v.push_back(Vec3f(BL[0], UR[1], UR[2]));
	v.push_back(Vec3f(BL[0], UR[1], BL[2]));
	v.push_back(Vec3f(BL[0], UR[1], BL[2]));
	v.push_back(Vec3f(UR[0], UR[1], BL[2]));
	v.push_back(Vec3f(UR[0], UR[1], BL[2]));
	v.push_back(UR);
	v.push_back(UR);
	v.push_back(Vec3f(UR[0], BL[1], UR[2]));
	v.push_back(Vec3f(BL[0], UR[1], UR[2]));
	v.push_back(Vec3f(BL[0], BL[1], UR[2]));
	v.push_back(Vec3f(BL[0], UR[1], BL[2]));
	v.push_back(BL);
	v.push_back(Vec3f(UR[0], UR[1], BL[2]));
	v.push_back(Vec3f(UR[0], BL[1], BL[2]));
	v.push_back(Vec3f(UR[0], BL[1], UR[2]));
	v.push_back(Vec3f(BL[0], BL[1], UR[2]));
	v.push_back(Vec3f(BL[0], BL[1], UR[2]));
	v.push_back(BL);
	v.push_back(BL);
	v.push_back(Vec3f(UR[0], BL[1], BL[2]));
	v.push_back(Vec3f(UR[0], BL[1], BL[2]));
	v.push_back(Vec3f(UR[0], BL[1], UR[2]));
}

void ConstructBox(vector<Vec3f>& v, Vec3f& UR, Vec3f& BL){
	v.push_back(UR);
	v.push_back(Vec3f(UR[0], UR[1], BL[2]));
	v.push_back(Vec3f(BL[0], UR[1], BL[2]));
	v.push_back(Vec3f(BL[0], UR[1], UR[2]));
	v.push_back(Vec3f(BL[0], BL[1], UR[2]));
	v.push_back(Vec3f(UR[0], BL[1], UR[2]));
	v.push_back(Vec3f(UR[0], BL[1], BL[2]));
	v.push_back(BL);
}

void ComputeCenterOfMassAndBBAndMomentOfInertia(){
	for(unsigned int m = 0; m < meshes.size(); m++){
		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float minZ = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;
		float maxZ = -FLT_MAX;
		for(MyMesh::VertexIter vi = meshes[m].mesh.vertices_begin(); vi != meshes[m].mesh.vertices_end(); ++vi){
			Vec3f pt = meshes[m].mesh.point(vi.handle());
			minX = min(minX, pt[0]);
			maxX = max(maxX, pt[0]);
			minY = min(minY, pt[1]);
			maxY = max(maxY, pt[1]);
			minZ = min(minZ, pt[2]);
			maxZ = max(maxZ, pt[2]);
		}
		Vec3f UR(maxX, maxY, maxZ);
		Vec3f BL(minX, minY, minZ);
		ConstructBox(meshes[m].BoxPoints, UR, BL);
		ConstructBoxEdges(meshes[m].BoxEdges, UR, BL);
		float TotalArea = 0;
		for(MyMesh::VertexIter vit = meshes[m].mesh.vertices_begin(); vit != meshes[m].mesh.vertices_end(); ++vit){
			float numberVertices  = 0;
			Vec3f centroid(0,0,0);
			float faceArea = 0;
			meshes[m].CenterOfMass+= meshes[m].mesh.point(vit.handle())/meshes[m].mesh.n_vertices();
		}
		float MomentOfInertia = 0;
		for(MyMesh::VertexIter it = meshes[m].mesh.vertices_begin(); it!= meshes[m].mesh.vertices_end(); ++it){
			MomentOfInertia += ((Vec3f(meshes[m].mesh.point(it.handle())) - meshes[m].CenterOfMass) * (Vec3f(meshes[m].mesh.point(it.handle())) - meshes[m].CenterOfMass)).sqrnorm();
		}
		MomentOfInertia *= meshes[m].mass;
		meshes[m].momentOfInertia = MomentOfInertia;
	}
}

///////////////////////////////////////////////////////////
// LOAD IN MESH
///////////////////////////////////////////////////////////
//Loaders, etc.
void LoadInMeshes(){
	fstream mFile(LOAD_IN_FILE);
	if(!mFile.good()){
		cout<<"Problem getting file."<<endl;
		cin.ignore(1);
		exit(1);
	}
	string line;
	bool nWalls = false;
	unsigned int counter = 0;
	while(!mFile.eof()){
		getline(mFile, line);
		if(!nWalls){
			numWalls = atoi(line.c_str());
			nWalls = true;
			continue;
		}
		if(line[0] == '#') continue;
		vector<string> parsed;
		istringstream pLine(line);
		copy(istream_iterator<string>(pLine),istream_iterator<string>(), back_inserter<vector<string> >(parsed));
		Object o;
		if(parsed.size() > 1){
			//if(!random || counter< numWalls){
			o.position = Vec3f(atof(parsed[1].c_str()), atof(parsed[2].c_str()), atof(parsed[3].c_str()));
			/*}else{
			float x = float((rand()%12) * ((rand()%2)-1));
			float y = float((rand()%12) * ((rand()%2)-1));
			float z = float(-16 + (rand()% 6));
			Vec3f p(x, y, z);
			o.position = p;
			}*/
		}
		if(parsed.size() > 4){
			if(!random || counter < numWalls){
				o.velocity = Vec3f(atof(parsed[4].c_str()), atof(parsed[5].c_str()), atof(parsed[6].c_str()));
			}else{
				float x = float(rand()%(int)(maxV) * ((rand()%2)-1));
				float y = float(rand()%(int)(maxV) * ((rand()%2)-1));
				float z = float(rand()%(int)(maxV) * ((rand()%2)-1));
				Vec3f v(x, y, z);
				o.velocity = v;
			}
		}
		if(parsed.size() > 7){
			if(!random || counter < numWalls)	o.mass = atof(parsed[7].c_str());
			else o.mass = 10.f* float((rand() - RAND_MAX/2)*2)/float(RAND_MAX);
		}
		if(parsed.size() > 8){
			o.QRotation = Vec4f(atof(parsed[8].c_str()), atof(parsed[9].c_str()), atof(parsed[10].c_str()), atof(parsed[11].c_str()));
		}
		if(parsed.size()>12){
			if(!random || counter < numWalls){
				o.angularVelocity = atof(parsed[12].c_str());
			}else{
				float angle = float(rand()%360);
				o.angularVelocity = angle;
			}
		}
		OpenMesh::IO::Options ropt;
		o.mesh.request_face_normals();
		o.mesh.request_vertex_normals();
		ropt += IO::Options::VertexNormal;
		if (!IO::read_mesh(o.mesh,parsed[0],ropt)) 
		{
			cerr << "read error\n";
			cin.ignore(1);
			exit(1);
		}

		for(MyMesh::FaceIter fit = o.mesh.faces_begin(); fit != o.mesh.faces_end(); ++fit){
			o.mesh.calc_face_normal(fit.handle());
		}
		o.index = meshes.size();
		meshes.push_back(o);
		counter ++;
	}
	ComputeCenterOfMassAndBBAndMomentOfInertia();
}


Vec3f ToOMVector(const Vector3f& a){
	return Vec3f(a.x(), a.y(), a.z());
}

Vector3f ToEigenVector(const Vec3f& a){
	return Vector3f(a[0], a[1], a[2]);
}

/////////////////////////////////////////////////////////
//
// FPS
//
/////////////////////////////////////////////////////////

void ComputeFPS(){
	prevTime = currentTime;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	numFrames ++;
	int tTime = currentTime - startTime;
	if(tTime > 1000){
		startTime = currentTime;
		cout<<"FPS: "<<double(numFrames)<<endl;
		numFrames = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
// TRANSFORM COORDINATES
//
///////////////////////////////////////////////////////////////////////////////////////

void GetTransformedPoint(Vec3f& pt, Object* obj, float dt, float scale = 1.f, float inter = 1.0){
	/*glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	Vec3f newPos = obj->position + obj->velocity*dt*inter;
	Vec4f newRot(obj->QRotation[0] + obj->angularVelocity[0]*dt*inter, obj->QRotation[1],  obj->QRotation[2],  obj->QRotation[2]);
	glTranslatef(newPos[0], newPos[1], newPos[2]);
	glRotatef(newRot[0], newRot[1], newRot[2], newRot[3]);
	float modelMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelMat);
	Matrix4f m4(modelMat);
	glPopMatrix();
	Vector4f toCoM = Vector4f(obj->CenterOfMass[0], obj->CenterOfMass[1], obj->CenterOfMass[2], 1);
	toCoM = m4*toCoM;
	pt = Vec3f(toCoM.x(), toCoM.y(), toCoM.z());
	*/
	Vector3f position = ToEigenVector(obj->position);
	Vector3f velocity = ToEigenVector(obj->velocity);
	float angle = obj->QRotation[0];
	angle += obj->angularVelocity*dt*inter;
	Vector3f newPos = position + velocity*dt*inter;
	Vector3f axis(obj->QRotation[1], obj->QRotation[2], obj->QRotation[3]);
	axis.normalize();
	Translation3f move =Translation3f(newPos);
	angle = angle*2*M_PI/360;
	AngleAxisf turn = AngleAxisf(angle, axis);
	Vector3f p(pt[0], pt[1], pt[2]);
	p = p*scale;
	p = turn*p;
	p = move*p;
	pt = Vec3f(p.x(), p.y(), p.z());
}


void UpdateCoords(vector<Vec3f>& v, Object* o, float dt, float scale = 1.f, float inter = 1.f){
	for(unsigned int boxV = 0; boxV < v.size(); boxV++){
		GetTransformedPoint(v[boxV],o, dt, scale, inter);
	}
}

int MathSign(const float& v){
	if( v == 0) return 0;
	else return v/abs(v);
}
////////////////////////////////////////////////////////////////////////////////
//
// SIMPLEX STRUCT
//
////////////////////////////////////////////////////////////////////////////////
struct Simplex{
private: vector<Vec3f> data, dataA, dataB;

public: 

	Simplex(){data.clear(); dataA.clear(); dataB.clear();}

	const Vec3f atA(unsigned int i){
		return dataA[i];
	}

	const Vec3f atB(unsigned int i){
		return dataB[i];
	}

	const Vec3f at(unsigned int i){
		return data[i];
	}

	Vec3f operator[](const unsigned int i) const{
		return data[i];
	}

	void AddAandB(Vec3f& a, Vec3f& b){
		dataA.push_back(a);
		dataB.push_back(b);
	}

	void PopAandB(){
		dataA.pop_back();
		dataB.pop_back();
	}

	void SetAandB(unsigned int i, Vec3f& a, Vec3f& b){
		dataA[i] = a;
		dataB[i] = b;
	}

	void SetToSimplex(Simplex& s){
		data.clear();
		for(unsigned int i = 0; i< s.GetSize(); i++){
			data.push_back(s[i]);
			dataA.push_back(s.atA(i));
			dataB.push_back(s.atB(i));
		}
	}

	void Add(const Vec3f& E){
		data.push_back(E);
	}
	unsigned int GetSize(){
		return data.size();
	}
	void clearSimplex(){
		data.clear();
		dataA.clear();
		dataB.clear();
	}
	bool alreadyIn(Vec3f& v){
		for(unsigned int i = 0; i< data.size(); i++){
			if(data[i] == v) return true;
		}
		return false;
	} 
	void DeleteNonClosestIn(){
		assert(data.size() > 1);
		if(data.size() == 2){ //Line
			Vector3f A = ToEigenVector(data[1]);
			Vector3f B = ToEigenVector(data[0]);
			if((-A).dot(B-A) <= 0){ //A is closest
				data[0] = data[1];
				data.pop_back();
				dataA[0] = dataA[1];
				dataB[0] = dataB[1];
				dataA.pop_back();
				dataB.pop_back();
			}else if((-B).dot(A-B) <= 0){ //B is closest
				data.pop_back();
				dataA.pop_back();
				dataB.pop_back();
			}else{ //AB is closest
				return;
			}

		}else if(data.size() == 3){ //Triangle
			Vector3f A = ToEigenVector(data[2]);
			Vector3f B = ToEigenVector(data[1]);
			Vector3f C = ToEigenVector(data[0]);
			Vector3f AB = B -A;
			Vector3f AC = C- A;
			Vector3f BC = C-B;
			Vector3f Nab = AC.cross(AB).cross(AB);
			Vector3f Nac = AB.cross(AC).cross(AC);
			Vector3f Nbc = (-AB).cross(BC).cross(BC);

			if(AB.dot(-A) <= 0 && AC.dot(-A)<=0){ //A is closest;
				data[0] = data[2];
				dataA[0] = dataA[2];
				dataB[0] = dataB[2];
				data.pop_back();
				data.pop_back();
				dataA.pop_back();
				dataA.pop_back();
				dataB.pop_back();
				dataB.pop_back();
			}else if ((-AB).dot(-B) <=0 && BC.dot(-B)<=0){//B is closest
				data[0] = data[1];
				dataA[0] = dataA[1];
				dataB[0] = dataB[1];
				data.pop_back();
				data.pop_back(); 
				dataA.pop_back();
				dataA.pop_back();
				dataB.pop_back();
				dataB.pop_back();
			}else if((-AC).dot(-C)<= 0 && (-BC).dot(-C) <=0){//C is closest
				data.pop_back();
				data.pop_back();
				dataA.pop_back();
				dataA.pop_back();
				dataB.pop_back();
				dataB.pop_back();
			}else if(Nab.dot(-A)>=0 && AB.dot(-A) > 0){//AB is closest
				data[0] = data[1];//BBA
				data[1] = data[2];//BAA
				data.pop_back();//BA
				dataA[0] = dataA[1];
				dataA[1] = dataA[2];
				dataB[0] = dataB[1];
				dataB[1] = dataB[2];
				dataA.pop_back();
				dataB.pop_back();
			}else if(Nac.dot(-A) >=0 && AC.dot(-A) > 0){//AC is closest
				data[1] = data[2]; //CAA
				data.pop_back();
				dataA[1] = dataA[2];
				dataB[1] = dataB[2];
				dataA.pop_back();
				dataB.pop_back();
			}else if(Nbc.dot(-B) >=0 && BC.dot(-B) > 0){//BC is closest
				data.pop_back();
				dataA.pop_back();
				dataB.pop_back();
			}else{
				return;
			}
		}else{ //Tetrahedron
			unsigned int posPiece1, posPiece2, posPiece3;
			Vector3f A = ToEigenVector(data[3]);
			Vector3f B = ToEigenVector(data[2]);
			Vector3f C = ToEigenVector(data[1]);
			Vector3f D = ToEigenVector(data[0]);
			Vector3f AO = -A;
			Vector3f BO = -B;
			Vector3f CO = -C;
			Vector3f DO = -D;
			Vector3f AB = B-A;
			Vector3f AC = C-A;
			Vector3f AD = D-A;
			Vector3f Nabc = AB.cross(AC);
			Vector3f Nabd = AB.cross(AD);
			Vector3f Nacd = AC.cross(AD);
			Vector3f Nbcd = (C-B).cross(D-B);
			if((Nabc.dot(AO)*(Nabc.dot(AD))<0)){///ABC to check
				posPiece1 = 1;
				posPiece2 = 2;
				posPiece3 = 3;
			}else if(Nabd.dot(AO) * (Nabd.dot(AC) < 0)){ //ABD to check
				posPiece1 = 0;
				posPiece2 = 2;
				posPiece3 = 3;
			}else if(Nacd.dot(AO)*Nacd.dot(AB)<0){//ACD to check
				posPiece1 = 0;
				posPiece2 = 1;
				posPiece3 = 3;
			}else if(Nbcd.dot(BO) * Nbcd.dot(A-B) < 0){//BCD to check
				posPiece1 = 0;
				posPiece2 = 1;
				posPiece3 = 2;
			}else{ return;}

			Vector3f TriC = ToEigenVector(data[posPiece1]);
			Vector3f TriB = ToEigenVector(data[posPiece2]);
			Vector3f TriA = ToEigenVector(data[posPiece3]);
			Vec3f Ac = dataA[posPiece1];
			Vec3f Ab = dataA[posPiece2];
			Vec3f Aa = dataA[posPiece3];
			Vec3f Bc = dataB[posPiece1];
			Vec3f Bb = dataB[posPiece2];
			Vec3f Ba = dataB[posPiece3];
			Vector3f TriAO = -TriA;
			Vector3f TriBO = -TriB;
			Vector3f TriCO = -TriC;
			Vector3f TriAB = TriB-TriA;
			Vector3f TriAC = TriC-TriA;
			Vector3f TriBC = TriC-TriB;
			Vector3f TriNab = TriAC.cross(TriAB).cross(TriAB);
			Vector3f TriNac = TriAB.cross(TriAC).cross(TriAC);
			Vector3f TriNbc = (TriA-TriB).cross(TriBC).cross(TriBC);

			if(TriAB.dot(TriAO) <= 0 && TriAC.dot(TriAO) <= 0){ //A is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ToOMVector(TriA));
				dataA.push_back(Aa);
				dataB.push_back(Ba);
			}else if((TriA- TriB).dot(TriBO) <= 0 && TriBC.dot(TriBO)<=0){//B is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ToOMVector(TriB));
				dataA.push_back(Ab);
				dataB.push_back(Bb);
			}else if((TriA-TriC).dot(TriCO) <= 0 && (TriB-TriC).dot(TriCO) <= 0){ //C is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ToOMVector(TriC));
				dataA.push_back(Ac);
				dataB.push_back(Bc);
			}else if(TriNab.dot(TriAO)>=0 && TriAB.dot(TriAO) > 0){//AB is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ToOMVector(TriB));
				data.push_back(ToOMVector(TriA));
				dataA.push_back(Ab);
				dataA.push_back(Aa);
				dataB.push_back(Bb);
				dataB.push_back(Ba);
			}else if(TriNac.dot(TriAO) >= 0 && TriAC.dot(TriAO) > 0){ //AC is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ToOMVector(TriC));
				data.push_back(ToOMVector(TriA));
				dataA.push_back(Ac);
				dataA.push_back(Aa);
				dataB.push_back(Bc);
				dataB.push_back(Ba);
			}else if(TriNbc.dot(TriBO) >= 0 && TriBC.dot(TriBO) > 0){//BC closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ToOMVector(TriC));
				data.push_back(ToOMVector(TriB));
				dataA.push_back(Ac);
				dataA.push_back(Ab);
				dataB.push_back(Bc);
				dataB.push_back(Bb);
			}else{//ABC closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ToOMVector(TriC));
				data.push_back(ToOMVector(TriB));
				data.push_back(ToOMVector(TriA));
				dataA.push_back(Ac);
				dataA.push_back(Ab);
				dataA.push_back(Aa);
				dataB.push_back(Bc);
				dataB.push_back(Bb);
				dataB.push_back(Ba);
			}
		}
	}


};

//////////////////////////////////////////////////////////////////////////////////////////////
////
//// SUPPORT FUNCTION
////
/////////////////////////////////////////////////////////////////////////////////////////////
//
//
Vec3f Support(vector<Vec3f>& A, vector<Vec3f>& B, Vec3f v, Simplex& P){
	Vector3f d = ToEigenVector(v);
	double farthestADir = ToEigenVector(A[0]).dot(d);
	unsigned int farthestAIndex = 0;
	for(unsigned int i = 1; i<A.size(); i++){
		float newDot = ToEigenVector(A[i]).dot(d);
		if(newDot > farthestADir){
			farthestADir = newDot;
			farthestAIndex = i;
		}
	}
	d = -d;
	double farthestBDir = ToEigenVector(B[0]).dot(d);
	unsigned int farthestBIndex = 0;
	for(unsigned int i = 1; i<B.size(); i++){
		float newDot = ToEigenVector(B[i]).dot(d);
		if(newDot > farthestBDir){
			farthestBDir = newDot;
			farthestBIndex = i;
		}
	}
	float D = (ToEigenVector(-A[farthestAIndex])).dot(ToEigenVector(B[farthestBIndex]) - ToEigenVector(A[farthestAIndex]));
	P.AddAandB(A[farthestAIndex], B[farthestBIndex]);
	return (A[farthestAIndex]-B[farthestBIndex]);
}
///////////////////////////////////////////////////////////////////
// CLOSEST IN
///////////////////////////////////////////////////////////////////
Vec3f ClosestIn(Simplex& P){
	assert(P.GetSize() > 0);
	if(P.GetSize() == 1) return P.at(0);
	else if(P.GetSize() == 2){ //We have a line.
		Vector3f AO = ToEigenVector(-P.at(1));
		Vector3f AB = ToEigenVector(P.at(0) - P.at(1));
		Vector3f BO = ToEigenVector(-P.at(0));
		float v = (AB.dot(AO))/(AB.dot(AO) + (-AB).dot(BO));
		return (P.at(1)*(1-v) + (P.at(0))*v);
	}else{//We have a triangle
		Vector3f A = ToEigenVector(P.at(2));
		Vector3f B = ToEigenVector(P.at(1));
		Vector3f C = ToEigenVector(P.at(0));
		Vector3f AB = B-A;
		Vector3f AC = C-A;
		Vector3f N = AB.cross(AC);
		Vector3f Nab = N.cross(AB);
		Vector3f Nac = N.cross(AC);

		float v = (-A).dot(Nac)/((AB).dot(Nac));
		float w = (-A).dot(Nab)/((AC).dot(Nab));
		float u = 1-v-w;

		return (P.at(2)*u + P.at(1)*v + P.at(0)*w);
	}
}



Vec3f GJKDistance(vector<Vec3f>&A, vector<Vec3f>&B, Simplex& P){
	P.clearSimplex();
	Vec3f v= Support(A, B, A[0] - B[0],P);
	P.Add(v);
	v = ClosestIn(P);

	float lastDist = FLT_MAX;
	Simplex lastP;
	lastP.SetToSimplex(P);
	Vec3f lastV = v;
	float epsilon = 0.1;

	while(true){
		float dist = v.norm();
		Vec3f w = Support(A, B, -v, P);
		float f = dist - (dist - w.norm());
		if(f<= tolerance*dist || dist<tolerance){
			return v;
		}if(lastDist-dist<= epsilon*lastDist){
			P.SetToSimplex(lastP);
			return lastV;
		}else{
			lastP.SetToSimplex(P);
			lastV = v;
		}
		if(P.alreadyIn(w))
			return v;
		if(ToEigenVector(v).dot(ToEigenVector(w)) > 0)return v;
		P.Add(w);

		v = ClosestIn(P);
		P.DeleteNonClosestIn();

		if(P.GetSize() > 3) return v; //Should never reach here.
	}
	return v;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////
//  COLLISION TEST
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CollisionTest(double dt){


	bool spread = true;
	//////////////////Bounding Boxes///////////////////////
	for(unsigned int b = numWalls; b< meshes.size(); b++){
		Object* objA = &(meshes[b]);
		vector<Vec3f> BoundingBoxA = objA->BoxPoints;
		UpdateCoords(BoundingBoxA, objA, dt, 1.1);
		if(spread){
			vector<Vec3f> oldBA= objA->BoxPoints;
			UpdateCoords(oldBA, objA, dt, 1.1, 0);
			for(unsigned int i = 0; i< oldBA.size(); i++){
				BoundingBoxA.push_back(oldBA[i]);
			}
		}
		for(unsigned int bn = 0; bn < meshes.size(); bn ++){
			if(bn == b) continue;
			Object* objB = &(meshes[bn]);
			bool alreadyFound = false;
			for(unsigned int i = 0; i< objA->collided0.size(); i++){
				if(objB == objA->collided0[i]){alreadyFound =true; break;}
			}
			if(alreadyFound) continue;
			vector<Vec3f> BoundingBoxB = objB->BoxPoints;
			Vec4f newRotB(objB->QRotation[0] + objB->angularVelocity*dt, objB->QRotation[1],  objB->QRotation[2],  objB->QRotation[2]);
			UpdateCoords(BoundingBoxB, objB, dt, 1.1);

			if(spread){
				vector<Vec3f> oldBB= objB->BoxPoints;
				UpdateCoords(oldBB, objB, dt, 1.1, 0);
				for(unsigned int i = 0; i< oldBB.size(); i++){
					BoundingBoxB.push_back(oldBB[i]);
				}
			}
			Simplex P;
			GJKDistance(BoundingBoxA, BoundingBoxB, P);
			if(P.GetSize() >3){ //Huzzah, collision
				if(bn >= numWalls){
					objA->collided0.push_back(objB);
					objB->collided0.push_back(objA);
				}else{
					objA->collided0.push_back(objB);
				}
			}
		}
	}

	float spaceBubble = 1.01;

	////////////////////////Actual Meshes///////////////////////
	for(unsigned int b = numWalls; b < meshes.size(); b++){
		Object* objA = &(meshes[b]);
		for(unsigned int bn = 0; bn < objA->collided0.size(); bn++){
			Object* objB = objA->collided0[bn];
			vector<Vec3f> BoundingBoxA, BoundingBoxB;
			bool alreadyFound = false;
			for(unsigned int i = 0; i< objA->actualCollision.size(); i++){
				if(objB == objA->actualCollision[i]){alreadyFound =true; break;}
			}
			if(alreadyFound) continue;
			for(MyMesh::VertexIter it = objA->mesh.vertices_begin(); it != objA->mesh.vertices_end(); ++it){
				Vec3f p = objA->mesh.point(it.handle());
				BoundingBoxA.push_back(p);
			}
			for(MyMesh::VertexIter it = objB->mesh.vertices_begin(); it != objB->mesh.vertices_end(); ++it){
				Vec3f p = objB->mesh.point(it.handle());
				BoundingBoxB.push_back(p);
			}
			float minInterpol = min(objA->minInterpolate, objB->minInterpolate);
			Simplex P;
			Vec3f V;
			float step;
			float dist = FLT_MAX;
			float computedInterpol = 0.f;
			float stepSize = 0.01;
			for(step = 0.0; step <= minInterpol; step += stepSize){
				UpdateCoords(BoundingBoxA, objA, dt, step);
				UpdateCoords(BoundingBoxB, objB, dt, step);
				Simplex Result;
				Result.clearSimplex();
				Vec3f Vresult = GJKDistance(BoundingBoxA, BoundingBoxB, Result);
				if(Result.GetSize() > 3){//our first collision
					break;
				}else if(step <= minInterpol){//Found a safe space to be in.
					P.SetToSimplex(Result);
					V = Vresult;
					computedInterpol = step;
				}else break;
			}
			dist = V.norm();
			if(dist < tolerance)cout<<"DIST: "<<dist<<endl;
			if(P.GetSize() < 4 && computedInterpol <= minInterpol){ //This is where our interpolation happened, we will use just beforehand to animate.
				//This is the earliest point prior to collision
				if(computedInterpol < objA->minInterpolate){
					objA->actualCollision.clear();
					objA->pointOfContact.clear();
					objA->contactNormal.clear();
				}

				if(computedInterpol < objB->minInterpolate){
					objB->actualCollision.clear();
					objB->pointOfContact.clear();
					objB->contactNormal.clear();
				}
				objA->minInterpolate = computedInterpol;
				objA->actualCollision.push_back(objB);
				objB->minInterpolate = computedInterpol;
				objB->actualCollision.push_back(objA);

				/////////////////////////////////////////////////
				// Barycentrics: Use them for A and B.
				///////////////////////////////////////////////
				Vec3f contactA, contactB;
				if(P.GetSize() == 1){
					contactA = P.atA(0);
					contactB = P.atB(0);
				}else if(P.GetSize() == 2){//linear interpo;
					Vector3f A = ToEigenVector(P.at(1));
					Vector3f B = ToEigenVector(P.at(0));
					Vector3f AB = B-A;
					float v = (AB.dot(-A))/(AB.dot(-A)+ (-AB).dot(-B));
					contactA = P.atA(1)*(1-v) + P.atA(0)*v;
					contactB = P.atB(1)*(1-v) + P.atB(0)*v;
				}else{//triangle
					Vector3f A = ToEigenVector(P.at(2));
					Vector3f B = ToEigenVector(P.at(1));
					Vector3f C = ToEigenVector(P.at(0));
					Vector3f N = (B-A).cross(C-A);
					Vector3f Nab = N.cross(B-A);
					Vector3f Nac = N.cross(C-A);
					float v = ((-A).dot(Nac))/((B-A).dot(Nac));
					float w = ((-A).dot(Nab))/((C-A).dot(Nab));
					float u = 1 - v - w;
					contactA = P.atA(2)*u + P.atA(1)*v + P.atA(0)*w;
					contactB = P.atB(2)*u + P.atB(1)*v + P.atB(0)*w;
				}

				Vec3f norm = contactB-contactA;
				norm.normalize();

				objA->pointOfContact.push_back(contactA);
				objB->pointOfContact.push_back(contactA);

				cout<<"Point of ContactA: "<<contactA<<endl;
				cout<<"Point of ContactB: "<<contactB<<endl;
				cout<<"Normal1: "<<norm<<endl;
				//	cout<<"Normal2: "<<-norm<<endl;
				//cin.ignore(1);
				objA->contactNormal.push_back(norm);
				objB->contactNormal.push_back(-norm);
			}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// PHYSICS
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void Physics(float dt){
	for(unsigned int i = numWalls; i< meshes.size(); i++){
		Object* o = &(meshes[i]);
		if(o->actualCollision.size() == 0)continue;
		for(unsigned int j = 0; j < o->actualCollision.size(); j++){
			Object* c = o->actualCollision[j];

			Vector3f vab = ToEigenVector(o->velocity - c->velocity);
			float e = (o->elasticity + c->elasticity)/2;
			Vector3f n = ToEigenVector(o->contactNormal[j].normalized());
			Vector3f rAPp = ToEigenVector(o->pointOfContact[j] - o->CenterOfMass);
			Vector3f axisOfRotation(o->QRotation[1], o->QRotation[2], o->QRotation[3]);
			rAPp = axisOfRotation.cross(rAPp);
			axisOfRotation = Vector3f(c->QRotation[1], c->QRotation[2], c->QRotation[3]);
			Vector3f rBPp = axisOfRotation.cross(ToEigenVector(o->pointOfContact[j] - o->CenterOfMass));

			float raSum = ((rAPp.dot(n))/o->momentOfInertia) * ((rAPp.dot(n))/o->momentOfInertia);
			float rbSum = ((rBPp.dot(n))/c->momentOfInertia) * ((rBPp.dot(n))/c->momentOfInertia);

			Vec3f wA = o->QRotation[0] * Vec3f(o->QRotation[1], o->QRotation[2], o->QRotation[3]);
			Vec3f wB = c->QRotation[0] * Vec3f(c->QRotation[1], c->QRotation[2], c->QRotation[3]);



			float denominator = (n.dot(n)*(1/o->mass + 1/c->mass));// + raSum + rbSum;
			float impulseJ = -(1+e)*(vab.dot(n))/denominator;
			o->netImpulse += ToOMVector(n*(impulseJ/o->mass));
			
		}
	}
	for(unsigned int i = numWalls; i< meshes.size(); i++){
		Object* o = &(meshes[i]);
		if(o->actualCollision.size()==0) continue;
		o->position -= o->velocity*dt*o->minInterpolate*0.9;
		o->QRotation[0] -= o->angularVelocity*dt*o->minInterpolate*0.1;
		o->velocity += o->netImpulse;
		o->netImpulse = Vec3f(0,0,0);
		o->position += o->velocity*dt*o->minInterpolate*3;
	}

}

void AnimateObjects(){
	int timeElapsed = currentTime-prevTime;
	double dt = double(timeElapsed)/1000.0;
	for(unsigned int i = 0; i< meshes.size(); i++){
		meshes[i].collided0.clear();
		meshes[i].minInterpolate = 1.0;
		meshes[i].actualCollision.clear();
		meshes[i].generalForces.clear();
		meshes[i].generalForces.push_back(gravity);
		meshes[i].contactNormal.clear();
		meshes[i].pointOfContact.clear();
		meshes[i].netAImpulse = Vec3f(0,0,0);
		meshes[i].netImpulse = Vec3f(0,0,0);
		//cin.ignore(1);
	}
	CollisionTest(dt);
	for(unsigned int i = 0; i < meshes.size(); i++){
		Object* o = &(meshes[i]);
		float interpol = o->minInterpolate;
		//if(o->actualCollision.size() > 0) interpol = interpol - 0.1;
		o->position = o->position + o->velocity.operator*(dt)*interpol;
		float newAngle = o->QRotation[0];
		newAngle += o->angularVelocity*dt*interpol;
		o->QRotation[0] = newAngle;
	}
	Physics(dt);
}

void idle(){
	AnimateObjects();
	ComputeFPS();
	glutPostRedisplay();
}


/////////////////////////////////////
// Physics Engine drawing, etc.
////////////////////////////////////

void display (void ){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60, 1, 0.1, 1000.0 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	bool bbTier1 = true;
	bool bbTier2 = true;

	Vec3f trans = -mCameraTranslation;

	glTranslatef(trans[0], trans[1], trans[2]);

	glRotatef(-mCameraElevation, 1, 0, 0);
	glRotatef(-mCameraAzimuth, 0, 1, 0);

	glScaled(scale,scale,scale);

	glViewport(0,0,win_width,win_height);

	glDisable(GL_LIGHTING);
	/////////////////////////////////////////////////////////
	//Outside Bounding Boxes
	/////////////////////////////////////////////////////////
	if(bbTier1){
		for(unsigned int m = 0; m<meshes.size();m++){
			vector<Vec3f> edges = meshes[m].BoxEdges;
			UpdateCoords(edges, &(meshes[m]), 0);
			for(unsigned int e = 0; e<edges.size();){
				glBegin(GL_LINES);
				if(meshes[m].collided0.size()>0) glColor3f(1,0,0);
				else glColor3f(0,0,1);
				glVertex3f(edges[e][0],edges[e][1], edges[e][2]);
				e++;
				glVertex3f(edges[e][0],edges[e][1], edges[e][2]);
				e++;
				glEnd();
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////
	//Mesh Box
	//////////////////////////////////////////////////////////////////////////////////
	if(bbTier2){
		for(unsigned int m = 0; m<meshes.size();m++){
			/*	glPushMatrix();
			glTranslatef(meshes[m].position[0], meshes[m].position[1], meshes[m].position[2]);
			glRotatef(meshes[m].QRotation[0], meshes[m].QRotation[1], meshes[m].QRotation[2], meshes[m].QRotation[3]);
			*/	for(MyMesh::HalfedgeIter hi = meshes[m].mesh.halfedges_begin(); hi != meshes[m].mesh.halfedges_end(); ++hi){
				MyMesh::VertexHandle ptAH = meshes[m].mesh.to_vertex_handle(hi.handle());
				MyMesh::VertexHandle ptBH = meshes[m].mesh.from_vertex_handle(hi.handle());
				Vec3f ptA = meshes[m].mesh.point(ptAH);
				Vec3f ptB = meshes[m].mesh.point(ptBH);
				GetTransformedPoint(ptA, &(meshes[m]), 0);
				GetTransformedPoint(ptB, &(meshes[m]), 0);
				glBegin(GL_LINES);
				if(meshes[m].actualCollision.size() >0)glColor3f(1,0,0);
				else glColor3f(0,0,1);
				glVertex3f(ptA[0], ptA[1], ptA[2]);
				glVertex3f(ptB[0], ptB[1], ptB[2]);
				glEnd();
			}
			//glPopMatrix();
		}
	}


	/////////////////////////////////////////////////
	// ITERATE THROUGH AND DRAW THEM
	/////////////////////////////////////////////////

	for(unsigned int i = 0; i<numWalls; i++){
		glPushMatrix();
		Object* room = &(meshes[i]);
		MyMesh* rMesh = &(room->mesh);
		glTranslated(room->position[0], room->position[1], room->position[2]);
		glRotatef(room->QRotation[0],room->QRotation[1], room->QRotation[2], room->QRotation[3]); 
		for(MyMesh::FaceIter f_it = rMesh->faces_begin(); f_it != rMesh->faces_end(); ++f_it){
			MyMesh::HalfedgeHandle heh = rMesh->halfedge_handle(f_it.handle());
			for(int j = 0; j<3; j++){
				MyMesh::VertexHandle startH = rMesh->to_vertex_handle(heh);
				MyMesh::VertexHandle fromH = rMesh->from_vertex_handle(heh);
				glBegin(GL_LINES);
				glColor3f(1,1,0);
				Vec3f start = rMesh->point(startH);
				Vec3f finish = rMesh->point(fromH);
				glVertex3f(start[0], start[1], start[2]);
				glVertex3f(finish[0], finish[1], finish[2]);
				glEnd();
				heh = rMesh->next_halfedge_handle(heh);
			}

		}
		glPopMatrix();
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHT1);
	float position2[] = {0, 5, 0, 1};
	float ambient2[] = {0, 0, 0, 1};
	float specular2[] = {0.1, 0.1, 0.1, 1};
	float diffuse2[] = {0, 0.2, 1, 1};
	glLightfv(GL_LIGHT1, GL_POSITION, position2);
	glLightfv(GL_LIGHT1, GL_AMBIENT,ambient2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);



	glEnable(GL_LIGHT2);
	float position[] = {0, 1, 0, 1};
	float ambient[] = {0,0,0,1};
	float specular[] = {0,0,0,1};
	float diffuse[] = {1,1,1,1};
	glLightfv(GL_LIGHT2, GL_POSITION, position);
	glLightfv(GL_LIGHT2, GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);

	glEnable(GL_COLOR_MATERIAL);

	//////////////The other meshes/////////////////////
	for(unsigned int i = numWalls; i<meshes.size(); i++){
		//The first item, the world....
		if(i == 0) glShadeModel(GL_FLAT);
		else glShadeModel(GL_SMOOTH);
		glPushMatrix();
		Object* o = &(meshes[i]);
		glTranslatef(o->position[0], o->position[1], o->position[2]); //move cube2 to the left
		glRotatef(o->QRotation[0],o->QRotation[1],o->QRotation[2],o->QRotation[3]); // angle in degrees, x, y,z
		MyMesh* mesh = &(o->mesh);
		glBegin(GL_TRIANGLES);
		if(i == 0){
			GLfloat black[] = {0,0,0};
			GLfloat grey[] = {0.3, 0.3, 0.3};
			glColor3f(0.3, 0.3, 0.3);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black); 
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grey);
		}else{
			glColor3f(o->color[0], o->color[1], o->color[2]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, o->color);
		}

		for (MyMesh::FaceIter it = mesh->faces_begin(); it != mesh->faces_end(); ++it) {
			//assuming triangular meshes
			MyMesh::HalfedgeHandle it2 = mesh->halfedge_handle(it.handle());
			for(int v = 0; v< 3; v++){
				MyMesh::VertexHandle v_handle = mesh->to_vertex_handle(it2);
				if(i>0 && mesh->has_vertex_normals()){
					Vec3f n = mesh->normal(v_handle);
					glNormal3f(n[0], n[1], n[2]);
				}else if(mesh->has_face_normals()){
					Vec3f n = mesh->normal(it.handle());
					glNormal3f(n[0], n[1], n[2]);
				}
				Vec3f p = mesh->point(v_handle);
				glVertex3f(p[0],p[1],p[2]);
				it2 = mesh->next_halfedge_handle(it2);
			}
		}
		glEnd();
		glPopMatrix(); // you need one of these for every glPushMatrix()
	}

	glutSwapBuffers();
}

/////////////////////////////////
// GLUT CALLBACKS
/////////////////////////////////
/*
//=================================================================//
Function: reshape
-Purpose: This function just tracks changes in window dimensions and
modifies the image accordingly.

-Parameters: The width and height of the window
//=================================================================//
*/
void reshape( int w, int h )
{
	win_width = w;
	win_height = h;

	//glutPostRedisplay();
}

/*
//=================================================================//
Functions: CameraAdjustments
-Purpose: These functions modify the globals associated with 
pseudo-camera placement in order to render an image according
to a simple camera operation.

-Parameters: delta= the amount to change each value by.

NOTES: Taken from CS148.
//=================================================================//
*/


void AdjustCameraAzimuthBy(float delta)
{
	mCameraAzimuth += delta;
}

void AdjustCameraElevationBy(float delta)
{
	mCameraElevation += delta;
}

void AdjustCameraTranslationBy(Vec3f delta)
{
	mCameraTranslation += delta;
}


/*
//=================================================================//
Function: keyboard
-Purpose: For non-special keys (ie, not arrow keys), this function
is the default keyboard analyzer.  For now, it simply quits when
you press Escape, toggles solid/wireframe values, and scales the 
mesh Up with 'u' and down with 'j'.

-Parameters: The x, y position of the image plane, and the key.
//=================================================================//
*/
void keyboard( unsigned char key, int x, int y )
{
	float moveSensitivity = 0.1;
	switch(key)
	{
	case 27: // Escape key
		exit(0);
		break;
	case 'a':
		AdjustCameraTranslationBy(Vec3f(-moveSensitivity,0,0));
		break;
	case 'd':
		AdjustCameraTranslationBy(Vec3f(moveSensitivity,0,0));
		break;
	case 's':
		AdjustCameraTranslationBy(Vec3f(0,-moveSensitivity,0));
		break;
	case 'w':
		AdjustCameraTranslationBy(Vec3f(0,moveSensitivity,0));
		break;
	case 'q':
		AdjustCameraTranslationBy(Vec3f(0,0,moveSensitivity));
		break;
	case 'e':
		AdjustCameraTranslationBy(Vec3f(0,0,-moveSensitivity));
		break;
	case 'A':
		AdjustCameraTranslationBy(Vec3f(-moveSensitivity*5,0,0));
		break;
	case 'D':
		AdjustCameraTranslationBy(Vec3f(moveSensitivity*5,0,0));
		break;
	case 'S':
		AdjustCameraTranslationBy(Vec3f(0,-moveSensitivity*5,0));
		break;
	case 'W':
		AdjustCameraTranslationBy(Vec3f(0,moveSensitivity*5,0));
		break;
	case 'Q':
		AdjustCameraTranslationBy(Vec3f(0,0,moveSensitivity*5));
		break;
	case 'E':
		AdjustCameraTranslationBy(Vec3f(0,0,-moveSensitivity*5));
		break;
	case ('=' || '+'):
		if(scale < 0.1) scale += 0.01;
		else if(scale < 10) scale += 0.1;
		break;
	case '-':
		if(scale > 0.1) scale -= 0.1;
		else if(scale > 0) scale -= 0.01;
		break;
	default:
		break;
	}
	//glutPostRedisplay();
}

/*
//=================================================================//
Function: MouseCallback
-Purpose: This function primarily informs the glut loop that a 
significant mouse button has been pressed.  It uses this to
differentiate between mouse buttons and whether one should 
actually do anything.

-Parameters: The button, state, and x, y position of the image plane.

NOTES: Taken from CS148.
//=================================================================//
*/
void MouseCallback(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
	{
		gMouseButton = button;
	} else
	{
		gMouseButton = -1;
	}

	if (state == GLUT_UP)
	{
		gPreviousMouseX = -1;
		gPreviousMouseY = -1;
	}
}
/*
//=================================================================//
Function: MouseMotionCallback
-Purpose: This function will take in x, y coordinates upon pressing
the left mouse button.  It will then rotate the "camera" in order
to change perspective.  Because OpenGL does not actually use a 
camera, we perform transformation matrices on the object. 

-Parameters: The x, y position of the image plane.

NOTES: Taken and slightly modified from CS148.
//=================================================================//
*/
void MouseMotionCallback(int x, int y)
{
	if (gPreviousMouseX >= 0 && gPreviousMouseY >= 0)
	{
		//compute delta
		float deltaX = x-gPreviousMouseX;
		float deltaY = y-gPreviousMouseY;
		gPreviousMouseX = x;
		gPreviousMouseY = y;
		float rotateSensitivity = 0.5f;

		//orbit or zoom
		if (gMouseButton == GLUT_LEFT_BUTTON)
		{
			AdjustCameraAzimuthBy(-deltaX*rotateSensitivity);
			AdjustCameraElevationBy(-deltaY*rotateSensitivity);

		}        
	} else
	{
		gPreviousMouseX = x;
		gPreviousMouseY = y;
	}
	//glutPostRedisplay();
}

/*
//=================================================================//
Function: SpecialKeyCallback
-Purpose: For every loop in glut this function evaluates user input,
specifically the use of the arrow keys in order to pan up,
down, left, or right on screen. 

-Parameters: The key value, and the x, y position of the image plane.

NOTES: Taken from CS148.
//=================================================================//
*/
void SpecialKeyCallback(int key, int x, int y)
{
	switch(key) {
	case GLUT_KEY_LEFT:
		//Move ball by...
		break;
	case GLUT_KEY_RIGHT:
		//Move ball by...
		break;
	case GLUT_KEY_DOWN:
		//Move Ball
		break;
	case GLUT_KEY_UP:
		//Move ball
		break;
	default:
		break;
	}
	//glutPostRedisplay();
}


///////////////////////////////////
// GLUT MAIN FUNCTION
///////////////////////////////////

int main (int argc, char *argv[]){
	win_width =	1024/2;
	win_height = 768/2;
	scale = 1.0;
	srand (time(NULL));

	//random = true;

	glutInit(&__argc, __argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Phsyics Engine Testground");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MouseMotionCallback);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	GLenum res = glewInit();
	if (res != GLEW_OK)	{
		cout<<(stderr, "Error: '%s'\n", glewGetErrorString(res))<<endl;
		return 1;
	}
	prevTime = glutGet(GLUT_ELAPSED_TIME);
	startTime = prevTime;

	LoadInMeshes();

	glutMainLoop();
	return 0;
}