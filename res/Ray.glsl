//!#version 400
#include "Model.glsl"
#include "Uniforms.glsl"
#include "PRNG.glsl"

in vec3 WorldX;
in vec3 WorldY;
in vec3 WorldZ;

const bool RenderBlackHole = true;

struct Ray
{
	vec3 RayOrigin;
	vec3 RayDir;
	vec3 RayColor;
};

struct HitRecord
{
	bool Hit;
	float t;
	Sphere HitSphere;
};

struct BlackHoleInfo
{
	vec3 BlackHolePos;
	float SchwarzschildRadius;
	float PSphereRadius;
	vec3 Radial;
	float r;
	vec3 UnitRadial;
	vec3 Omega;
	vec3 UnitAngular;
	float rVel;
	float k;
	float PhiVel;
	float rAcc;
	bool Escaped;
	float dt;
	Sphere Influence;
};

void WorldColor(in vec3 direction, out vec4 color)
{
	direction = normalize(direction);
	vec3 SunPos = normalize(cos(SunAltitude) * sin(SunAzimuthal) * WorldX + sin(SunAltitude) * WorldY + cos(SunAltitude) * cos(SunAzimuthal) * WorldZ);
	vec4 SunColor = vec4(1.0, 1.0, 0.6, 1.0);
	SunColor *= vec4(vec3(SunIntensity), 1.0);

	vec3 SunHaloPos = SunPos;
	vec4 SunHaloColor = vec4(1.0, 1.0, 0.2, 1.0);
	SunHaloColor *= vec4(vec3(2.0), 1.0);
	float SunHaloRadius = 0.9;								//Should be in [0, 1], it is the cos of the angular radius

	float theta = dot(direction, WorldY);
	float theta2 = dot(direction, SunHaloPos);
	float theta3 = dot(direction, SunPos);

	if(theta < 0.0)
	{
		float expVal = exp(-10.0 * theta * theta);
		color = vec4(0.8 * expVal, 0.8 * expVal, expVal, 1.0);
	}

	else
	{
		theta = cos(theta);
		float R = (theta - 0.2) * SkyVariation + 0.8 * (1.0 - SkyVariation);
		float G = R;
		float B = theta * SkyVariation + 1.0 - SkyVariation;
		color = vec4(R, G, B, 1.0);
	}
	
	color *= vec4(vec3(10.0), 1.0);
	float t = exp(-1.0 * (theta2 - 1.0) * (theta2 - 1.0)/ (SunHaloRadius * SunHaloRadius));
	SunHaloColor = (SunHaloColor) * sin(1.57 * t);
	color += SunHaloColor;

	t = exp(-1.0 * (theta3 - 1.0) * (theta3 - 1.0)/ (SunRadius * SunRadius));
	color = (SunColor) * sin(1.57 * t) + color;
}

HitRecord HitPoint(in Ray ray, in Sphere sphere)
{
	HitRecord record;
	record.Hit = false;
	record.HitSphere = sphere;
	record.t = -1.0;

	vec3 diff = ray.RayOrigin - sphere.Position;
	float RaySphereDist = dot(diff, diff);
	float discriminant = dot(ray.RayDir, diff) * dot(ray.RayDir, diff) - (RaySphereDist - sphere.Radius * sphere.Radius);

	if(discriminant < 0.0)
		return record;
			
	float temp = (-sqrt(discriminant) - dot(ray.RayDir, diff));
	float temp2 = (sqrt(discriminant) - dot(ray.RayDir, diff));

	if(abs(temp) < 0.001 && abs(temp2) < 0.001)
		return record;

	record.Hit = true;

	if(abs(temp) < 0.001)
	{
		record.t = temp2;
		return record;
	}

	if(abs(temp2) < 0.001)
	{
		record.t = temp;
		return record;
	}

	if(RaySphereDist < sphere.Radius * sphere.Radius)
	{
		if(temp > 0.0)
		{
			record.t = temp;
			return record;
		}

		record.t = temp2;
		return record;
	}
	
	if(temp < temp2)
	{
		record.t = temp;
		return record;
	}
		
	record.t = temp2;
	return record;
}

HitRecord HitPoint(Ray ray, Sphere Models[ModelCount])
{
	HitRecord record;
	record.Hit = false;
	record.t = 99999.999;

	for(int i = 0; i < ModelCount; i++)
	{
		Sphere sphere = Models[i];
		HitRecord temp = HitPoint(ray, sphere);

		if(0.0 < temp.t && temp.t < record.t)
		{
			record.Hit = true;
			record.HitSphere = sphere;
			record.t = temp.t;
		}
	}

	return record;
}

void Scatter(Diffuse diffuse, inout Ray ray, HitRecord record, in float seed)
{
	ray.RayOrigin = ray.RayOrigin + record.t * ray.RayDir;				//RayOrigin = Intersection

	vec3 normal = ray.RayOrigin - record.HitSphere.Position;
	normal = normalize(normal);

	if(dot(normal, ray.RayDir) > 0.0)
		normal = -normal;

	vec3 randVec = pcg3dSphere(ray.RayDir + seed + 1.0);

	randVec += normal;

	if(abs(randVec.x) < 0.001 && abs(randVec.y) < 0.001 && abs(randVec.z) < 0.001)		//Catch reflection rays close to 0
		randVec = normal;

	ray.RayDir = mix(reflect(ray.RayDir, normal), randVec, diffuse.Roughness);
	ray.RayDir = normalize(ray.RayDir);

	ray.RayColor *= diffuse.Albedo;
}

float reflectance(float cosine, float IOR)
{
	float r0 = (1.0 - IOR) / (1.0 + IOR);
	r0 = r0 * r0;
	return r0 + (1.0 - r0) * pow(1.0 - cosine, 5.0);
}

void Scatter(Glass glass, inout Ray ray, HitRecord record, in float seed)
{
	ray.RayOrigin = ray.RayOrigin + record.t * ray.RayDir;				//RayOrigin = Intersection

	vec3 normal = ray.RayOrigin - record.HitSphere.Position;
	normal = normalize(normal);

	float IOR = 1.0/glass.IOR;

	if(dot(normal, ray.RayDir) > 0.0)
	{
		normal = -normal;
		IOR = glass.IOR;
	}

	float cos_theta = min(dot(-ray.RayDir, normal), 1.0);
	float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	if(IOR * sin_theta > 1.0 || reflectance(cos_theta, IOR) > pcg3d(ray.RayDir + seed).x)
		ray.RayDir = reflect(ray.RayDir, normal);
	
	else
		ray.RayDir = refract(ray.RayDir, normal, IOR);
	
	ray.RayDir = normalize(ray.RayDir);
	ray.RayColor *= glass.Albedo;
}

void UpdateRay(inout Ray ray, HitRecord record, in float seed)
{
	Material material = MaterialList[record.HitSphere.MatIndex];
	switch(material.Type)
	{
		case DiffuseType:
		{
			Diffuse diffuse;
			diffuse.Albedo = material.Albedo;
			diffuse.Roughness = material.Roughness;
			diffuse.Emission = material.Emission;
			Scatter(diffuse, ray, record, seed);
			break;
		}

		case GlassType:
		{
			Glass glass;
			glass.Albedo = material.Albedo;
			glass.IOR = material.IOR;
			Scatter(glass, ray, record, seed);
			break;
		}
	}
}

bool UpdateRay(inout Ray ray, inout BlackHoleInfo BHInfo)
{
	BHInfo.UnitAngular = normalize(cross(BHInfo.Omega, BHInfo.Radial));
	BHInfo.Radial += ray.RayDir * BHInfo.dt;
	BHInfo.r = length(BHInfo.Radial);
	BHInfo.UnitRadial = BHInfo.Radial / BHInfo.r;

	BHInfo.rVel = (BHInfo.rAcc * BHInfo.dt + BHInfo.rVel);
	BHInfo.PhiVel = BHInfo.k / (BHInfo.r * BHInfo.r);
	ray.RayDir = BHInfo.rVel * BHInfo.UnitRadial + BHInfo.r * BHInfo.PhiVel * BHInfo.UnitAngular;
	ray.RayDir = normalize(ray.RayDir);
	ray.RayOrigin = BHInfo.Radial + BHInfo.BlackHolePos;
	BHInfo.rAcc = (BHInfo.r - BHInfo.PSphereRadius) * BHInfo.PhiVel * BHInfo.PhiVel;

	if(BHInfo.r < BHInfo.PSphereRadius && BHInfo.rVel < 0.0)
		return true;

	BHInfo.Escaped = (BHInfo.rVel > 0.0) && (BHInfo.r > BHInfo.Influence.Radius);
	return false;
}

Ray GetRay(vec3 PixelPos, float seed)
{
	Ray ray;
	ray.RayOrigin = PixelPos;
	ray.RayColor = vec3(1.0);

	vec3 RayOffset = 2.0 * vec3(pcg3d(ray.RayOrigin + seed).xy, 0.0) - 1.0;			//[Improve]: Make native square sampling
	float OffsetWidth = Sensor_Size / float(FramebufferWidth);
	float OffsetHeight =  (Sensor_Size / AspectRatio) / float(FramebufferHeight);
	RayOffset *= vec3(OffsetWidth, OffsetHeight, 0.0);

	ray.RayOrigin += RayOffset;

	float LensFocalLength = Focal_Length * Focus_Dist / (Focal_Length + Focus_Dist);
	vec3 FocusPoint = ray.RayOrigin * LensFocalLength + vec3(0.0, 0.0, Focal_Length * Focal_Length);
	FocusPoint /= LensFocalLength - Focal_Length;

	float DiskRadius = Focal_Length / (2.0 * F_Stop);
	vec3 DiskPoint = DiskRadius * pcg3dDisk(ray.RayOrigin + seed + 1.0) + vec3(0.0, 0.0, -Focal_Length);

	ray.RayOrigin = DiskPoint;
	ray.RayDir = normalize(FocusPoint - ray.RayOrigin);

	return ray;
}

//Compute BlackHoleInfo from Initial Ray
void ComputeBlackHoleInfo(Ray ray, inout BlackHoleInfo BHInfo)
{
	BHInfo.Radial = ray.RayOrigin - BHInfo.BlackHolePos;
	BHInfo.r = length(BHInfo.Radial);
	BHInfo.UnitRadial = BHInfo.Radial / BHInfo.r;

	HitRecord record = HitPoint(ray, BHInfo.Influence);
	BHInfo.Escaped = !record.Hit && (BHInfo.r > BHInfo.Influence.Radius);
	if(BHInfo.Escaped)
		return;

	BHInfo.Omega = cross(BHInfo.Radial, ray.RayDir);
	BHInfo.rVel = dot(ray.RayDir, BHInfo.UnitRadial);
	BHInfo.k = BHInfo.r * length(ray.RayDir - BHInfo.rVel * BHInfo.UnitRadial);
	BHInfo.PhiVel = BHInfo.k / (BHInfo.r * BHInfo.r);
	BHInfo.rAcc = (BHInfo.r - BHInfo.PSphereRadius) * BHInfo.PhiVel;
	BHInfo.UnitAngular = normalize(cross(BHInfo.Omega, BHInfo.Radial));
}

vec3 TraceRay(in Ray ray, in Sphere Models[ModelCount], in int max_depth, in float seed)
{
	ray = GetRay(ray.RayOrigin, seed);

	BlackHoleInfo BHInfo;
	if(RenderBlackHole)
	{
		BHInfo.BlackHolePos = BlackHolePosition;
		BHInfo.BlackHolePos -= CameraPos;
		BHInfo.BlackHolePos = View * BHInfo.BlackHolePos;
		BHInfo.SchwarzschildRadius = SchwarzsRadius;
		BHInfo.PSphereRadius = BHInfo.SchwarzschildRadius * 3.0 / 2.0;
		BHInfo.dt = StepSize;

		BHInfo.Influence.Position = BHInfo.BlackHolePos;
		BHInfo.Influence.Radius = MaxInfluenceRadius;

		ComputeBlackHoleInfo(ray, BHInfo);
	}

	for(int depth = 0; depth < max_depth; depth++)
	{
		HitRecord record = HitPoint(ray, Models);

		if(record.t > 2.0 * BHInfo.dt && !BHInfo.Escaped && RenderBlackHole)
		{
			if(UpdateRay(ray, BHInfo))
				return vec3(0.0);
			continue;
		}

		if(!record.Hit)
		{
			vec4 color;
			WorldColor(ray.RayDir, color);
			color *= vec4(ray.RayColor, 1.0);
			return color.rgb;
		}

		Material material = MaterialList[record.HitSphere.MatIndex];

		if(material.Emission != 0.0)
		{
			ray.RayColor *= material.Albedo * material.Emission;
			return ray.RayColor;
		}

		UpdateRay(ray, record, seed);

		if(RenderBlackHole)
			ComputeBlackHoleInfo(ray, BHInfo);
	}

	return vec3(0.0);
}