#pragma once

#include <glm.hpp>
#include <vector>
#include "Material.h"

class Terrain
{
public:
	Terrain(int sizeX, int sizeY, int stride, int amplitude, bool faceNormals);
	Terrain(glm::vec3& pos, int sizeX, int sizeY, int stride, int amplitude, bool faceNormals);

	void printPoints();

	std::vector<glm::vec3> vertices() { return _points; }
	std::vector<glm::vec2> texCoords() { return _texcoords; }
	std::vector<glm::vec3> normals() { return _normals; }
	glm::vec3 pos() { return _pos; }
	int sizeX() { return _sizeX; }
	int sizeY() { return _sizeY; }
	Material mat() { return _mat; }

private:
	std::vector<glm::vec3> _points;
	std::vector<glm::vec2> _texcoords;
	std::vector<glm::vec3> _normals;

	glm::vec3 _pos;
	int _sizeX;
	int _sizeY;

	Material _mat = Material(glm::vec3(0.5,1,0.8), glm::vec3(0.8,0.8,0.8), glm::vec3(0.1,0.1,0.1), 12);

	void makeTerrain(int sizeX, int sizeY, int stride, int amplitude, bool faceNormals);
	std::vector<std::vector<int>> random_displacement(int sizeX, int sizeY, int amplitude);
	std::vector<std::vector<float>> perlin_displacement(int sizeX, int sizeY, int amplitude);
};

