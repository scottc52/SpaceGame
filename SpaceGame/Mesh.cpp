//
//Mesh.cpp
//

#include "Mesh.h"

using namespace std;
using namespace OpenMesh;

IO::_OBJReader_ *p_instance = NULL; 

bool MyMeshIO::LoadMesh(MyMesh &m, const string &s, IO::Options opt){
	if(!p_instance){
		p_instance = new IO::_OBJReader_();
	}
	bool = IO::read_mesh(m, s, opt); 
	mesh->request_vertex_normals();
	mesh->request_face_normals();
	mesh->update_normals();	
	return 
}