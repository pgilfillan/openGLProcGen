#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Shader.h"
#include "Util.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "Terrain.h"

GLFWwindow *window;

int glfwSetUp();
int glewSetUp();
int drawCube();
int drawTerrain();

int main() {
	return drawTerrain();
}

int drawTerrain() {
	int glfwInit = glfwSetUp();
	if (glfwInit) return glfwInit;

	int glewInit = glewSetUp();
	if (glewInit) return glewInit;

	//Set up vertex array
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Enable input
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Set up shaders
	Shader shader("Resources/Shaders/simpleVShader.vs", "Resources/Shaders/simpleFShader.fs");

	//Make terrain
	Terrain terrain(64, 64, 1, 6, false);

	//Initialise camera
	Camera cam = Camera(glm::vec3(-20, 70, -20), 
						glm::vec3(terrain.pos().x + terrain.sizeX()/2, 0, terrain.pos().y + terrain.sizeY() / 2),
						glm::vec3(0, 1, 0));

	//Initialise MVP matrix
	GLuint matrixID = glGetUniformLocation(shader.getProgram(), "MVP");
	GLuint modelMatrixID = glGetUniformLocation(shader.getProgram(), "M");
	GLuint viewMatrixID = glGetUniformLocation(shader.getProgram(), "V");
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 300.0f);
	glm::mat4 View = cam.getView();
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	//Load textures
	ResourceManager resManager;
	GLuint firstTex = resManager.loadBMP("Resources/Textures/grass.bmp");
	if (!firstTex) Util::panic("Texture didn't load properly\n");
	GLuint textureID = glGetUniformLocation(shader.getProgram(), "myTextureSampler");
	GLuint secondTex = resManager.loadBMP("Resources/Textures/stone.bmp");
	if (!secondTex) Util::panic("Texture didn't load properly\n");
	GLuint textureID2 = glGetUniformLocation(shader.getProgram(), "myTextureSampler2");

	//Setup lighting
	glm::vec3 lightDir(2, 1, 2);
	glm::vec3 lightIntensity(1,1,1);
	glm::vec3 ambientIntesity(0.5, 0.5, 0.5);
	GLuint lightDirID = glGetUniformLocation(shader.getProgram(), "lightDir");
	GLuint lightIntensityID = glGetUniformLocation(shader.getProgram(), "lightIntensity");
	GLuint ambientIntensityID = glGetUniformLocation(shader.getProgram(), "ambientIntensity");

	//Setup material
	GLuint matAmbID = glGetUniformLocation(shader.getProgram(), "ambientCoeff");
	GLuint matDiffID = glGetUniformLocation(shader.getProgram(), "diffuseCoeff");
	GLuint matSpecID = glGetUniformLocation(shader.getProgram(), "specularCoeff");
	GLuint matPhongID = glGetUniformLocation(shader.getProgram(), "phongExp");

	//Set up buffers
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, terrain.vertices().size() * sizeof(glm::vec3), &terrain.vertices()[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, terrain.texCoords().size() * sizeof(glm::vec2), &terrain.texCoords()[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, terrain.normals().size() * sizeof(glm::vec3), &terrain.normals()[0], GL_STATIC_DRAW);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Draw window, exit on ShouldClose or escape key
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.getProgram());

		//Set uniforms
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &View[0][0]);

		glUniform3fv(lightDirID, 1, &lightDir[0]);
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);
		glUniform3fv(ambientIntensityID, 1, &ambientIntesity[0]);

		glUniform3fv(matAmbID, 1, &terrain.mat().ambientCoeff[0]);
		glUniform3fv(matDiffID, 1, &terrain.mat().diffuseCoeff[0]);
		glUniform3fv(matSpecID, 1, &terrain.mat().specularCoeff[0]);
		glUniform1f(matPhongID, terrain.mat().phongExponent);

		//Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, firstTex);
		glUniform1i(textureID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, terrain.vertices().size() * 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//Cleanup
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(shader.getProgram());
	glDeleteTextures(1, &firstTex);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();
	return 0;
}

int drawCube() {
	int glfwInit = glfwSetUp();
	if (glfwInit) return glfwInit;

	int glewInit = glewSetUp();
	if (glewInit) return glewInit;

	//Set up vertex array
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Enable input
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Set up shaders
	Shader shader("Resources/Shaders/simpleVShader.vs", "Resources/Shaders/simpleFShader.fs");

	//Initialise camera
	Camera cam = Camera(glm::vec3(7, 5, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//Initialise MVP matrix
	GLuint matrixID = glGetUniformLocation(shader.getProgram(), "MVP");
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = cam.getView();
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	ResourceManager resManager;
	//Load textures
	GLuint firstTex = resManager.loadBMP("Resources/Textures/grass.bmp");
	if (!firstTex) Util::panic("Texture didn't load properly\n");
	GLuint textureID = glGetUniformLocation(shader.getProgram(), "myTextureSampler");

	//Load models
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = resManager.loadOBJ("Resources/Models/cube.obj", vertices, uvs, normals);

	//Set up buffers
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Draw window, exit on ShouldClose or escape key
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.getProgram());

		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, firstTex);
		glUniform1i(textureID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//Cleanup
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(shader.getProgram());
	glDeleteTextures(1, &firstTex);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();
	return 0;
}

int glfwSetUp() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialise GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "First", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to create window\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	return 0;
}

int glewSetUp() {
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	else {
		return 0;
	}
}