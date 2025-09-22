#include "Model.glsl"
#include "Uniforms.glsl"
#include "PRNG.glsl"

in vec3 WorldX;
in vec3 WorldY;
in vec3 WorldZ;

struct Ray
{
	vec3 RayOrigin;
	vec3 RayDir;
	vec3 RayColor;
};

void WorldColor(in vec3 direction, out vec4 color)
{
	direction = normalize(direction);
	vec3 SunPos = normalize(cos(SunAltitude) * sin(SunAzimuthal) * WorldX + sin(SunAltitude) * WorldY + cos(SunAltitude) * cos(SunAzimuthal) * WorldZ);
	vec4 SunColor = vec4(1.0, 1.0, 0.6, 1.0);
	SunColor *= vec4(vec3(SunIntensity), 1.0);

	vec3 SunHaloPos = SunPos;
	vec4 SunHaloColor = vec4(1.0, 1.0, 0.2, 1.0);
	SunHaloColor *= vec4(vec3(4.0), 1.0);
	float SunHaloRadius = 1.0;								//Should be in [0, 1], it is the cos of the angular radius

	float theta = dot(direction, WorldY);
	float theta2 = dot(direction, SunHaloPos);
	float theta3 = dot(direction, SunPos);

	if(theta < 0.0)
	{
		float expVal = exp(-10.0 * theta * theta);
		color = vec4(0.7 * expVal, 0.7 * expVal, expVal, 1.0);
	}

	else
	{
		theta = cos(theta);
		float R = (theta - 0.3) * SkyVariation + 0.7 * (1.0 - SkyVariation);
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

float HitPoint(in Ray ray, in Sphere sphere)
{
	vec3 diff = ray.RayOrigin - sphere.Position;
	float RaySphereDist = dot(diff, diff);
	float discriminant = dot(ray.RayDir, diff) * dot(ray.RayDir, diff) - (RaySphereDist - sphere.Radius * sphere.Radius);

	if(discriminant < 0.0)
		return -1.0;
			
	float temp = (-sqrt(discriminant) - dot(ray.RayDir, diff));
	float temp2 = (sqrt(discriminant) - dot(ray.RayDir, diff));

	if(abs(temp) < 0.001 && abs(temp2) < 0.001)
		return -1.0;

	if(abs(temp) < 0.001)
		return temp2;

	if(abs(temp2) < 0.001)
		return temp;

	if(RaySphereDist < sphere.Radius * sphere.Radius)
	{
		if(temp > 0.0)
			return temp;

		return temp2;
	}
	
	if(temp < temp2)
		return temp;

	return temp2;
}

void UpdateRay(inout Ray ray, in Sphere HitSphere, in float t, in float seed)
{
	ray.RayOrigin = ray.RayOrigin + t * ray.RayDir;				//RayOrigin = Intersection

	vec3 normal = ray.RayOrigin - HitSphere.Position;
	normal = normalize(normal);

	if(dot(normal, ray.RayDir) > 0.0)
		normal = -normal;

	vec3 randVec = pcg3dSphere(ray.RayDir + seed + 1.0);

	randVec += normal;

	if(abs(randVec.x) < 0.001 && abs(randVec.y) < 0.001 && abs(randVec.z) < 0.001)		//Catch reflection rays close to 0
		randVec = normal;

	ray.RayDir = mix(reflect(ray.RayDir, normal), randVec, HitSphere.Mat.Roughness);
	ray.RayDir = normalize(ray.RayDir);

	ray.RayColor *= HitSphere.Mat.BaseColor;
}

vec3 ComputeRayColor(in Ray ray, in Sphere Models[ModelCount], in int max_bounces, in float seed)
{
	vec3 RayOffset = 2.0 * vec3(pcg3d(ray.RayDir + seed + 1.0).xy, 0.0) - 1.0;			//Make native square sampling
	float OffsetWidth = Sensor_Size / float(FramebufferWidth);
	float OffsetHeight =  (Sensor_Size / AspectRatio) / float(FramebufferHeight);

	RayOffset *= vec3(OffsetWidth, OffsetHeight, 0.0);

	ray.RayDir = ray.RayDir + RayOffset;
	ray.RayDir = normalize(ray.RayDir);

	for(int bounces = 0; bounces < max_bounces; bounces++)
	{
		bool IsHit = false;
		float t = 99999.999;
		Sphere HitSphere;

		for(int i = 0; i < ModelCount; i++)
		{
			Sphere sphere = Models[i];
			float temp = HitPoint(ray, sphere);

			if(0.0 < temp && temp < t)
			{
				IsHit = true;
				HitSphere = sphere;
				t = temp;
			}
		}

		if(!IsHit)
		{
			vec4 color;
			WorldColor(ray.RayDir, color);
			color *= vec4(ray.RayColor, 1.0);
			return color.xyz;
		}

		if(HitSphere.Mat.Emission != 0.0)
		{
			ray.RayColor *= HitSphere.Mat.BaseColor * HitSphere.Mat.Emission;
			return ray.RayColor;
		}

		UpdateRay(ray, HitSphere, t, seed);
	}

	return vec3(0.0);
}