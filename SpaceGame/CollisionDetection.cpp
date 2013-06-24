#include "CollisionDetection.h"
#include "GameState.h"
#include "GameObject.h"
#include <map>

float tolerance = 1e-4;

int MathSign(const float& v){
	if( v == 0) return 0;
	else return v/abs(v);
}

//////////////////////////////////////////////////////////////////////////
//
// BOUNDING BOX COMPUTATION
//
/////////////////////////////////////////////////////////////////////////
void ComputeBoundingBoxesAndPhysicsConstants(vector<GameObject*>& objects){
	for(unsigned int o = 0; o<objects.size(); o++){
		MyMesh* meshptr= objects[o]->GetMesh();
		if(meshptr == NULL) continue;
		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float minZ = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;
		float maxZ = -FLT_MAX;
		for(MyMesh::VertexIter vi = meshptr->vertices_begin(); vi != meshptr->vertices_end(); ++vi){
			Vec3f pt = meshptr->point(vi.handle());
			objects[o]->meshBox.push_back(pt);
			minX = min(minX, pt[0]);
			maxX = max(maxX, pt[0]);
			minY = min(minY, pt[1]);
			maxY = max(maxY, pt[1]);
			minZ = min(minZ, pt[2]);
			maxZ = max(maxZ, pt[2]);
		}
		Vec3f UR(maxX, maxY, maxZ);
		Vec3f BL(minX, minY, minZ);
		objects[o]->boundingBox.push_back(UR);
		objects[o]->boundingBox.push_back(Vec3f(UR[0], UR[1], BL[2]));
		objects[o]->boundingBox.push_back(Vec3f(BL[0], UR[1], BL[2]));
		objects[o]->boundingBox.push_back(Vec3f(BL[0], UR[1], UR[2]));
		objects[o]->boundingBox.push_back(Vec3f(BL[0], BL[1], UR[2]));
		objects[o]->boundingBox.push_back(Vec3f(UR[0], BL[1], UR[2]));
		objects[o]->boundingBox.push_back(Vec3f(UR[0], BL[1], BL[2]));
		objects[o]->boundingBox.push_back(BL);

		//	ConstructBoxEdges(meshes[m].BoxEdges, UR, BL);
		float TotalArea = 0;
		for(MyMesh::VertexIter vit = meshptr->vertices_begin(); vit != meshptr->vertices_end(); ++vit){
			float numberVertices  = 0;
			objects[o]->CenterOfMass+= meshptr->point(vit.handle())/meshptr->n_vertices();
		}
		float MomentOfInertia = 0;
		/*	for(MyMesh::VertexIter it = meshes[m].mesh.vertices_begin(); it!= meshes[m].mesh.vertices_end(); ++it){
		MomentOfInertia += ((Vec3f(meshes[m].mesh.point(it.handle())) - meshes[m].CenterOfMass) * (Vec3f(meshes[m].mesh.point(it.handle())) - meshes[m].CenterOfMass)).sqrnorm();
		}
		MomentOfInertia *= meshes[m].mass;
		meshes[m].momentOfInertia = MomentOfInertia;*/
	}
}






////////////////////////////////////////////////////////////////////////
//
// UPDATE COORDINATES
//
/////////////////////////////////////////////////////////////////////////

void GetTransformedPoint(Vec3f& pt, Vec3f position, Vec3f velocity, Vec4f rotation, Vec4f wvelocity, float dt, float scale = 1.f, float inter = 1.0){
	float angle = rotation[0];
	angle += wvelocity[0]*dt*inter;
	Vector3f newPos = Vector3f(position[0] + velocity[0]*dt*inter, position[1] + velocity[1]*dt*inter, position[2] + velocity[2]*dt*inter);
	Vector3f axis(rotation[1], rotation[2], rotation[3]);
	axis.normalize();
	Translation3f move =Translation3f(newPos.x(), newPos.y(), newPos.z());
	angle = angle*2*M_PI/360;
	AngleAxisf turn = AngleAxisf(angle, axis);
	Vector3f p = Vector3f(pt[0], pt[1], pt[2]);
	p = p*scale;
	p = turn*p;
	p = move*p;
	pt = Vec3f(p.x(), p.y(), p.z());
}

void UpdateCoords(vector<Vec3f>& v, Vec3f position, Vec3f velocity, Vec4f rotation, Vec4f wvelocity, float dt, float scale, bool pers, float inter, float xScale, float yScale, float zFar, float zNear){
	float angle = rotation[0];
	angle += wvelocity[0]*dt*inter;
	Vector3f newPos = Vector3f(position[0] + velocity[0]*dt*inter, position[1] + velocity[1]*dt*inter, position[2] + velocity[2]*dt*inter);
	Vector3f axis(rotation[1], rotation[2], rotation[3]);
	axis.normalize();
	Translation3f move =Translation3f(newPos.x(), newPos.y(), newPos.z());
	angle = angle*2*M_PI/360;
	AngleAxisf turn = AngleAxisf(angle, axis);
	Matrix4f pmat;
	if(pers){
		pmat << xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, -(zFar+zNear)/(zFar-zNear), -1,
			0, 0, -2*zNear*zFar/(zFar-zNear), 0;
	}else{
		pmat = Matrix4f::Identity();
	}
	for(unsigned int boxV = 0; boxV < v.size(); boxV++){
		Vector3f p = Vector3f(v[boxV][0], v[boxV][1], v[boxV][2]);
		/*Vector4f pt(p.x(), p.y(), p.z(), 1);
		pt = pmat*pt;
		p = Vector3f(pt.x(), pt.y(), pt.z());*/
		p = scale* p;
		if(axis.norm() > 0)
			p = turn* p;
		p = move* p;
		v[boxV] = Vec3f(p.x(), p.y(), p.z());
	}
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
			Vector3f A = ConvertToEigen3Vector(data[1]);
			Vector3f B = ConvertToEigen3Vector(data[0]);
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
			Vector3f A = ConvertToEigen3Vector(data[2]);
			Vector3f B = ConvertToEigen3Vector(data[1]);
			Vector3f C = ConvertToEigen3Vector(data[0]);
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
			Vector3f A = ConvertToEigen3Vector(data[3]);
			Vector3f B = ConvertToEigen3Vector(data[2]);
			Vector3f C = ConvertToEigen3Vector(data[1]);
			Vector3f D = ConvertToEigen3Vector(data[0]);
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

			Vector3f TriC = ConvertToEigen3Vector(data[posPiece1]);
			Vector3f TriB = ConvertToEigen3Vector(data[posPiece2]);
			Vector3f TriA = ConvertToEigen3Vector(data[posPiece3]);
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
				data.push_back(ConvertToOM3Vector(TriA));
				dataA.push_back(Aa);
				dataB.push_back(Ba);
			}else if((TriA- TriB).dot(TriBO) <= 0 && TriBC.dot(TriBO)<=0){//B is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ConvertToOM3Vector(TriB));
				dataA.push_back(Ab);
				dataB.push_back(Bb);
			}else if((TriA-TriC).dot(TriCO) <= 0 && (TriB-TriC).dot(TriCO) <= 0){ //C is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ConvertToOM3Vector(TriC));
				dataA.push_back(Ac);
				dataB.push_back(Bc);
			}else if(TriNab.dot(TriAO)>=0 && TriAB.dot(TriAO) > 0){//AB is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ConvertToOM3Vector(TriB));
				data.push_back(ConvertToOM3Vector(TriA));
				dataA.push_back(Ab);
				dataA.push_back(Aa);
				dataB.push_back(Bb);
				dataB.push_back(Ba);
			}else if(TriNac.dot(TriAO) >= 0 && TriAC.dot(TriAO) > 0){ //AC is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ConvertToOM3Vector(TriC));
				data.push_back(ConvertToOM3Vector(TriA));
				dataA.push_back(Ac);
				dataA.push_back(Aa);
				dataB.push_back(Bc);
				dataB.push_back(Ba);
			}else if(TriNbc.dot(TriBO) >= 0 && TriBC.dot(TriBO) > 0){//BC closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ConvertToOM3Vector(TriC));
				data.push_back(ConvertToOM3Vector(TriB));
				dataA.push_back(Ac);
				dataA.push_back(Ab);
				dataB.push_back(Bc);
				dataB.push_back(Bb);
			}else{//ABC closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(ConvertToOM3Vector(TriC));
				data.push_back(ConvertToOM3Vector(TriB));
				data.push_back(ConvertToOM3Vector(TriA));
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
	Vector3f d(v[0], v[1], v[2]);
	double farthestADir = Vector3f(A[0][0], A[0][1], A[0][2]).dot(d);
	unsigned int farthestAIndex = 0;
	for(unsigned int i = 1; i<A.size(); i++){
		float newDot = Vector3f(A[i][0], A[i][1], A[i][2]).dot(d);
		if(newDot > farthestADir){
			farthestADir = newDot;
			farthestAIndex = i;
		}
	}
	d = -d;
	double farthestBDir = ConvertToEigen3Vector(B[0]).dot(d);
	unsigned int farthestBIndex = 0;
	for(unsigned int i = 1; i<B.size(); i++){
		float newDot = ConvertToEigen3Vector(B[i]).dot(d);
		if(newDot > farthestBDir){
			farthestBDir = newDot;
			farthestBIndex = i;
		}
	}
	float D = (ConvertToEigen3Vector(-A[farthestAIndex])).dot(ConvertToEigen3Vector(B[farthestBIndex]) - ConvertToEigen3Vector(A[farthestAIndex]));
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
		Vector3f AO(-P.at(1)[0], -P.at(1)[1], -P.at(1)[2]);
		Vector3f AB(P.at(0)[0] - P.at(1)[0], P.at(0)[1] - P.at(1)[1], P.at(0)[2] - P.at(1)[2]);
		Vector3f BO(-P.at(0)[0], -P.at(0)[1], -P.at(0)[2]);
		float v = (AB.dot(AO))/(AB.dot(AO) + (-AB).dot(BO));
		return (P.at(1)*(1-v) + (P.at(0))*v);
	}else{//We have a triangle
		Vector3f A(P.at(2)[0], P.at(2)[1], P.at(2)[2]);
		Vector3f B(P.at(1)[0], P.at(1)[1], P.at(1)[2]);
		Vector3f C (P.at(0)[0], P.at(0)[1], P.at(0)[2]);
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
		Vector3f vE(v[0], v[1], v[2]);
		Vector3f wE(w[0], w[1], w[2]);
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
		if(vE.dot(wE) > 0)return v;
		P.Add(w);

		v = ClosestIn(P);
		P.DeleteNonClosestIn();

		if(P.GetSize() > 3) return v; //Should never reach here.
	}
	return v;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Collision Detection
//
//////////////////////////////////////////////////////////////////////////////////////////

bool AlreadyIn(GameObject* a, GameObject* b, unsigned int tierNo, GameRoom* room){
	if(tierNo == 0){
		map<GameObject*, vector<GameObject*> >::iterator it = room->collisionTier0List.find(a);
		if(it == room->collisionTier0List.end()) return false;
		vector<GameObject*> v= room->collisionTier0List[a];
		for(unsigned int i = 0; i<v.size(); i++){
			if(v[i] == b) return true;
		}
	}else if(tierNo == 1){
		map<GameObject*, vector<GameObject*> >::iterator it = room->collisionTier1List.find(a);
		if(it == room->collisionTier1List.end()) return false;
		vector<GameObject*> v= room->collisionTier1List[a];
		for(unsigned int i = 0; i<v.size(); i++){
			if(v[i] == b) return true;
		}
	}else{
		map<GameObject*, vector<GameObject*> >::iterator it = room->collisionTier2List.find(a);
		if(it == room->collisionTier2List.end()) return false;
		vector<GameObject*> v= room->collisionTier2List[a];
		for(unsigned int i = 0; i<v.size(); i++){
			if(v[i] == b) return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RESOLVE CONTACT DATA USING BARYCENTRIC COORDINATES
//
////////////////////////////////////////////////////////////////////////////////////////////
Vec3f ResolveContact(Simplex& P){
	/////////////////////////////////////////////////
	// Barycentrics: Use them for A and B.
	///////////////////////////////////////////////
	assert(P.GetSize() > 0);
	Vec3f contactA, contactB;
	if(P.GetSize() == 1){
		contactA = P.atA(0);
		contactB = P.atB(0);
	}else if(P.GetSize() == 2){//linear interpo;
		Vec3f A = P.at(1);
		Vec3f B = P.at(0);
		Vec3f AB = B-A;
		float v = (dot(AB, -A))/(dot(AB, -A)+ dot(-AB, -B));
		contactA = P.atA(1)*(1-v) + P.atA(0)*v;
		contactB = P.atB(1)*(1-v) + P.atB(0)*v;
	}else{//triangle
		Vec3f A = (P.at(2));
		Vec3f B = (P.at(1));
		Vec3f C = (P.at(0));
		Vec3f N = cross(B-A,C-A);
		Vec3f Nab = cross(N, B-A);
		Vec3f Nac = cross(N, C-A);
		float v = (dot(-A, Nac))/(dot(B-A,Nac));
		float w = (dot(-A, Nab))/(dot(C-A, Nab));
		float u = 1 - v - w;
		contactA = P.atA(2)*u + P.atA(1)*v + P.atA(0)*w;
		contactB = P.atB(2)*u + P.atB(1)*v + P.atB(0)*w;
	}

	Vec3f norm = contactB-contactA;
	norm.normalize();

	return contactA;

}

//////////////////////////////////////////////////////////////////////////////////////////
// Detects all collisions of various types with each object.
void PerformCollisionDetection(GameRoom* room, GamePlayer* player, double dt,  float xScale, float yScale, float zNear, float zFar){
	dt/=1000;
	vector<GameObject*> Objects = room->GetGameObjects();
	PSystems* ps;// = Render::gameState->GetParticleSystems();
	list<Projectile*>* projs = ps->GetBullets();
	/////////////////////////////////////////////////////////////////////////////
	// PARTICLE COLLISION DETECTION
	/////////////////////////////////////////////////////////////////////////////
	for(unsigned int i = 0; i<Objects.size(); i++){
		GameObject* o = Objects[i];
		if(o->CollisionTierNum<1) continue;
		vector<Vec3f> aBox = o->boundingBox;
		Vector3f pos = o->GetPosition();
		Vec3f p(pos.x(), pos.y(), pos.z());
		Vec4f r = o->GetRotation();
		Vec4f wv = o->angularVelocity;
		Vec3f v = o->velocity;
		UpdateCoords(aBox, p, v, r, wv, 0, 1.f, true, 1.0, xScale, yScale, zNear, zFar);
		for(list<Projectile*>::iterator it = projs->begin(); it != projs->end(); ++it){
			vector<Vec3f> bBox = (*(*it)).boundingBox;
			Vector3f pos2 = (*(*it)).getPosition();
			Vec3f p2(pos.x(), pos.y(), pos.z());
			Vec4f r2(0,0,1,0);
			Vec4f wv2(1,0,0,0);
			Vector3f vel2 = (*(*it)).getVelocity();
			Vec3f v2(vel2.x(), vel2.y(), vel2.z());
			UpdateCoords(bBox, p2, v2, r2, wv2, 0, 10.f, true, 1.0,  xScale, yScale, zNear, zFar);
			Simplex P;
			Vec3f V;
			V = GJKDistance(aBox, bBox, P);
			if(P.GetSize() > 3 || V.norm() < tolerance){ //We have a collision
				Vec3f contact = ResolveContact(P); 
				o->collidedProjectiles.push_back((*it));
				CollisionData o1D, o2D;
				o1D.pointOfContact = contact;
				o1D.contactNormal = V.normalized();
				o->projectileCollisionData[(*it)]=o1D;
				(*it)->drawCollision = true;
			}
		}

	}

	////////////////////////////////////////////////////////////////////////////
	// SPECIAL DATA FOR MAIN CHARACTER ONLY
	////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////
	//	Tier 0 collision
	//////////////////////////////////////////////////

	for(unsigned int a = 0; a<Objects.size()-1; a++){
		GameObject* o1 = Objects[a];
		if(o1->CollisionTierNum < 1 )continue;
		vector<Vec3f> aBox = o1->boundingBox;
		Vector3f pos = o1->GetPosition();
		Vec3f position = Vec3f(pos.x(), pos.y(), pos.z()); 
		Vec4f rotation = o1->GetRotation(); 
		Vec3f vel = o1->velocity;
		Vec4f wvel = o1->angularVelocity;
		UpdateCoords(aBox, position, vel, rotation, wvel ,dt, o1->outSideCollisionScale, true, 1.f, xScale, yScale, zNear, zFar);
		vector<Vec3f> oldABox = o1->boundingBox;
		UpdateCoords(oldABox, position, vel, rotation, wvel,0, o1->outSideCollisionScale, true, 1.f, xScale, yScale, zNear, zFar);
		for(unsigned int i = 0; i <oldABox.size(); i++){
			aBox.push_back(oldABox[i]);
		}
		for(unsigned int b = a+1; b<Objects.size(); b++){
			GameObject* o2 = Objects[b];
			if(o1->objType == WORLD_OBJECT_TYPE && o2->objType == WORLD_OBJECT_TYPE || o2->CollisionTierNum < 1){
				continue;
			}
			vector<Vec3f> bBox = o2->boundingBox;
			pos= o2->GetPosition();
			position = Vec3f(pos.x(), pos.y(), pos.z());
			vel = o2->velocity;
			rotation = o2->GetRotation();
			wvel = o2->angularVelocity;
			UpdateCoords(bBox, position, vel, rotation, wvel,dt, o2->outSideCollisionScale, true, 1.f, xScale, yScale, zNear, zFar);
			vector<Vec3f> oldBBox = o2->boundingBox;
			UpdateCoords(oldBBox, position, vel, rotation, wvel,0, o2->outSideCollisionScale,true, 1.f, xScale, yScale, zNear, zFar);
			for(unsigned int i = 0; i <oldBBox.size(); i++){
				bBox.push_back(oldBBox[i]);
			}
			Simplex P;
			Vec3f V;
			V = GJKDistance(aBox, bBox, P);
			if(V.norm() < tolerance){ //We have a collision
				if(!AlreadyIn(o1, o2, 0, room)){
					Vec3f contact = ResolveContact(P); 
					room->collisionTier0List[o1].push_back(o2);
					room->collisionTier0List[o2].push_back(o1);
					CollisionData o1D, o2D;
					o1D.pointOfContact = contact;
					o2D.pointOfContact = contact;
					o1D.contactNormal = V.normalized();
					o2D.contactNormal = -V.normalized();
					o1->tier0CollisionData[o2] = o1D;
					o2->tier0CollisionData[o1] = o2D;
				}
			}
		}
	}
	////////////////////////////////////////////////
	// Tier 1 collision
	///////////////////////////////////////////////
	map<GameObject*, vector<GameObject*> >::iterator it = room->collisionTier0List.begin();
	while(it!=room->collisionTier0List.end()){
		GameObject* o1 = it->first;
		if(o1->CollisionTierNum<2)
			continue;
		Vector3f pos = o1->GetPosition();
		Vec3f position(pos.x(), pos.y(), pos.z());
		Vec4f rotation = o1->GetRotation();
		Vec3f vel = o1->velocity;
		Vec4f wvel = o1->angularVelocity;
		vector<Vec3f> aBox = o1->boundingBox;
		UpdateCoords(aBox,position, vel, rotation, wvel ,0, 1.0,true, 1.f, xScale, yScale, zNear, zFar);
	//	vector<Vec3f> oldABox = o1->boundingBox;
	//	UpdateCoords(aBox,position, vel, rotation, wvel ,0, 1.0,true, 1.f, xScale, yScale, zNear, zFar);
	//	for(unsigned int i = 0; i <oldABox.size(); i++){
	//		aBox.push_back(oldABox[i]);
	//	}
		vector<GameObject*> Bs = it->second;
		it++;
		for(unsigned int b = 0; b<Bs.size(); b++){
			GameObject* o2 = Bs[b];
			if(o2->CollisionTierNum<2)
				continue;
			vector<Vec3f> bBox = o2->boundingBox;
			//pos = o2->GetPosition();
			position = Vec3f(pos.x(), pos.y(), pos.z());
			vel = o2->velocity;
			wvel = o2->angularVelocity;
			rotation = o2->GetRotation();
			UpdateCoords(bBox, position, vel, rotation, wvel ,0,1.0, true, 1.f, xScale, yScale, zNear, zFar);
			vector<Vec3f> oldBBox = o2->boundingBox;
			//UpdateCoords(oldBBox,position, vel, rotation, wvel ,0, 1.0, true, 1.f, xScale, yScale, zNear, zFar);
			//for(unsigned int i = 0; i <oldBBox.size(); i++){
			//	bBox.push_back(oldBBox[i]);
			//}
			Simplex P;
			Vec3f V;
			V = GJKDistance(aBox, bBox, P);
			if(V.norm() < tolerance){ //We have a collision.
				if(!AlreadyIn(o1, o2, 1, room)) {
					if(o1->objType == ACTIVE_OBJECT_TYPE && o2->objType == ACTIVE_OBJECT_TYPE){
						o1->drawCollision = true;
						o2->drawCollision = true;
					}
					Vec3f contact = ResolveContact(P);
					room->collisionTier1List[o1].push_back(o2);
					room->collisionTier1List[o2].push_back(o1);
					CollisionData o1D, o2D;
					o1D.pointOfContact = contact;
					o2D.pointOfContact = contact;
					o1D.contactNormal = V.normalized();
					o2D.contactNormal = -V.normalized();
					o1->tier1CollisionData[o2] = o1D;
					o2->tier1CollisionData[o1] = o2D;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	//
	// Mesh Level Detection
	//
	/////////////////////////////////////////////////////////////////////////////

}