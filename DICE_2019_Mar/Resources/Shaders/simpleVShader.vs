#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec2 texCoordFrag;
out vec3 position_cameraspace;
out vec3 normal_cameraspace;

out int texToUse;

void main() {
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    texCoordFrag = vertexUV;
	position_cameraspace = (V*M*vec4(vertexPosition_modelspace, 1)).xyz;
    normal_cameraspace = normalize(V*M*vec4(normal, 0)).xyz;

	if (1 - normal.y > -0.1)
		texToUse = 0;
	else
		texToUse = 1;
}