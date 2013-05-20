#include "Camera.h"

#include <cmath>

using namespace Eigen;

//Projects vec onto the plane defined by normal. The normal should be normalized.
Vector3f ProjectVectorOnPlane(const Vector3f& vec, const Vector3f& normal) {
	return vec - abs(normal.dot(vec)) * normal;
}


Camera::Camera(const Point3f& pivot, const Eigen::Vector3f& dir, 
			   const Eigen::Vector3f& up, float radius, 
			   float nearViewPlane, float farViewPlane, float fovy, 
			   float aspectRatio):
	nearViewPlane(nearViewPlane), farViewPlane(farViewPlane), pivot(pivot), 
	dir(dir.normalized()), up(ProjectVectorOnPlane(up, this->dir).normalized()), 
	radius(radius), fovy(fovy), aspectRatio(aspectRatio) {}
		

Camera::Point3f Camera::getPivotPoint() const {
	return pivot;
}

Camera::Point3f Camera::getPosition() const {
	return pivot - radius * dir;
}

Vector3f Camera::getDirection() const {
	return dir;
}

Vector3f Camera::getUpVector() const {
	return up;
}

float Camera::getRadius() const {
	return radius;
}

float Camera::getVerticalFieldOfView() const {
	return fovy;
}

float Camera::getHorizontalFieldOfView() const {
	return fovy * aspectRatio;
}

float Camera::getAspectRatio() const {
	return aspectRatio;
}

float Camera::getNearViewPlane() const {
	return nearViewPlane;
}

float Camera::getFarViewPlane() const  {
	return farViewPlane;
}


void Camera::setPivotPoint(const Point3f& p) {
	pivot = p;
}

void Camera::setPosition(const Point3f& p) {
	dir = pivot - getPosition(); 
	radius = dir.norm();
	dir.normalize();
	up = ProjectVectorOnPlane(up, dir).normalized();
}

void Camera::setDirection(const Vector3f& dir) {
	this->dir = dir.normalized();
	up = ProjectVectorOnPlane(up, this->dir);
}

void Camera::setUpVector(const Vector3f& up) {
	this->up = ProjectVectorOnPlane(up, dir);
}

void Camera::setRadius(float dist) {
	radius = dist;
}

void Camera::setVerticalFieldOfView(float angle) {
	fovy = angle;
}

void Camera::setAspectRatio(float ratio) {
	aspectRatio = ratio;
}

void Camera::setNearViewPlane(float nearViewPlane) {
	this->nearViewPlane = nearViewPlane;
}

void Camera::setFarViewPlane(float farViewPlane) {
	this->farViewPlane = farViewPlane;
}


void Camera::displace(const Vector2f& disp) {
	pivot += disp[0] * dir.cross(up) + disp[1] * up;
}

void Camera::displace(const Vector3f& disp) {
	displace(Vector2f(disp[0], disp[1]));
	pivot += disp[2] * dir;
}

void Camera::spin(const Vector2f& up) {
	AngleAxisf rot(up.normalized()[1], dir);
	this->up = rot.matrix() * this->up;
}

void Camera::spin(float angle) {
	AngleAxisf rot(angle, -dir);
	up = rot.matrix() * up;
}

float DegreesToRadians(float degrees) { return degrees * M_PI / 180.0f; }

void Camera::rotate(float right, float up) {
	Matrix3f rot = AngleAxisf(DegreesToRadians(right), this->up).matrix() *
				   AngleAxisf(DegreesToRadians(up), this->up.cross(dir)).matrix();
	dir = rot * dir;
	this->up = rot * this->up;
	//this->up = ProjectVectorOnPlane(this->up, dir).normalized();
}

void Camera::zoom(float r) {
	radius /= r;
}

void Camera::look(const Point3f& pivot, const Vector3f& dir, const Vector3f& up,
				  float dist) {
	this->pivot = pivot;
	this->dir = dir.normalized();
	this->up = ProjectVectorOnPlane(up, this->dir).normalized();
	radius = dist;
}

void Camera::lookAt(const Point3f& pivot) {
	Point3f currentPos = getPosition();
	this->pivot = pivot;
	dir = pivot - currentPos; 
	radius = dir.norm();
	dir.normalize();
	up = ProjectVectorOnPlane(up, dir).normalized();
}

void Camera::transform(const Matrix4f& trans) {
	//TODO implement
	//verify up vector
}

Camera operator*(const Matrix4f& mat, const Camera& cam) {
	Camera transCam = cam;
	transCam.transform(mat);
	return transCam;
}