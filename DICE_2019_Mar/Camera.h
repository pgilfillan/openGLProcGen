#pragma once

#include "glm/glm.hpp"

class Camera {

public:
	Camera(const glm::vec3& pos, const glm::vec3& lookAtPoint, const glm::vec3& upVec);
	void setPos(const glm::vec3& newPos) { _pos = newPos; }
	glm::vec3& getPos() { return _pos; }
	void lookAt(const glm::vec3& newPos, const glm::vec3& lookAtPoint, const glm::vec3& upVec);
	void lookAt(const glm::vec3& lookAtPoint, const glm::vec3& upVec);
	void lookAt(const glm::vec3& lookAtPoint) { _lookAtPoint = lookAtPoint; }
	const glm::mat4 getView();

private:
	glm::vec3 _pos;
	glm::vec3 _lookAtPoint;
	glm::vec3 _upVec;

};