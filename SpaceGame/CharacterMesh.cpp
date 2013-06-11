#include "CharacterMesh.h"

using namespace Eigen;
using namespace std;

CharacterMesh::CharacterMesh(const string& filename) {
		
	ifstream infile(filename.c_str());
	if(!infile.is_open())
		throw exception(string("'" + filename + "' could not be opened").c_str());

	string line;
	int lineNum = 1; //for syntax error reporting
	while(getline(infile, line)) {
		stringstream tokenizer(line);
		string lineType;
		if(!(tokenizer >> lineType))
			continue;//this must be a blank line
		if(lineType ==  "#" || line.empty()) //comment OR blank line
			continue;
		else if(lineType == "v") { //vertex
			float data[3];
			if(!(tokenizer >> data[0] >> data[1] >> data[2])) {
				infile.close();
				throw exception(string(lineNum + ":invalid syntax").c_str());
			}
			vertices.push_back(Point3f(data[0], data[1], data[2]));
		} else if(lineType == "vn") {
			float data[3];
			if(!(tokenizer >> data[0] >> data[1] >> data[2])) {
				infile.close();
				throw exception(string(lineNum + ":invalid syntax").c_str());
			}
			normals.push_back(Vector3f(data[0], data[1], data[2]));		
		} else if(lineType == "vt") {
			float data[2];
			if(!(tokenizer >> data[0] >> data[1])) { //there's one more tex coor
				infile.close();
				throw exception(string(lineNum + ":invalid syntax").c_str());
			}
			texture.push_back(TexCoor2D(data[0], data[1]));	
		} else if(lineType == "f") { //face
			try {
				ReadFace(tokenizer);
			} catch(exception& e) {
				infile.close();
				throw exception(string(lineNum + ":invalid syntax").c_str());
			}

		} else {
			continue; //ignore invalid lines
			//infile.close();
			//throw exception(string(lineNum + ":invalid syntax").c_str());
		}
		//case of line with whitespace?
		++lineNum;
	}
	cout << "Number of vertices: " << vertices.size() << endl 
			<< "Number of faces: "	   << faces.size()    << endl;

	////set up VBO
	//vertexDataSize = sizeof(Point3f) + 
	//	((!normals.empty()) ? sizeof(Vector3f) : 0) + 
	//	((!texture.empty()) ? sizeof(TexCoor2D) : 0);
	////USE = instead of memcpy
	//vertexBuf = new unsigned char[vertices.size() * vertexDataSize];
	//for(int i = 0; i < vertices.size(); ++i) {
	//	memcpy(&vertexBuf[i * vertexDataSize], &vertices[i], sizeof(Point3f));
	//}
	//indexBuf = new unsigned int[faces.size() * 3];
	//for(int i = 0; i < faces.size(); ++i) {
	//	indexBuf[i * 3 + 0] = faces[i].v1;
	//	indexBuf[i * 3 + 1] = faces[i].v2;
	//	indexBuf[i * 3 + 2] = faces[i].v3;
	//}
	//if(!normals.empty() && !texture.empty()) { //assuming that if normal's are there, then texture is also there
	//	for(int i = 0; i < faces.size(); ++i) {
	//		memcpy(
	//			&vertexBuf[faces[i].v1 * vertexDataSize + sizeof(Point3f)],
	//			&normals[faces[i].n1], sizeof(Vector3f)
	//		);
	//		memcpy(
	//			&vertexBuf[faces[i].v2 * vertexDataSize + sizeof(Point3f)],
	//			&normals[faces[i].n2], sizeof(Vector3f)
	//		);
	//		memcpy(
	//			&vertexBuf[faces[i].v3 * vertexDataSize + sizeof(Point3f)],
	//			&normals[faces[i].n3], sizeof(Vector3f)
	//		);
	//		memcpy(
	//			&vertexBuf[faces[i].v1 * vertexDataSize + sizeof(Point3f) + sizeof(Vector3f)],
	//			&texture[faces[i].t1], sizeof(TexCoor2D)
	//		);
	//		memcpy(
	//			&vertexBuf[faces[i].v2 * vertexDataSize + sizeof(Point3f) + sizeof(Vector3f)],
	//			&texture[faces[i].t2], sizeof(TexCoor2D)
	//		);
	//	}
	//}

	infile.close();
}

void CharacterMesh::ReadFace(stringstream& tokenizer) {
	string s;
	tokenizer >> s;
	size_t delim1_index = s.find('/');
	if(delim1_index == string::npos) { //f v
		unsigned int v1, v2, v3;
		v1 = atoi(s.c_str()); //check for error
		if(!(tokenizer >> v2 >> v3))
			throw exception("invalid face syntax");
		faces.push_back(Face(v1 - 1, v2 - 1, v3 - 1)); //- 1 for OBJ file compatibility
	} else {
		unsigned int v1, n1, t1, v2, n2, t2, v3, n3, t3;
		char *format = "%d/%d/%d";
		int successCount;
		successCount = sscanf(s.c_str(), format, &v1, &t1, &n1);
		if(successCount != 3) {
			throw exception("invalid face syntax");
		}
		s.clear();
		tokenizer >> s;
		successCount = sscanf(s.c_str(), format, &v2, &t2, &n2);
		if(successCount != 3) {
			throw exception("invalid face syntax");
		}
		s.clear();
		tokenizer >> s;
		successCount = sscanf(s.c_str(), format, &v3, &t3, &n3);
		if(successCount != 3) {
			throw exception("invalid face syntax");
		}
		faces.push_back(Face(		//- 1 for OBJ file compatibility
			v1 - 1, v2 - 1, v3 - 1, 
			n1 - 1, n2 - 1, n3 - 1, 
			t1 - 1, t2 - 1, t3 - 1
		));
	}
}