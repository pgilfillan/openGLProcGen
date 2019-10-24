#pragma once

#include <glm.hpp>

struct Material {
	glm::vec3 ambientCoeff;
	glm::vec3 diffuseCoeff;
	glm::vec3 specularCoeff;
	float phongExponent;

	Material(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float phong) :
		ambientCoeff(amb), diffuseCoeff(diff), specularCoeff(spec), phongExponent(phong) {}
};