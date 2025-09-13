struct Material
{
	vec3 BaseColor;
	float Roughness;
	float Emission;
};

struct Sphere
{
	vec3 Position;
	float Radius;
	Material Mat;
};