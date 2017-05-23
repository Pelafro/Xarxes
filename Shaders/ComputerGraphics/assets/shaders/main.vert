//The fragment shader operates on each pixel in a given polygon
#version 330 core
 
layout(location=0) in vec3 vertPosition;
layout(location=1) in vec3 vertNormal;
layout(location=2) in vec2 vertUV;

out SHADER_SOCKET {
	vec3 fragPosition; // Position in world space.
	vec3 fragNormal; // Surface normal in world space.
	vec2 fragUV;
} vs_out;
 
// Model, View, Projection matrix
uniform mat4 camera;
uniform mat4 model;
 
void main()
{
    gl_Position = camera * model * vec4(vertPosition, 1.0f);
 
    vs_out.fragPosition = vec3(model * vec4(vertPosition, 1.0f));
    vs_out.fragNormal = mat3(transpose(inverse(model))) * vertNormal;
    vs_out.fragUV = vertUV;
}