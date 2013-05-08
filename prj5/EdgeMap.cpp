/**
*edgemap.cpp
*/

#include "EdgeMap.h"
#include "Mesh.h"
#include <map>
#include <vector>

using namespace std;

Edge_Key::Edge_Key(int nv1, int sv1):nv(nv1), sv(sv1){
	if ((unsigned long) nv > (unsigned long) sv){
		swap(nv, sv);  
	}
}

bool comp(const Edge_Key &a, const Edge_Key &b){
	if (a.nv > b.nv){
		return false; 
	}
	
	if ( a.nv <  b.nv){
		return true;
	} 

	if ( a.sv < b.sv){
		return true; 
	}else{
		return false;
	}
}

void EdgeMap::putAll(vector<Face> *faces){
	
	for (int i = 0 ;  i <(int) faces->size(); i++){
		Face *f = &faces->at(i);
		int a = f->vertices[0];
		int b = f->vertices[1];
		int c = f->vertices[2];
		put(a,b,i);
		put(a,c,i);
		put(b,c,i);
	}
	return; 
}

EdgeMap::EdgeMap(int count1): count(count1){
	EComp fn = comp;
	edgeMap = map<Edge_Key, Edge_Value, EComp>(fn);
}

void EdgeMap::put(int v1, int v2, int f){
	Edge_Key key(v1, v2);
	map<Edge_Key, Edge_Value, EComp>::iterator it = edgeMap.find(key); 
	Edge_Value ev;
	if (it == edgeMap.end()){
		ev= Edge_Value();
		ev.l = f; 
		ev.mid = count;
		count ++;
	} else {
		ev = it->second; 
		ev.r = f;
	}
	edgeMap[key] = ev; 
}


Edge_Value EdgeMap::getEntry(int v1,int v2){
	Edge_Key key(v1, v2);
	map<Edge_Key, Edge_Value, EComp>::iterator it = edgeMap.find(key); 
	Edge_Value ev;
	if (it == edgeMap.end()){
		ev.l = ev.r = -1;ev.mid = -1; 
		return ev;
	}
	ev = it->second;
	return ev; 
} 

int EdgeMap::getMid(int v1, int v2){ 
	return getEntry(v1, v2).mid;
}

unsigned int EdgeMap::size(){
	return edgeMap.size(); 
}

pair<int, int> EdgeMap::getFaces(int v1, int v2){
	Edge_Value ev = getEntry(v1, v2);
	return make_pair(ev.l, ev.r);
} 
