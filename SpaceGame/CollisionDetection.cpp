#include "CollisionDetection.h"
#include "GameState.h"
#include "GameObject.h"
#include <map>

float tolerance = 1e-4;

int MathSign(const float& v){
	if( v == 0) return 0;
	else return v/abs(v);
}
////////////////////////////////////////////////////////////////////////
//
// UPDATE COORDINATES
//
/////////////////////////////////////////////////////////////////////////

void GetTransformedPoint(Vec3f& pt, Vec3f position, Vec3f velocity, Vec4f& rotation, Vec4f& wvelocity, float dt, float scale = 1.f, float inter = 1.0){
	float angle = rotation[0];
	angle += wvelocity[0]*dt*inter;
	Vector3f newPos = ConvertToEigen3Vector(position + velocity*dt*inter);
	Vector3f axis(rotation[1], rotation[2], rotation[3]);
	axis.normalize();
	Translation3f move =Translation3f(newPos);
	angle = angle*2*M_PI/360;
	AngleAxisf turn = AngleAxisf(angle, axis);
	Vector3f p = ConvertToEigen3Vector(pt);
	p = p*scale;
	p = turn*p;
	p = move*p;
	pt = ConvertToOM3Vector(p);
}

void UpdateCoords(vector<Vec3f>& v, Vec3f position, Vec3f velocity, Vec4f& rotation, Vec4f& wvelocity, float dt, float scale = 1.f, float inter = 1.f){
	float angle = rotation[0];
	angle += wvelocity[0]*dt*inter;
	Vector3f newPos = ConvertToEigen3Vector(position + velocity*dt*inter);
	Vector3f axis(rotation[1], rotation[2], rotation[3]);
	axis.normalize();
	Translation3f move =Translation3f(newPos);
	angle = angle*2*M_PI/360;
	AngleAxisf turn = AngleAxisf(angle, axis);
	for(unsigned int boxV = 0; boxV < v.size(); boxV++){
		Vector3f p = ConvertToEigen3Vector(v[boxV]);
		p = scale* p;
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
			Vec3f A = data[1];
			Vec3f B = data[0];
			if(dot(-A, B-A) <= 0){ //A is closest
				data[0] = data[1];
				data.pop_back();
				dataA[0] = dataA[1];
				dataB[0] = dataB[1];
				dataA.pop_back();
				dataB.pop_back();
			}else if(dot(-B, A-B) <= 0){ //B is closest
				data.pop_back();
				dataA.pop_back();
				dataB.pop_back();
			}else{ //AB is closest
				return;
			}

		}else if(data.size() == 3){ //Triangle
			Vec3f A = data[2];
			Vec3f B = data[1];
			Vec3f C = data[0];
			Vec3f AB = B -A;
			Vec3f AC = C- A;
			Vec3f BC = C-B;
			Vec3f Nab = cross(cross(AC,AB), AB);
			Vec3f Nac = cross(cross(AB,AC), AC);
			Vec3f Nbc = cross(cross(-AB, BC), BC);

			if(dot(-A, AB) <= 0 && dot(-A, AC)<=0){ //A is closest;
				data[0] = data[2];
				dataA[0] = dataA[2];
				dataB[0] = dataB[2];
				data.pop_back();
				data.pop_back();
				dataA.pop_back();
				dataA.pop_back();
				dataB.pop_back();
				dataB.pop_back();
			}else if (dot(-AB, -B) <=0 && dot(BC, -B)<=0){//B is closest
				data[0] = data[1];
				dataA[0] = dataA[1];
				dataB[0] = dataB[1];
				data.pop_back();
				data.pop_back(); 
				dataA.pop_back();
				dataA.pop_back();
				dataB.pop_back();
				dataB.pop_back();
			}else if(dot(-AC, -C)<= 0 && dot(-C, -BC) <=0){//C is closest
				data.pop_back();
				data.pop_back();
				dataA.pop_back();
				dataA.pop_back();
				dataB.pop_back();
				dataB.pop_back();
			}else if(dot(Nab, -A)>=0 && dot(AB,-A) > 0){//AB is closest
				data[0] = data[1];//BBA
				data[1] = data[2];//BAA
				data.pop_back();//BA
				dataA[0] = dataA[1];
				dataA[1] = dataA[2];
				dataB[0] = dataB[1];
				dataB[1] = dataB[2];
				dataA.pop_back();
				dataB.pop_back();
			}else if(dot(Nac, -A) >=0 && dot(AC, -A) > 0){//AC is closest
				data[1] = data[2]; //CAA
				data.pop_back();
				dataA[1] = dataA[2];
				dataB[1] = dataB[2];
				dataA.pop_back();
				dataB.pop_back();
			}else if(dot(Nbc, -B) >=0 && dot(BC, -B) > 0){//BC is closest
				data.pop_back();
				dataA.pop_back();
				dataB.pop_back();
			}else{
				return;
			}
		}else{ //Tetrahedron
			unsigned int posPiece1, posPiece2, posPiece3;
			Vec3f A = data[3];
			Vec3f B = data[2];
			Vec3f C = data[1];
			Vec3f D = data[0];
			Vec3f AO = -A;
			Vec3f BO = -B;
			Vec3f CO = -C;
			Vec3f DO = -D;
			Vec3f AB = B-A;
			Vec3f AC = C-A;
			Vec3f AD = D-A;
			Vec3f Nabc = cross(AB,AC);
			Vec3f Nabd = cross(AB, AD);
			Vec3f Nacd = cross(AC, AD);
			Vec3f Nbcd = cross(C-B, D-B);
			if((dot(Nabc, AO)*(dot(Nabc, AD))<0)){///ABC to check
				posPiece1 = 1;
				posPiece2 = 2;
				posPiece3 = 3;
			}else if(dot(Nabd, AO) * (dot(Nabd, AC) < 0)){ //ABD to check
				posPiece1 = 0;
				posPiece2 = 2;
				posPiece3 = 3;
			}else if(dot(Nacd, AO)*dot(Nacd, AB)<0){//ACD to check
				posPiece1 = 0;
				posPiece2 = 1;
				posPiece3 = 3;
			}else if(dot(Nbcd, BO) * dot(Nbcd, A-B) < 0){//BCD to check
				posPiece1 = 0;
				posPiece2 = 1;
				posPiece3 = 2;
			}else{ return;}

			Vec3f TriC = data[posPiece1];
			Vec3f TriB = data[posPiece2];
			Vec3f TriA = data[posPiece3];
			Vec3f Ac = dataA[posPiece1];
			Vec3f Ab = dataA[posPiece2];
			Vec3f Aa = dataA[posPiece3];
			Vec3f Bc = dataB[posPiece1];
			Vec3f Bb = dataB[posPiece2];
			Vec3f Ba = dataB[posPiece3];
			Vec3f TriAO = -TriA;
			Vec3f TriBO = -TriB;
			Vec3f TriCO = -TriC;
			Vec3f TriAB = TriB-TriA;
			Vec3f TriAC = TriC-TriA;
			Vec3f TriBC = TriC-TriB;
			Vec3f TriNab = cross(cross(TriAC, TriAB), (TriAB));
			Vec3f TriNac = cross(cross(TriAB, TriAC), TriAC);
			Vec3f TriNbc = cross(cross(TriA-TriB,TriBC), TriBC);

			if(dot(TriAB, TriAO) <= 0 && dot(TriAC, TriAO) <= 0){ //A is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back(TriA);
				dataA.push_back(Aa);
				dataB.push_back(Ba);
			}else if(dot(TriA- TriB, TriBO) <= 0 && dot(TriBC, TriBO)<=0){//B is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back((TriB));
				dataA.push_back(Ab);
				dataB.push_back(Bb);
			}else if(dot(TriA-TriC, TriCO) <= 0 && dot(TriB-TriC,TriCO) <= 0){ //C is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back((TriC));
				dataA.push_back(Ac);
				dataB.push_back(Bc);
			}else if(dot(TriNab, TriAO)>=0 && dot(TriAB, TriAO) > 0){//AB is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back((TriB));
				data.push_back((TriA));
				dataA.push_back(Ab);
				dataA.push_back(Aa);
				dataB.push_back(Bb);
				dataB.push_back(Ba);
			}else if(dot(TriNac, TriAO) >= 0 && dot(TriAC, TriAO) > 0){ //AC is closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back((TriC));
				data.push_back((TriA));
				dataA.push_back(Ac);
				dataA.push_back(Aa);
				dataB.push_back(Bc);
				dataB.push_back(Ba);
			}else if(dot(TriNbc, TriBO) >= 0 && dot(TriBC, TriBO) > 0){//BC closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back((TriC));
				data.push_back((TriB));
				dataA.push_back(Ac);
				dataA.push_back(Ab);
				dataB.push_back(Bc);
				dataB.push_back(Bb);
			}else{//ABC closest
				data.clear();
				dataA.clear();
				dataB.clear();
				data.push_back((TriC));
				data.push_back((TriB));
				data.push_back((TriA));
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
	Vec3f d = v;
	double farthestADir = dot(A[0], d);
	unsigned int farthestAIndex = 0;
	for(unsigned int i = 1; i<A.size(); i++){
		float newDot = dot(A[i], d);
		if(newDot > farthestADir){
			farthestADir = newDot;
			farthestAIndex = i;
		}
	}
	d = -d;
	double farthestBDir = dot(B[0], d);
	unsigned int farthestBIndex = 0;
	for(unsigned int i = 1; i<B.size(); i++){
		float newDot = dot(B[i], d);
		if(newDot > farthestBDir){
			farthestBDir = newDot;
			farthestBIndex = i;
		}
	}
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
		Vec3f AO = -P.at(1);
		Vec3f AB = P.at(0) - P.at(1);
		Vec3f BO = -P.at(0);
		float v = (dot(AB, AO))/(dot(AB, AO) + dot(-AB, BO));
		return (P.at(1)*(1-v) + (P.at(0))*v);
	}else{//We have a triangle
		Vec3f A = P.at(2);
		Vec3f B = P.at(1);
		Vec3f C = P.at(0);
		Vec3f AB = B-A;
		Vec3f AC = C-A;
		Vec3f N = cross(AB, AC);
		Vec3f Nab = cross(N, AB);
		Vec3f Nac = cross(N, AC);

		float v = dot(-A, Nac)/(dot(AB, Nac));
		float w = dot(-A, Nab)/(dot(AC, Nab));
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
		if(dot(v, w) > 0)return v;
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
}

//////////////////////////////////////////////////////////////////////////////////////////
// Detects all collisions of various types with each object.
void PerformCollisionDetection(GameRoom* room, GamePlayer* player, double dt){
	////////////////////////////////////////////////////////////////////////////
	// SPECIAL DATA FOR MAIN CHARACTER ONLY
	////////////////////////////////////////////////////////////////////////////

	vector<GameObject*> Objects = room->GetGameObjects();
	for(unsigned int a = 0; a<Objects.size()-1; a++){
		//Spherical test.
		GameObject* o1 = Objects[a];
		for(unsigned int b = a+1; b<Objects.size(); b++){
			GameObject* o2 = Objects[b];
			if((o2->GetPosition() - o1->GetPosition()).norm() < (o1->radius + o2->radius)){
				if(!AlreadyIn(o1, o2, 0, room)) {
					room->collisionTier0List[o1].push_back(o2);
					room->collisionTier0List[o2].push_back(o1);
				}
/*linux frienly version
		if(o1->CollisionTierNum<1) continue;
		vector<Vec3f> aBox = o1->boundingBox;
		Vec3f A = ConvertToOM3Vector(o1->GetPosition()); 
		Vec4f B = Vec4f(1,0,0,0); 
		UpdateCoords(aBox, A, o1->velocity, o1->GetRotation(), B ,dt, o1->outSideCollisionScale);
		vector<Vec3f> oldABox = o1->boundingBox;
		A = ConvertToOM3Vector(o1->GetPosition()); 
		UpdateCoords(oldABox, A, o1->velocity, o1->GetRotation(), B,0, o1->outSideCollisionScale);
		for(unsigned int i = 0; i <oldABox.size(); i++){
			aBox.push_back(oldABox[i]);
		}
		for(unsigned int b = a+1; b<Objects.size(); b++){
			GameObject* o2 = Objects[b];

			if(o2->CollisionTierNum<1) continue;
			vector<Vec3f> bBox = o2->boundingBox;
			A = ConvertToOM3Vector(o2->GetPosition());
			B = Vec4f(0,0,0,1);
			UpdateCoords(bBox, A, (o2->velocity), (o2->GetRotation()), B,dt, o2->outSideCollisionScale);
			vector<Vec3f> oldBBox = o2->boundingBox;
			A = ConvertToOM3Vector(o2->GetPosition());
			UpdateCoords(oldBBox, A, (o2->velocity), (o2->GetRotation()), B,0, o2->outSideCollisionScale);
			for(unsigned int i = 0; i <oldBBox.size(); i++){
				bBox.push_back(oldBBox[i]);
			}
			Simplex P;
			Vec3f V;
			V = GJKDistance(aBox, bBox, P);
			if(P.GetSize() > 3){ //We have a collision
*/
				////////////////////////////////////////////////
				// Tier 0 collision
				///////////////////////////////////////////////
				/*	GameObject* o1 = Objects[a];
				if(o1->CollisionTierNum<1) continue;
				vector<Vec3f> aBox = o1->boundingBox;
				UpdateCoords(aBox, ConvertToOM3Vector(o1->GetPosition()), o1->velocity, o1->GetRotation(), Vec4f(1,0,0,0),dt, o1->outSideCollisionScale);
				vector<Vec3f> oldABox = o1->boundingBox;
				UpdateCoords(oldABox, ConvertToOM3Vector(o1->GetPosition()), o1->velocity, o1->GetRotation(), Vec4f(1,0,0,0),0, o1->outSideCollisionScale);
				for(unsigned int i = 0; i <oldABox.size(); i++){
				aBox.push_back(oldABox[i]);
				}
				for(unsigned int b = a+1; b<Objects.size(); b++){
				GameObject* o2 = Objects[b];

				if(o2->CollisionTierNum<1) continue;
				vector<Vec3f> bBox = o2->boundingBox;
				UpdateCoords(bBox, ConvertToOM3Vector(o2->GetPosition()), (o2->velocity), (o2->GetRotation()), Vec4f(0,0,0,1),dt, o2->outSideCollisionScale);
				vector<Vec3f> oldBBox = o2->boundingBox;
				UpdateCoords(oldBBox, ConvertToOM3Vector(o2->GetPosition()), (o2->velocity), (o2->GetRotation()), Vec4f(0,0,0,1),0, o2->outSideCollisionScale);
				for(unsigned int i = 0; i <oldBBox.size(); i++){
				bBox.push_back(oldBBox[i]);
				}
				Simplex P;
				Vec3f V;
				V = GJKDistance(aBox, bBox, P);
				if(P.GetSize() > 3){ //We have a collision.
				*/
			}
		}
	}
	/*
	////////////////////////////////////////////////
	// Tier 1 collision
	///////////////////////////////////////////////
	map<GameObject*, vector<GameObject*> >::iterator it = room->collisionTier0List.begin();
	while(it!=room->collisionTier0List.end()){
		GameObject* o1 = it->first;
		if(o1->CollisionTierNum<2) continue;
		vector<Vec3f> aBox = o1->boundingBox;
		Vec3f A = ConvertToOM3Vector(o1->GetPosition());
		Vec4f B = Vec4f(0,0,0,1);
		UpdateCoords(aBox, A,(o1->velocity), (o1->GetRotation()), B,dt);
		vector<Vec3f> oldABox = o1->boundingBox;
		A = ConvertToOM3Vector(o1->GetPosition());
		UpdateCoords(oldABox, A, (o1->velocity), (o1->GetRotation()), B,0);
		vector<GameObject*> Bs = it->second;
		it++;
		for(unsigned int i = 0; i <Bs.size(); i++){
			aBox.push_back(oldABox[i]);
		}
		for(unsigned int b = 0; b<Bs.size(); b++){
			GameObject* o2 = Objects[b];

			if(o2->CollisionTierNum<2) continue;
			vector<Vec3f> bBox = o2->boundingBox;
			A = ConvertToOM3Vector(o2->GetPosition());
			UpdateCoords(bBox, A, (o2->velocity),(o2->GetRotation()), B,dt);
			vector<Vec3f> oldBBox = o2->boundingBox;
			A = ConvertToOM3Vector(o2->GetPosition());
			UpdateCoords(oldBBox, A, (o2->velocity), (o2->GetRotation()), B,0);
			for(unsigned int i = 0; i <oldBBox.size(); i++){
				bBox.push_back(oldBBox[i]);
			}
			Simplex P;
			Vec3f V;
			V = GJKDistance(aBox, bBox, P);
			if(P.GetSize() > 3){ //We have a collision.
				if(!AlreadyIn(o1, o2, 1, room)) {
					room->collisionTier1List[o1].push_back(o2);
					room->collisionTier1List[o2].push_back(o1);
					GameDebugger::GetInstance()->WriteDebugMessageToConsole("Hit was successful!", 533);
				}
			}
		}
	}*/
	//////////////////////////////////////////////////////////////////////////////
	//
	// Mesh Level Detection
	//
	/////////////////////////////////////////////////////////////////////////////
}