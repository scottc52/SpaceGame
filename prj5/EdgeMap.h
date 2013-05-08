/**
edge_map.h
*/
#ifndef E_MAP_H
#define E_MAP_H 
#include <map>
#include <vector>

using namespace std;

class Edge_Key{
public:
	int nv, sv;
	Edge_Key(int nv, int sv);
};

typedef struct edge_Value{
	int l, r; 
	int mid;
	edge_Value():l(-1), r(-1), mid(-1){} 
} Edge_Value; 

typedef bool (*EComp)(const Edge_Key &, const Edge_Key &);

//forward Declare Face
class Face;

class EdgeMap{
private: 
	int count; 
public:	
	pair<int, int> getFaces(int v1, int v2);
	int getMid(int v1, int v2);
	Edge_Value getEntry(int v1, int v2); 
	void put(int v1, int v2, int f);
	void putAll(vector<Face> *faces);  
	EdgeMap(int count = 0);
	unsigned int size();
private:
	map<Edge_Key, Edge_Value, EComp> edgeMap; 
};




#endif
