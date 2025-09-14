#version 330 core
const int ModelCount = 4;

in vec3 WorldX;
in vec3 WorldY;
in vec3 WorldZ;

#include "res/Model.glsl"
#include "res/PRNG.glsl"
#include "res/Ray.glsl"

in vec2 positions;
in Sphere Spheres[ModelCount];

uniform float AspectRatio;
uniform int CurrentSample;
uniform int max_bounces;

out vec4 FragmentColor;

void main()
{
	float FOV = 1.0;
	float z_depth = 1.0/(2.0 * tan(FOV/2.0));
	vec3 pixel_Position = vec3(positions.x * AspectRatio, positions.y, -z_depth);

	Ray TracingRay;

	TracingRay.RayOrigin = vec3(0.0, 0.0, 0.0);
	TracingRay.RayDir = normalize(pixel_Position);
	TracingRay.RayColor = vec3(1.0, 1.0, 1.0);

	vec3 color = ComputeRayColor(TracingRay, Spheres, max_bounces, float(CurrentSample));
	vec4 colorOut = vec4(color, 1.0);
	
	FragmentColor = colorOut;
}