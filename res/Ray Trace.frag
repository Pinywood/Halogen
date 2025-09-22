#version 400 core

#include "Ray.glsl"

in vec2 positions;
in Sphere Spheres[ModelCount];

out vec4 FragmentColor;

void main()
{
	vec3 pixel_Position = vec3(positions.x * Sensor_Size / 2.0, positions.y * Sensor_Size / (2.0 * AspectRatio), -Focal_Length);

	Ray TracingRay;

	TracingRay.RayOrigin = vec3(0.0, 0.0, 0.0);
	TracingRay.RayDir = pixel_Position;
	TracingRay.RayColor = vec3(1.0, 1.0, 1.0);

	vec3 color = ComputeRayColor(TracingRay, Spheres, max_bounces, float(CurrentSample));
	vec4 colorOut = vec4(color, 1.0);
	FragmentColor = colorOut;
}