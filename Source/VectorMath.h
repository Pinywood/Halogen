#pragma once

class Vec3
{
public:
	Vec3()
		:x(0.0), y(0.0), z(0.0)
	{
	}

	Vec3(const double& scalar)
		:x(scalar), y(scalar), z(scalar) 
	{
	}

	Vec3(const double& x, const double& y, const double& z)
		:x(x), y(y), z(z) 
	{
	}

	Vec3 operator+(const Vec3& other);

	Vec3 operator-(const Vec3& other);

	Vec3 operator*(const Vec3& other);

	double dot(Vec3 A, Vec3 B);

public:
	float x, y, z;
};

class Vec2
{
public:
	Vec2()
		:x(0.0), y(0.0)
	{
	}

	Vec2(const double& scalar)
		:x(scalar), y(scalar)
	{
	}

	Vec2(const double& x, const double& y)
		:x(x), y(y)
	{
	}

	Vec2 operator+(const Vec2& other);

	Vec2 operator-(const Vec2& other);

	Vec2 operator*(const Vec2& other);

	double dot(Vec2 A, Vec2 B);

public:
	float x, y;
};
