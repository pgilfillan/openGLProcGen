#pragma once

class Shader {
public:
	Shader(const char *vsName, const char *fsName);
	~Shader() = default;

	GLuint getProgram() { return program; }

private:
	GLuint program;
};

