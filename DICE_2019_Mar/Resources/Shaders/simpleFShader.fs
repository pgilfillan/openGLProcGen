#version 330 core

in vec2 texCoordFrag;
in vec3 position_cameraspace;
in vec3 normal_cameraspace;
in int texToUse;

uniform sampler2D textureSampler;
uniform sampler2D textureSampler2;

uniform mat4 V;

//Light properties
uniform vec3 lightDir;
uniform vec3 lightIntensity;
uniform vec3 ambientIntensity;

//Material properties
uniform vec3 ambientCoeff;
uniform vec3 diffuseCoeff;
uniform vec3 specularCoeff;
uniform float phongExp;

out vec3 colour;

void main(void) {
	vec3 s = normalize(V * vec4(lightDir, 0)).xyz;
    vec3 v = normalize(-position_cameraspace);
    vec3 r = normalize(reflect(-s, normal_cameraspace));

	vec3 ambient = ambientIntensity * ambientCoeff;
    vec3 diffuse = max(lightIntensity * diffuseCoeff * dot(normal_cameraspace, s), 0);
    vec3 specular;
    if (dot(normal_cameraspace, s) > 0)
        specular = max(lightIntensity * specularCoeff * pow(dot(r,v), phongExp), 0);
    else
        specular = vec3(0.0);

    colour = (texture( textureSampler, texCoordFrag ).rgb)*(ambient + diffuse) + specular;

}