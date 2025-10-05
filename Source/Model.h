#pragma once
#include "VectorMath.h"

enum class BSDFType
{
	Diffuse, Glass
};

template<>
struct std::formatter<BSDFType> : std::formatter<std::string>
{
	auto format(const BSDFType& type, format_context& ctx) const
	{
		if (type == BSDFType::Diffuse)
			return std::formatter<std::string>::format(std::format("{}", "DiffuseType"), ctx);

		if (type == BSDFType::Glass)
			return std::formatter<std::string>::format(std::format("{}", "GlassType"), ctx);

		else
			return std::formatter<std::string>::format(std::format("{}", "<NO_TYPE>"), ctx);
	}
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
	float Radius = 1.0;
	std::string MaterialName;
};