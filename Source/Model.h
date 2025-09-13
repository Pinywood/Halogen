#pragma once
#include "VectorMath.h"

struct Material
{
	Vec3 BaseColor;
	float Roughness;
	float Emission;
};

struct Sphere
{
	Vec3 Position;
	float Radius;
	Material material;
};