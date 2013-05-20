/*****************************************************************************
 * FILE: Camera.h
 * ==========================
 * This file defines the Camera class for maitaining and manipulating the
 * position of the camera in the scene.  It also stores parameters for the 
 * perspective viewing transform.
 * --------------------------
 * AUTHOR: Stephen Hudson
 * LAST UPDATED: 5/19/2013
 * TODO:
 * >> implement Matrix transformation
 *****************************************************************************/

#ifndef _CAMERA_H
#define _CAMERA_H

#include <Eigen\Core>
#include <Eigen\Geometry>
/*
 * CLASS: Camera
 * =========================
 * DESCRIPTION: A camera that stores both the parameters for a perspective 
 * transformation as well as its position and orientation in space.
 * -------------------------
 * FUNCTIONALITY: Allows for a third person view in which the camera spherically
 * rotates about a pivot point.  The user can specify the distance from the 
 * camera from this pivot. If this distance is 0, then you get a first person
 * view with rotation about the focal point.
 * -------------------------
 * NOTES: All angles should be represented in degrees.
 */
class Camera {

public:

	typedef Eigen::Vector3f Point3f;

	//(note: the up vector given will be projected into the plane normal to the
	//the direction)
	Camera(const Point3f& pivot, const Eigen::Vector3f& dir, 
		   const Eigen::Vector3f& up, float radius, float nearViewPlane, 
		   float farViewPlane, float fovy = 135.0f, float aspectRatio = (4.0f/3.0f));


	//GETTERS

	Point3f getPivotPoint() const;
	Point3f getPosition() const; //returns position of camera in world space
	Eigen::Vector3f getDirection() const;
	Eigen::Vector3f getUpVector() const;
	float getRadius() const;
	float getVerticalFieldOfView() const;
	float getHorizontalFieldOfView() const;
	float getAspectRatio() const;
	float getNearViewPlane() const;
	float getFarViewPlane() const;


	//SETTERS: All of these functions, unless otherwise noted, change the specified
	//parameter WITHOUT affecting the other parameters.

	void setPivotPoint(const Point3f& p);
	//Changes the direction and the radius while keeping the pivot constant.
	void setPosition(const Point3f& p);
	void setDirection(const Eigen::Vector3f& dir);
	//Projects the given 3D vector into the plane normal to the direction
	void setUpVector(const Eigen::Vector3f& up);  
	void setRadius(float dist);
	void setVerticalFieldOfView(float angle);
	void setAspectRatio(float ratio);
	void setNearViewPlane(float nearViewPlane);
	void setFarViewPlane(float farViewPlane);

	//Displace the pivot along the plane normal to the direction. The positive
	//y axis is the camera up vector.
	void displace(const Eigen::Vector2f& disp);
	//Displace the pivot with the orthogonal basis as defined by the camera position
	//and orientation.  However, a positive z value will move the pivot forward.
	void displace(const Eigen::Vector3f& disp);

	//Changes the up vector in the plane normal to the direction. The current up
	//vector in the plane is (0, 1).
	void spin(const Eigen::Vector2f& up);
	//Spins the camera counterclockwise such that the direction is constant.
	void spin(float angle);

	//Rotates the camera around the sphere defined by the pivot point.
	void rotate(float right, float up);

	//Zooms the camera such that everything is magnified to the ratio given.
	void zoom(float r);

	//Reset the pivot, direction, up vector and radius.
	void look(const Point3f& pivot, const Eigen::Vector3f& dir, 
			  const Eigen::Vector3f& up, float dist);
	//Change the pivot to p (and with it the direction and radius) while keeping the 
	//position constant.
	void lookAt(const Point3f& pivot);

	//Multiply all camera parameters by the transformation matrix.
	void transform(const Eigen::Matrix4f& trans);

private:
	float nearViewPlane, farViewPlane, radius, fovy, aspectRatio;
	Point3f pivot;
	Eigen::Vector3f dir, up;
};

Camera operator*(const Eigen::Matrix4f& mat, const Camera& cam);

#endif //_CAMERA_H