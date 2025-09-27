#version 400 core

layout (location = 0) in vec2 vertexPositions;

#include "Model.glsl"
#include "Uniforms.glsl"

out vec2 positions;

out vec3 WorldX;
out vec3 WorldY;
out vec3 WorldZ;

void main()
{
	gl_Position = vec4(vertexPositions, 0.0, 1.0);

	positions = vertexPositions;

	WorldX = View * vec3(1.0, 0.0, 0.0);
	WorldY = View * vec3(0.0, 1.0, 0.0);
	WorldZ = View * vec3(0.0, 0.0, 1.0);
}