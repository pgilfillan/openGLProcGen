#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(const glm::vec3& pos, const glm::vec3& lookAtPoint, const glm::vec3& upVec) {
	lookAt(pos, lookAtPoint, upVec);
}

void Camera::lookAt(const glm::vec3& newPos, const glm::vec3& lookAtPoint, const glm::vec3& upVec) {
	_pos = newPos;
	_lookAtPoint = lookAtPoint;
	_upVec = upVec;
}

void Camera::lookAt(const glm::vec3& lookAtPoint, const glm::vec3& upVec) {
	_lookAtPoint = lookAtPoint;
	_upVec = upVec;
}

const glm::mat4 Camera::getView() {
	return glm::lookAt(_pos, _lookAtPoint, _upVec);
}