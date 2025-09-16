#version 330 core

layout (location = 0) in vec2 vertexPositions;

#include "res/Model.glsl"

const int ModelCount = 4;
uniform Sphere SphereList[ModelCount];
uniform mat3 View;
uniform vec3 CameraPos;

uniform float AspectRatio;
uniform float u_Sensor_Size;
uniform float u_Focal_Length;

out Sphere Spheres[ModelCount];
out vec2 positions; 

out float Sensor_Size;
out float Focal_Length;

out vec3 WorldX;
out vec3 WorldY;
out vec3 WorldZ;

void main()
{
	gl_Position = vec4(vertexPositions, 0.0, 1.0);

	for(int i = 0; i < ModelCount; i++)
	{
		Spheres[i] = SphereList[i];
		Spheres[i].Position -= CameraPos;
		Spheres[i].Position = View * Spheres[i].Position;
	}

	positions = vertexPositions;

	Sensor_Size = u_Sensor_Size / 1000.0;
	Sensor_Size /= 2.0 * AspectRatio;			//To make x values between [-Sensor_Size, Sensor_Size]

	Focal_Length = u_Focal_Length / 1000.0;

	WorldX = View * vec3(1.0, 0.0, 0.0);
	WorldY = View * vec3(0.0, 1.0, 0.0);
	WorldZ = View * vec3(0.0, 0.0, 1.0);
}