#version 400

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 view, proj;

out vec3 vertex;
out vec3 normal;

void main() 
{
	normal = vertex_normal;
	vertex = vertex_position;
	gl_Position = vec4(vertex,1.0f);
}
