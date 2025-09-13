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
uniform int samples;
uniform int max_bounces;

uniform float exposure;
uniform float gamma;

out vec4 FragmentColor;

float ToneMapFunc(in float x)
{
	float alpha = 3.0;
	x = pow(x, alpha);
	return x / (x + 1.0);
}

vec4 ToneMapper(in vec4 color)
{
	color.x = ToneMapFunc(color.x);
	color.y = ToneMapFunc(color.y);
	color.z = ToneMapFunc(color.z);
	return color;
}

void main()
{
	float FOV = 1.0;
	float z_depth = 1.0/(2.0 * tan(FOV/2.0));
	vec3 pixel_Position = vec3(positions.x * AspectRatio, positions.y, -z_depth);

	Ray TracingRay;
	vec4 colorOut = vec4(0.0);

	for(int sample = 0; sample < samples; sample++)
	{
		TracingRay.RayOrigin = vec3(0.0, 0.0, 0.0);
		TracingRay.RayDir = normalize(pixel_Position);
		TracingRay.RayColor = vec3(1.0, 1.0, 1.0);

		vec3 CurrentColor = ComputeRayColor(TracingRay, Spheres, max_bounces, float(sample));
		colorOut += vec4(CurrentColor, 1.0);
	}

	colorOut *= vec4(1.0/float(samples));
	colorOut *= vec4(vec3(exposure), 1.0);
	colorOut = ToneMapper(colorOut);
	colorOut = vec4(pow(colorOut.r, gamma), pow(colorOut.g, gamma), pow(colorOut.b, gamma), 1.0);
	FragmentColor = colorOut;
}