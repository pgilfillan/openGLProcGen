#include <stdio.h>
#include <stdlib.h>

#include "GL/glew.h"
#include "Shader.h"
#include "Util.h"


Shader::Shader(const char *vsName, const char *fsName) {
	program = glCreateProgram();

	//Open vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *vs_source = Util::fileToBuffer(vsName);
	if (!vs_source) {
		Util::panic("Failed to open vertex shader\n");
	}
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);

	//Open fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *fs_source = Util::fileToBuffer(fsName);
	if (!fs_source) {
		Util::panic("Failed to open fragment shader\n");
	}
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}