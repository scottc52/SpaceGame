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
	bool ret = IO::read_mesh(m, s, opt); 
	m.request_vertex_normals();
	m.request_face_normals();
	m.update_normals();	
	return ret;
}