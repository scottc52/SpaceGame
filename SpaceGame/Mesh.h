/*
 * A mesh class, dependent on OpenMesh.  We will use this class for consistency as well as a central place for modification. 
 * The additional benefit is that we can also simply declare pointers to a particular mesh, to avoid multiple instances of the same mesh
 * for multiple objects
 */
#ifndef _MESH_H_
#define _MESH_H_
#include "windows.h"
//// OpenMeshIncludes
#undef min
#undef max
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
using namespace OpenMesh;

//HERE WE CAN DEFINE CUSTOM TRAITS TO THE MESH AND IT WILL APPLY ACROSS ALL MODULES.

class GameMesh{
private:
	typedef PolyMesh_ArrayKernelT<> MyMesh;
public: 
	MyMesh mesh;
	GameMesh::GameMesh(){}
	GameMesh::~GameMesh(){}
};
#endif _MESH_H_