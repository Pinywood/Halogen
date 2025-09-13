struct Ray
{
	vec3 RayOrigin;
	vec3 RayDir;
	vec3 RayColor;
};

void WorldColor(in vec3 direction, out vec4 color)
{
	direction = normalize(direction);
	float SunHaloRadius = 1.0;									//Should be in [0, 1], it is the cos of the angular radius
	vec3 SunHaloPos = normalize(WorldX + WorldY + WorldZ);
	vec4 SunHaloColor = vec4(1.0, 1.0, 0.0, 1.0);
	SunHaloColor *= vec4(vec3(4.0), 10.0);

	float SunRadius = 0.002;									//Should be in [0, 1], it is the cos of the angular radius
	vec3 SunPos = SunHaloPos;
	vec4 SunColor = vec4(1.0, 1.0, 0.6, 1.0);
	SunColor *= vec4(vec3(1000.0), 1.0);

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
		color = vec4(theta - 0.3, theta - 0.3, theta, 1.0);
	}

	
	color *= vec4(vec3(10.0), 1.0);
	float t = exp(-1.0 * (theta2 - 1.0) * (theta2 - 1.0)/ (SunHaloRadius * SunHaloRadius));
	SunHaloColor = (SunHaloColor) * sin(1.57 * t);
	color += SunHaloColor;

	t = exp(-1.0 * (theta3 - 1.0) * (theta3 - 1.0)/ (SunRadius * SunRadius));
	color = (SunColor - color) * sin(1.57 * t) + color;
}

float HitPoint(in Ray ray, in Sphere sphere)
{
	vec3 diff = ray.RayOrigin - sphere.Position;
	float discriminant = pow(dot(ray.RayDir, diff), 2.0) - (dot(diff, diff) - pow(sphere.Radius, 2.0));

	if(discriminant < 0.0)
		return -1.0;
			
	float temp = (-sqrt(discriminant) - dot(ray.RayDir, diff));
	return temp;
}

void UpdateRay(inout Ray ray, in Sphere HitSphere, in float t, in float seed)
{
	ray.RayOrigin = ray.RayOrigin + t * ray.RayDir;		//RayOrigin = intersection

	vec3 normal = ray.RayOrigin - HitSphere.Position;
	normal = normalize(normal);

	vec3 randVec = pcg3dSphere(ray.RayDir + seed + 1.0);

	if(dot(randVec, normal) < 0.0)
		randVec *= -1.0;

	ray.RayDir = mix(reflect(ray.RayDir, normal), randVec, HitSphere.Mat.Roughness);
	ray.RayDir = normalize(ray.RayDir);

	ray.RayColor *= HitSphere.Mat.BaseColor;
}

vec3 ComputeRayColor(in Ray ray, in Sphere Models[ModelCount], in int max_bounces, in float seed)
{
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