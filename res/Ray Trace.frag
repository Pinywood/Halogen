#version 330 core
const int ModelCount = 4;

in vec3 WorldX;
in vec3 WorldY;
in vec3 WorldZ;

#include "res/Model.glsl"

in vec2 positions;
in Sphere Spheres[ModelCount];

in float Sensor_Size;
in float Focal_Length;

uniform float AspectRatio;
uniform int CurrentSample;
uniform int max_bounces;
uniform int FramebufferWidth;
uniform int FramebufferHeight;

#include "res/PRNG.glsl"
#include "res/Ray.glsl"

out vec4 FragmentColor;

void main()
{
	vec3 pixel_Position = vec3(positions.x * AspectRatio * Sensor_Size, positions.y * Sensor_Size, -Focal_Length);

	Ray TracingRay;

	TracingRay.RayOrigin = vec3(0.0, 0.0, 0.0);
	TracingRay.RayDir = pixel_Position;
	TracingRay.RayColor = vec3(1.0, 1.0, 1.0);

	vec3 color = ComputeRayColor(TracingRay, Spheres, max_bounces, float(CurrentSample));
	vec4 colorOut = vec4(color, 1.0);
	
	FragmentColor = colorOut;
}