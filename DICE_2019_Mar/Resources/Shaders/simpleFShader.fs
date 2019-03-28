#version 330 core

in vec2 UV;

out vec3 colour;

uniform sampler2D textureSampler;

void main(void) {

    colour = texture( textureSampler, UV ).rgb;

}