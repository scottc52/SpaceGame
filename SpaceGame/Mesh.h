/*
 * A mesh class, dependent on OpenMesh.  We will use this class for consistency as well as a central place for modification. 
 * The additional benefit is that we can also simply declare pointers to a particular mesh, to avoid multiple instances of the same mesh
 * for multiple objects
 */
#ifndef _MESH_H_
#define _MESH_H_
#ifdef _WIN32
#include "windows.h"
#endif
//// OpenMeshIncludes
#undef min
#undef max
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/reader/OBJReader.hh>
#include <string>
using namespace OpenMesh;

//HERE WE CAN DEFINE CUSTOM TRAITS TO THE MESH AND IT WILL APPLY ACROSS ALL MODULES.

typedef TriMesh_ArrayKernelT<> MyMesh;
namespace MyMeshIO{
	bool LoadMesh(MyMesh &mesh, const std::string &fname, IO::Options opt = IO::Options::Default);
}
#endif _MESH_H_
