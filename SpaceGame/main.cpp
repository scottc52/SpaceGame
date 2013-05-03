//////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////

/* Include windows only for windows users :P
*/
#include "windows.h"
#include <gl/glew.h>
#include "gl/GL.h"
#include <gl/freeglut.h>
#include <cassert>
#include <iostream>
//// OpenMeshIncludes
#undef min
#undef max
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
using namespace std;
using namespace OpenMesh;

int main(int _argc, char** _argv){
	cout<<"Hello, world!\n";
	cin.ignore(1);
	return 0;
}