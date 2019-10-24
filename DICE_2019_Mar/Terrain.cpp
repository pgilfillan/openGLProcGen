#include "Terrain.h"

#include <iostream>
#include <random>
#include <array>
#include <limits>

#include "glm/gtx/string_cast.hpp"
#include "Perlin.h"

Terrain::Terrain(int sizeX, int sizeY, int stride, int amplitude, bool faceNormals) {
	_pos = glm::vec3(0,0,0);
	makeTerrain(sizeX, sizeY, stride, amplitude, faceNormals);
}

Terrain::Terrain(glm::vec3& pos, const int sizeX, const int sizeY, const int stride, int amplitude, bool faceNormals) {
	_pos = pos;
	makeTerrain(sizeX, sizeY, stride, amplitude, faceNormals);
}

void Terrain::makeTerrain(int sizeX, int sizeY, int stride, int amplitude, bool faceNormals) {
	_sizeX = sizeX * stride;
	_sizeY = sizeY * stride;

	//Generate heightmap
	auto heights = perlin_displacement(sizeX, sizeY, amplitude);

	//Generate terrain
	std::vector<std::vector<std::vector<glm::vec3>>> faceNormalsList;
	if (!faceNormals) {
		for (int i = 0; i <= sizeX; ++i) {
			faceNormalsList.push_back(std::vector<std::vector<glm::vec3>>(sizeY+1));
		}
	}

	for (int i = 0; i < sizeX; ++i) {
		for (int j = 0; j < sizeX; ++j) {
			//Lower right triangle in square (vertices and normals)
			glm::vec3 point(i*stride, heights[i][j], j*stride);
			_points.push_back(point);
			point = glm::vec3((i + 1)*stride, heights[i+1][j+1], (j + 1)*stride);
			_points.push_back(point);
			point = glm::vec3((i+1)*stride, heights[i+1][j], j*stride);
			_points.push_back(point);
			//Upper left triangle in square (vertices and normals)
			point = glm::vec3(i*stride, heights[i][j], j*stride);
			_points.push_back(point);
			point = glm::vec3(i*stride, heights[i][j+1], (j + 1)*stride);
			_points.push_back(point);
			point = glm::vec3((i + 1)*stride, heights[i+1][j+1], (j + 1)*stride);
			_points.push_back(point);

			//Lower right triangle in square (tex coords)
			glm::vec2 coord(0,0);
			_texcoords.push_back(coord);
			coord = glm::vec2(1, 1);
			_texcoords.push_back(coord);
			coord = glm::vec2(1, 0);
			_texcoords.push_back(coord);
			//Upper left triangle in square (tex coords)
			coord = glm::vec2(0, 0);
			_texcoords.push_back(coord);
			coord = glm::vec2(0, 1);
			_texcoords.push_back(coord);
			coord = glm::vec2(1, 1);
			_texcoords.push_back(coord);

			if (faceNormals) {
				//Lower right triangle in square (normals)
				auto size = _points.size();
				auto normal = glm::normalize(glm::cross(_points[size - 5] - _points[size - 6], _points[size - 4] - _points[size - 6]));
				_normals.push_back(normal);
				_normals.push_back(normal);
				_normals.push_back(normal);
				//Upper right triangle in square (normals)
				normal = glm::normalize(glm::cross(_points[size - 2] - _points[size - 3], _points[size - 1] - _points[size - 3]));
				_normals.push_back(normal);
				_normals.push_back(normal);
				_normals.push_back(normal);
			} else {
				auto size = _points.size();
				auto normal = glm::cross(_points[size - 5] - _points[size - 6], _points[size - 4] - _points[size - 6]);
				faceNormalsList[i][j].push_back(normal);
				faceNormalsList[i+1][j].push_back(normal);
				faceNormalsList[i+1][j+1].push_back(normal);
				//Upper right triangle in square (normals)
				normal = glm::cross(_points[size - 2] - _points[size - 3], _points[size - 1] - _points[size - 3]);
				faceNormalsList[i][j].push_back(normal);
				faceNormalsList[i][j + 1].push_back(normal);
				faceNormalsList[i + 1][j + 1].push_back(normal);
			}
		}
	}

	if (!faceNormals) {
		for (int i = 0; i < sizeX; ++i) {
			for (int j = 0; j < sizeY; ++j) {
				//Lower right
				int count = 0;
				glm::vec3 normal(0.0f,0.0f,0.0f);
				for (glm::vec3 n : faceNormalsList[i][j]) {
					normal += n;
					++count;
				}
				normal /= count;
				_normals.push_back(normal);
				count = 0;
				normal = glm::vec3(0.0f, 0.0f, 0.0f);
				for (glm::vec3 n : faceNormalsList[i+1][j+1]) {
					normal += n;
					++count;
				}
				normal /= count;
				_normals.push_back(normal);
				count = 0;
				normal = glm::vec3(0.0f, 0.0f, 0.0f);
				for (glm::vec3 n : faceNormalsList[i+1][j]) {
					normal += n;
					++count;
				}
				normal /= count;
				_normals.push_back(normal);

				//Upper left
				count = 0;
				normal = glm::vec3(0.0f, 0.0f, 0.0f);
				for (glm::vec3 n : faceNormalsList[i][j]) {
					normal += n;
					++count;
				}
				normal /= count;
				_normals.push_back(normal);
				count = 0;
				normal = glm::vec3(0.0f, 0.0f, 0.0f);
				for (glm::vec3 n : faceNormalsList[i][j + 1]) {
					normal += n;
					++count;
				}
				normal /= count;
				_normals.push_back(normal);
				count = 0;
				normal = glm::vec3(0.0f, 0.0f, 0.0f);
				for (glm::vec3 n : faceNormalsList[i + 1][j+1]) {
					normal += n;
					++count;
				}
				normal /= count;
				_normals.push_back(normal);
			}
		}
	}

	std::cout << "Finished generating terrain" << '\n';
}

std::vector<std::vector<int>> Terrain::random_displacement(int sizeX, int sizeY, int amplitude) {
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, amplitude);
	std::vector<std::vector<int>> heights;
	for (int i = 0; i <= sizeX; ++i) {
		std::vector<int> row;
		for (int j = 0; j <= sizeY; ++j) {
			row.push_back(distr(eng));
		}
		heights.push_back(row);
	}

	return heights;
}

std::vector<std::vector<float>> Terrain::perlin_displacement(int sizeX, int sizeY, int amplitude) {
	double frequency = 8;
	int octaves = 4;

	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, std::numeric_limits<int>::max());
	std::uint32_t seed = distr(eng);

	const siv::PerlinNoise perlin(seed);
	const double fx = sizeX / frequency;
	const double fy = sizeY / frequency;
	std::vector<std::vector<float>> heights;
	for (int i = 0; i <= sizeX; ++i) {
		std::vector<float> row;
		for (int j = 0; j <= sizeY; ++j) {
			float noise = float(perlin.octaveNoise0_1(i / fx, j / fy, octaves))*amplitude;
			row.push_back(noise);
		}
		heights.push_back(row);
	}

	return heights;
}

void Terrain::printPoints() {
	std::cout << "Printing terrain" << '\n';
	std::cout << _points.size() << '\n';
	for (auto& point : _points) {
		std::cout << glm::to_string(point) << '\n';
	}
}
