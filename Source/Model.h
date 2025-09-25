#pragma once
#include "VectorMath.h"

enum class BSDFType
{
	Diffuse, Glass
};

struct Material
{
	BSDFType Type = BSDFType::Diffuse;
	Vec3 Albedo;
	float Roughness = 0.0;
	float Emission = 0.0;
	float IOR = 1.5;
};

struct Sphere
{
	Vec3 Position;
	float Radius;
	Material material;
};