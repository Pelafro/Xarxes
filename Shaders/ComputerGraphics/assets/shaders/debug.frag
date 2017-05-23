//The fragment shader operates on each pixel in a given polygon
#version 330 core

uniform vec3 lightColor; // Light's diffuse and specular contribution.
 
layout (location=0) out vec4 finalColor;

void main() {
	finalColor = vec4(lightColor, 1.0f);
}