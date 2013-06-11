/********************************************************************
 * FILE: Animation.h
 * =============================
 * 
 * -----------------------------
 * AUTHOR: Stephen Hudson
 ********************************************************************/
#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <Eigen\Core>
#include <Eigen\Geometry>

#include <fstream>


//One joint of a rig.
struct Joint {
	Eigen::Matrix4f bindPoseTransInv;
	int parent; //index of parent joint in skeleton

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

//The collection of joints for a rig in bind pose.
struct Skeleton {
public:
	Skeleton();
	explicit Skeleton(const char *filename);
	Skeleton(const Skeleton& other);
	~Skeleton();
	Skeleton &operator=(const Skeleton& other);

	Joint *joints;
	int nJoints;

private:
	void copy(const Skeleton& other);
	void destroy();
	void setInvalid();
	bool parseSkelFile(std::ifstream &file);
};

//Defines a transformation from a parents position in model space for a specific
//pose and a transfomration matrix ...
struct JointPose {
	JointPose();
	JointPose(float s, const Eigen::Quaternionf& q, const Eigen::Vector3f& t);
	JointPose(const Eigen::Matrix4f& mat): mat(mat){}
	JointPose lerp(const JointPose& other, float t);
	Eigen::Matrix4f transformationMatrix() const;
	Eigen::Vector3f trans;
	Eigen::Quaternionf rot;
	float scale; //only one-dimensional scaling will be allowed
	Eigen::Matrix4f mat;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct MatrixPalette {
public:
	MatrixPalette();
	MatrixPalette(const MatrixPalette& other);
	~MatrixPalette();
	MatrixPalette &operator=(const MatrixPalette& other);

	int numJoints;
	Eigen::Matrix4f *skinningMats;


private:
	void copy(const MatrixPalette &other);
	void destroy();
};

//One pose for a rig.
struct SkeletonPose {
public:
	SkeletonPose(/*args?*/);
	SkeletonPose(JointPose *jointPoses, Skeleton *skel);
	explicit SkeletonPose(const char *filename, Skeleton *skel); //not defined for interpolation
	SkeletonPose(const SkeletonPose& other);
	~SkeletonPose();
	SkeletonPose &operator=(const SkeletonPose& other);

	MatrixPalette buildMatrixPalette();
	void calcWorldSpaceJointsPoses();
	SkeletonPose lerp(const SkeletonPose& other, float t);

	Skeleton *skel;
	JointPose *jointPoses;
	Eigen::Matrix4f *worldSpaceJointTransforms;

private:
	void copy(const SkeletonPose& other);
	void destroy();
};

struct StaticPose {
public:
	StaticPose();
	explicit StaticPose(const char *filename, Skeleton *skel);
	StaticPose(const StaticPose& pose);
	~StaticPose();
	StaticPose &operator=(const StaticPose& other);

	MatrixPalette buildMatrixPalette() const;

	Skeleton *skel;
	Eigen::Matrix4f *worldSpaceJointTransforms;

private:
	bool parsePoseFile(std::ifstream& file);
	void copy(const StaticPose& other);
	void destroy();
	void setInvalid();
};

struct AnimationClip {
public:
	AnimationClip();
	//TODO in ctor, set "end" to the time of the second keyframe
	//TODO enforce that there are at least two keyframes
	AnimationClip(const char *filename, Skeleton *skel);
	AnimationClip(const AnimationClip& other);
	~AnimationClip();
	AnimationClip &operator=(const AnimationClip &other);

	float length() const;
	void reset();
	SkeletonPose interpolatePose(float dt);

	//function that takes two clips with the time they should overlap and the 
	//method for overlapping

	SkeletonPose *keyframes;
	float *times; //note: the time of the first keyframe should always be 0sec.
	int nPoses;

private:
	bool parseAnimFile(std::ifstream& file, Skeleton *skel);
	void copy(const AnimationClip& other);
	void destroy();
	void setInvalid();
	int begin, end; //cached indices of the last referenced keyframes
	//TODO since these values are always 1 apart, only one of them must be cached
	float time;
};

//to interpolate, you need two sets of adjacent skeleton poses and their respective times,
//a time to which to interpolatel

//Think about REPEATING animation clips and times that are out of bounds of the animation.

//The set of smooth skinning weights associated with a particular
//mesh
struct Skin {
public:
	Skin();
	explicit Skin(const char *filename/*, pointer to Mesh*/);
	Skin(const Skin &other);
	~Skin();
	Skin &operator=(const Skin &other);

	int numVerts;
	struct JointWeighting {
		int jointIndex[4];
		float weight[4];
		int numWeights;
	} *jointWeights;
	//pointer to mesh
	
private:
	void copy(const Skin &other); 
	void destroy();
	bool parseSkinFile(std::ifstream &file);
	void setInvalid();
};

#endif //ANIMATION_H