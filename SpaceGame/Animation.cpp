#include "Animation.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace Eigen;

static float DegreesToRadians(float degrees) { return degrees * (float)M_PI / 180.0f; }

Skeleton::Skeleton() {
	joints = NULL;
	nJoints = 0;
}

Skeleton::Skeleton(const char *filename) {
	
	//check that the extension type is correct
	string fileExt = ".skel";
	string filename_str(filename);
	if(filename_str.substr(filename_str.length() - fileExt.length()) != fileExt)
		setInvalid();
	else { //read in file
		ifstream skelFile(filename);
		if(!skelFile.is_open() || !parseSkelFile(skelFile)) {
			setInvalid();
			cerr << filename << " could not be loaded." << endl;
		}
		skelFile.close();
	}
}

Skeleton::Skeleton(const Skeleton& other) {
	copy(other);
}

Skeleton::~Skeleton() {
	destroy();
}

Skeleton &Skeleton::operator=(const Skeleton& other) {
	if(this != &other) {
		destroy();
		copy(other);
	}
	return *this;
}

void Skeleton::copy(const Skeleton& other) {
	nJoints = other.nJoints;
	if(other.joints != NULL) {
		joints = new Joint[nJoints];
		memcpy(joints, other.joints, sizeof(Joint) * nJoints);
	} else
		joints = NULL;
}

void Skeleton::destroy() {
	if(joints != NULL) delete[] joints;
}

/*Sets the skeleton to an invalid state for use when the file
 *was not read correctly.
 */
void Skeleton::setInvalid() {
	joints = NULL;
	nJoints = 0;
}

//Parses file into member variables
bool Skeleton::parseSkelFile(ifstream &file) {
	string line;
	getline(file, line);
	stringstream parser(line);
	if(!(parser >> nJoints)) return false;
	joints = new Joint[nJoints];
	for(int i = 0; i < nJoints; ++i) {
		if(!getline(file, line)) {
			delete[] joints;
			return false;
		}
		stringstream parser(line);
		string jointName;
		parser >> jointName;
		Joint j;
		float mat[3][4];
		if(!(
			parser >> j.parent >> 
			mat[0][0] >> mat[0][1] >> mat[0][2] >> mat[0][3] >> 
			mat[1][0] >> mat[1][1] >> mat[1][2] >> mat[1][3] >>
			mat[2][0] >> mat[2][1] >> mat[2][2] >> mat[2][3]
		)) {
			delete[] joints;
			return false;
		}
		for(int row = 0; row < 3; ++row)
			for(int col = 0; col < 4; ++col)
				j.bindPoseTransInv(col*4 + row) = mat[row][col];
		j.bindPoseTransInv(3) = 0;
		j.bindPoseTransInv(7) = 0;
		j.bindPoseTransInv(11) = 0;
		j.bindPoseTransInv(15) = 1;
		joints[i] = j;
	}
	return true;
}



////Returns true if n lies between min and max.

//
////float comparison function for bsearch
//int lessThan(const void *a, const void *b) { 
//	return *(float *)a < *(float *)b; 
//}
//
//int BinarySearch(float key, float array[], int size) {
//	//split the array in half
//
//	//see which half the key is in
//
//	//recurse on that half
//	if(size == 1) return (array[0] < key) array[0];
//	if(array[size / 2] < key)
//		return BinarySearch(key, array + size / 2, (size+1)/ 2);
//	else
//		return BinarySearch(key, array, size / 2);
//}
//
//AnimationClip::AnimationClip(): begin(0), end(0) {}

AnimationClip::AnimationClip(): times(NULL), keyframes(NULL), nPoses(0), time(0) {}

AnimationClip::AnimationClip(const char *filename, Skeleton *skel): time(0) {
	//check that the extension type is correct
	string fileExt = ".anim";
	string filename_str(filename);
	if(filename_str.substr(filename_str.length() - fileExt.length()) != fileExt)
		setInvalid();
	else { //read in file
		ifstream animFile(filename);
		if(!animFile.is_open() || !parseAnimFile(animFile, skel)) {
			setInvalid();
			cerr << filename << " could not be loaded." << endl;
		}
		animFile.close();
	}
}

AnimationClip::AnimationClip(const AnimationClip& other) {
	copy(other);
}

AnimationClip::~AnimationClip() {
	destroy();
}

AnimationClip &AnimationClip::operator=(const AnimationClip& other) {
	if(this != &other) {
		destroy();
		copy(other);
	}
	return *this;
}

void AnimationClip::copy(const AnimationClip& other) {
	nPoses = other.nPoses;
	time = 0.0f;
	if(nPoses != 0) {
		times = new float[nPoses];
		memcpy(times, other.times, sizeof(float) * nPoses);
		keyframes = new SkeletonPose[nPoses];
		for(int i = 0; i < nPoses; ++i)
			keyframes[i] = other.keyframes[i];
	} else {
		times = NULL;
		keyframes = NULL;
	}
}

void AnimationClip::destroy() {
	if(times != NULL) delete[] times;
	if(keyframes != NULL) delete[] keyframes;
}

void AnimationClip::setInvalid() {
	nPoses = 0;
	keyframes = NULL;
	times = NULL;
}

bool AnimationClip::parseAnimFile(ifstream& file, Skeleton *skel) {
	string line, emptyLine;
	if(!getline(file, line)) return false;
	stringstream parser(line);
	int numJoints;
	if(!(parser >> numJoints >> nPoses)) return false;
	numJoints = skel->nJoints;
	keyframes = new SkeletonPose[nPoses];
	times = new float[nPoses];
	for(int i = 0; i < nPoses; ++i) {
		if(!getline(file, emptyLine) || !getline(file, line)) {
			delete[] keyframes;
			delete[] times;
			return false;
		}
		stringstream parser(line);
		if(!(parser >> times[i])) {
			delete[] keyframes;
			delete[] times;
		}
		JointPose *jointPoses = new JointPose[numJoints];
		for(int j = 0; j < numJoints; ++j) {
			if(!getline(file, line)) {
				delete[] jointPoses;
				delete[] keyframes;
				delete[] times;
				return false;
			}
			stringstream parser(line);
			float scale, angleW, angleX, angleY, angleZ, transX, transY, transZ;
			//float mat[16];
			if(!(
				parser >> scale >> 
				angleW >> angleX >> angleY >> angleZ >> 
				transX >> transY >> transZ
				//parser >> mat[0] >> mat[1] >> mat[2] >> mat[3]
				//	   >> mat[4] >> mat[5] >> mat[6] >> mat[7]
				//	   >> mat[8] >> mat[9] >> mat[10] >> mat[11]
				//	   >> mat[12] >> mat[13] >> mat[14] >> mat[15]
			)) {
				delete[] jointPoses;
				delete[] keyframes;
				delete[] times;
				return false;
			}
			Quaternionf rot(angleW, angleX, angleY, angleZ);
			jointPoses[j] = JointPose(scale, rot, Vector3f(transX, transY, transZ)); 
			//jointPoses[j] = JointPose(Matrix4f(mat));
		}
		keyframes[i] = SkeletonPose(jointPoses, skel);
	}
	return true;
}

float AnimationClip::length() const {
	return times[nPoses - 1];
}

void AnimationClip::reset() { time = 0.0f; }

void AnimationClip::setSkeleton(Skeleton *skel) {
	for(int i = 0; i < nPoses; ++i)
		keyframes[i].skel = skel;
}

SkeletonPose AnimationClip::interpolatePose(float dt) {
	//find the two skeletonPoses around this time
	//return their interpolation
	time += dt;
	assert(time >= 0.0f);
	time = (time/length() - floor(time/length()))*length();
	//if time is greater than length
	int i;
	if(time == 0.0f)
		return keyframes[0];
	for(i = 0; i < nPoses; ++i) {
		if(time <= times[i]) break;
	}
	if(i == nPoses)
		return keyframes[nPoses - 1];
	else {
		float t = (time - times[i-1])/(times[i]-times[i-1]);
		return keyframes[i-1].lerp(keyframes[i], t);
	}
}


Skin::Skin() {
	numVerts = 0;
	jointWeights = NULL;
}

Skin::Skin(const char *filename/*, pointer to Mesh*/) {
	//check that the extension type is correct
	string fileExt = ".skin";
	string filename_str(filename);
	if(filename_str.substr(filename_str.length() - fileExt.length()) != fileExt)
		setInvalid();
	else { //read in file
		ifstream skinFile(filename);
		if(!skinFile.is_open() || !parseSkinFile(skinFile)) {
			setInvalid();
			cerr << filename << " could not be loaded." << endl;
		}
		skinFile.close();
	}
}

Skin::Skin(const Skin &other) {
	copy(other);
}

Skin::~Skin() {
	destroy();
}

Skin &Skin::operator=(const Skin &other) {
	if(this != &other) {
		destroy();
		copy(other);
	}
	return *this;
}

bool Skin::parseSkinFile(ifstream &file) {
	string line;
	getline(file, line);
	stringstream parser(line);
	if(!(parser >> numVerts)) return false;
	jointWeights = new JointWeighting[numVerts];
	for(int i = 0; i < numVerts; ++i) {
		if(!getline(file, line)) {
			delete[] jointWeights;
			return false;
		}
		stringstream parser(line);
		JointWeighting jw;
		int vertIndex;
		if(!(parser >> vertIndex >> jw.numWeights)) {
			delete[] jointWeights;
			return false;
		} else {
			for(int j = 0; j < jw.numWeights; ++j) {
				if(!(parser >> jw.jointIndex[j] >> jw.weight[j])) {
					delete[] jointWeights;
					return false;
				}
			}
		}

		jointWeights[i] = jw;
	}
	return true;
}

void Skin::setInvalid() {
	numVerts = 0;
	jointWeights = NULL;
}

void Skin::copy(const Skin &other) {
	numVerts = other.numVerts;
	if(other.jointWeights != NULL) {
		jointWeights = new JointWeighting[numVerts];
		memcpy(jointWeights, other.jointWeights, sizeof(JointWeighting) * numVerts);
	} else
		jointWeights = NULL;
}

void Skin::destroy() {
	if(jointWeights != NULL) delete[] jointWeights;
}



MatrixPalette::MatrixPalette(): numJoints(0), skinningMats(NULL) {}

MatrixPalette::MatrixPalette(const MatrixPalette& other) {
	copy(other);
}

MatrixPalette::~MatrixPalette() {
	destroy();
}

MatrixPalette &MatrixPalette::operator=(const MatrixPalette& other) {
	if(this != &other) {
		destroy();
		copy(other);
	}
	return *this;
}

void MatrixPalette::copy(const MatrixPalette &other) {
	numJoints = other.numJoints;
	if(other.skinningMats != NULL) {
		skinningMats = new Matrix4f[numJoints];
		memcpy(skinningMats, other.skinningMats, sizeof(Matrix4f) * numJoints);
	} else 
		skinningMats = NULL;
}

void MatrixPalette::destroy() {
	if(skinningMats != NULL) delete[] skinningMats;
}



//Converts the joint poise data into a transformation matrix
Matrix4f JointPose::transformationMatrix() const {
	Matrix3f transform3 = Matrix3f::Identity() * scale
					    * AngleAxisf(rot).matrix();
	Matrix4f transform4;

	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j)
			transform4(4*j + i) = transform3(3*j + i);
		transform4(12 + i) = trans[i];
	}
	transform4(3) = 0;
	transform4(7) = 0;
	transform4(11) = 0;
	transform4(15) = 1;
	
	//Matrix4f translate = Matrix4f::Identity();
	//translate(12) = trans[0];
	//translate(13) = trans[1];
	//translate(14) = trans[2];
	//return transform4 * translate;
	return transform4;
	//return mat;
}

static bool InRange(float n, float min, float max) {
	return n >= min && n <= max;
}

JointPose JointPose::lerp(const JointPose& other, float t) {
		assert(InRange(t, 0.0f, 1.0f));
		JointPose newPose;
		float tLeft = 1 - t;
		//newPose.mat = t*mat + tLeft*other.mat;
		newPose.trans = t*trans + (tLeft)*other.trans;
		newPose.scale = t*scale + (tLeft)*other.scale;
		newPose.rot = rot.slerp(t, other.rot);
		return newPose;
}


JointPose::JointPose() {}

JointPose::JointPose(float s, const Quaternionf& q, const Vector3f& t):
	scale(s), rot(q), trans(t) {}



SkeletonPose::SkeletonPose(): 
	jointPoses(NULL), skel(NULL), worldSpaceJointTransforms(NULL) {}

SkeletonPose::SkeletonPose(JointPose *jointPoses, Skeleton *skel):
	jointPoses(jointPoses), skel(skel), worldSpaceJointTransforms(NULL) {}

SkeletonPose::SkeletonPose(const SkeletonPose& other) {
	copy(other);
}

SkeletonPose::~SkeletonPose() {
	destroy();
}

SkeletonPose &SkeletonPose::operator=(const SkeletonPose& other) {
	if(this != &other) {
		destroy();
		copy(other);
	}
	return *this;
}

void SkeletonPose::copy(const SkeletonPose& other) {
	skel = other.skel;
	if(other.jointPoses != NULL) {
		jointPoses = new JointPose[skel->nJoints];
		memcpy(jointPoses, other.jointPoses, sizeof(JointPose) * skel->nJoints);
	} else 
		jointPoses = NULL;
	if(other.worldSpaceJointTransforms != NULL) {
		worldSpaceJointTransforms = new Matrix4f[skel->nJoints];
		memcpy(
			worldSpaceJointTransforms, other.worldSpaceJointTransforms,
			sizeof(Matrix4f) * skel->nJoints
		);
	} else
		worldSpaceJointTransforms = NULL;
}

void SkeletonPose::destroy() {
	if(jointPoses != NULL) delete[] jointPoses;
	if(worldSpaceJointTransforms != NULL) delete[] worldSpaceJointTransforms;
}

MatrixPalette SkeletonPose::buildMatrixPalette() {
	MatrixPalette matPal;
	matPal.numJoints = skel->nJoints;
	if(matPal.numJoints == 0) {
		matPal.skinningMats = NULL;
		return matPal;
	} else
		matPal.skinningMats = new Matrix4f[matPal.numJoints];
	calcWorldSpaceJointsPoses();
	for(int i = 0; i < skel->nJoints; ++i) {
		//transform to joint space with inverse bind pose matrix
		//transform to world space

		matPal.skinningMats[i] = skel->joints[i].bindPoseTransInv;
		matPal.skinningMats[i] = worldSpaceJointTransforms[i]
							   * matPal.skinningMats[i];
	}
	return matPal;
}

void SkeletonPose::calcWorldSpaceJointsPoses() {
	if(worldSpaceJointTransforms == NULL) {
		if(skel->nJoints == 0) return;
		worldSpaceJointTransforms = new Matrix4f[skel->nJoints];
	}
	worldSpaceJointTransforms[0] = jointPoses[0].transformationMatrix();
	for(int i = 1; i < skel->nJoints; ++i) {
		//calculate the global position of every vertex
		//find the matrix, pre-multiply it by its parent
		worldSpaceJointTransforms[i] = jointPoses[i].transformationMatrix();
		worldSpaceJointTransforms[i] = worldSpaceJointTransforms[skel->joints[i].parent]
									 * worldSpaceJointTransforms[i];
		//worldSpaceJointTransforms[i] = worldSpaceJointTransforms[skel->joints[i].parent];
		//worldSpaceJointTransforms[i] = jointPoses[i].transformationMatrix()
		//							 * worldSpaceJointTransforms[i];

	}
}

SkeletonPose SkeletonPose::lerp(const SkeletonPose& other, float t) {
	if(true) { //skeletons are compatible and initialized
		JointPose *jp = new JointPose[skel->nJoints];
		for(int i = 0; i < skel->nJoints; ++i) {
			jp[i] = jointPoses[i].lerp(other.jointPoses[i], t);
		}
		return SkeletonPose(jp, this->skel);
	}
}



StaticPose::StaticPose(): worldSpaceJointTransforms(NULL), skel(NULL) {}

StaticPose::StaticPose(const char *filename, Skeleton *skel): skel(skel) {
	//check that the extension type is correct
	string fileExt = ".pose";
	string filename_str(filename);
	if(filename_str.substr(filename_str.length() - fileExt.length()) != fileExt)
		setInvalid();
	else { //read in file
		ifstream poseFile(filename);
		if(!poseFile.is_open() || !parsePoseFile(poseFile))
			setInvalid();
		poseFile.close();
	}
}

StaticPose::StaticPose(const StaticPose& other) {
	copy(other);
}

StaticPose::~StaticPose() {
	destroy();
}

StaticPose &StaticPose::operator=(const StaticPose& other) {
	if(this != &other) {
		destroy();
		copy(other);
	}
	return *this;
}

MatrixPalette StaticPose::buildMatrixPalette() const {
	MatrixPalette matPal;
	matPal.numJoints = skel->nJoints;
	if(matPal.numJoints == 0) {
		matPal.skinningMats = NULL;
		return matPal;
	} else
		matPal.skinningMats = new Matrix4f[matPal.numJoints];
	for(int i = 0; i < skel->nJoints; ++i) {
		//transform to joint space with inverse bind pose matrix
		//transform to world space

		matPal.skinningMats[i] = skel->joints[i].bindPoseTransInv;
		matPal.skinningMats[i] = worldSpaceJointTransforms[i]
							   * matPal.skinningMats[i];
	}
	return matPal;
}

bool StaticPose::parsePoseFile(ifstream& file) {
	string line;
	getline(file, line);
	stringstream parser(line);
	worldSpaceJointTransforms = new Matrix4f[skel->nJoints];
	for(int i = 0; i < skel->nJoints; ++i) {
		if(!getline(file, line)) {
			delete[] worldSpaceJointTransforms;
			return false;
		}
		stringstream parser(line);
		if(!(
			parser >> 
			worldSpaceJointTransforms[i](0) >> worldSpaceJointTransforms[i](4) >> 
			worldSpaceJointTransforms[i](8) >> worldSpaceJointTransforms[i](12) >> 
			worldSpaceJointTransforms[i](1) >> worldSpaceJointTransforms[i](5) >> 
			worldSpaceJointTransforms[i](9) >> worldSpaceJointTransforms[i](13) >>
			worldSpaceJointTransforms[i](2) >> worldSpaceJointTransforms[i](6) >> 
			worldSpaceJointTransforms[i](10) >> worldSpaceJointTransforms[i](14)
		)) {
			delete[] worldSpaceJointTransforms;
			return false;
		}
		worldSpaceJointTransforms[i](3) = 0;
		worldSpaceJointTransforms[i](7) = 0;
		worldSpaceJointTransforms[i](11) = 0;
		worldSpaceJointTransforms[i](15) = 1;
	}

	//for(int i = 1; i < skel->nJoints; ++i) {
	//	worldSpaceJointTransforms[i] = worldSpaceJointTransforms[skel->joints[i].parent] * worldSpaceJointTransforms[i];
	//}

	return true;
}

void StaticPose::copy(const StaticPose& other) {
	skel = other.skel;
	if(other.worldSpaceJointTransforms != NULL) {
		worldSpaceJointTransforms = new Matrix4f[skel->nJoints];
		memcpy(worldSpaceJointTransforms, other.worldSpaceJointTransforms, sizeof(Matrix4f) * skel->nJoints);
	} else
		worldSpaceJointTransforms = NULL;
}

void StaticPose::destroy() {
	if(worldSpaceJointTransforms != NULL) delete[] worldSpaceJointTransforms;
}

void StaticPose::setInvalid() {
	worldSpaceJointTransforms = NULL;
	skel = NULL;
}