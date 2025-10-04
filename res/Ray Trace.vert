#version 400 core

layout (location = 0) in vec2 vertexPositions;

#include "Model.glsl"
#include "Uniforms.glsl"

out vec3 positions;

out vec3 WorldX;
out vec3 WorldY;
out vec3 WorldZ;

void main()
{
	gl_Position = vec4(vertexPositions, 0.0, 1.0);
	positions = vec3(vertexPositions.x * Sensor_Size / 2.0, vertexPositions.y * Sensor_Size / (2.0 * AspectRatio), 0.0);

	WorldX = View * vec3(1.0, 0.0, 0.0);
	WorldY = View * vec3(0.0, 1.0, 0.0);
	WorldZ = View * vec3(0.0, 0.0, 1.0);
}