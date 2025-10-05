struct Diffuse
{
	vec3 Albedo;
	float Roughness;
	float Emission;
};

struct Glass
{
	vec3 Albedo;
	float IOR;
};

const int DiffuseType = 0;
const int GlassType = 1;

struct Material
{
	int Type;
	vec3 Albedo;
	float Roughness;
	float Emission;
	float IOR;
};

struct Sphere
{
	vec3 Position;
	float Radius;
	int MatIndex;
};