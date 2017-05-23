//The fragment shader operates on each pixel in a given polygon
#version 330 core
 
layout(location=0) in vec3 vertPosition;
 
// Model, View, Projection matrix
uniform mat4 camera;
uniform mat4 model;
 
void main()
{
    gl_Position = camera * model * vec4(vertPosition, 1.0f);
}