#include "VectorMath.h"

Vec3 Vec3::operator+(const Vec3& other)
{
	return Vec3(x + other.x, y + other.y, z + other.z);
}

void Vec3::operator+=(const Vec3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

Vec3 Vec3::operator-(const Vec3& other)
{
	return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator*(const Vec3& other)
{
	return Vec3(x * other.x, y * other.y, z * other.z);
}

double Vec3::dot(Vec3 A, Vec3 B)
{
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

Vec2 Vec2::operator+(const Vec2& other)
{
	return Vec2(x + other.x, y + other.y);
}

void Vec2::operator+=(const Vec2& other)
{
	x += other.x;
	y += other.y;
}

Vec2 Vec2::operator-(const Vec2& other)
{
	return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(const Vec2& other)
{
	return Vec2(x * other.x, y * other.y);
}

double Vec2::dot(Vec2 A, Vec2 B)
{
	return A.x * B.x + A.y * B.y;
}

