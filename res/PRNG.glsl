float floatConstruct( uint m ) 
{
    const uint ieeeMantissa = 0x007FFFFFu; 
    const uint ieeeOne      = 0x3F800000u; 

    m &= ieeeMantissa;
    m |= ieeeOne;                          

    float  f = uintBitsToFloat( m );
    return f - 1.0;                        
}

float pcg(float u)
{
    uint v = floatBitsToUint(u);
	uint state = v * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return floatConstruct((word >> 22u) ^ word);
}

//Random vec3 in [0, 1] ^ 3
vec3 pcg3d(vec3 uvw) 
{
	uvec3 v = floatBitsToUint(uvw);
    v = v * 1664525u + 1013904223u;

    v.x += v.y*v.z;
    v.y += v.z*v.x;
    v.z += v.x*v.y;

    v ^= v >> 16u;

    v.x += v.y*v.z;
    v.y += v.z*v.x;
    v.z += v.x*v.y;

    return vec3(floatConstruct(v.x), floatConstruct(v.y), floatConstruct(v.z));
}

vec3 pcg3dDisk(vec3 seed)
{
	const float pi = 3.1415926535;
	float theta = 2.0 * pi * pcg3d(seed).x;
	float r = pcg3d(seed).y;

    float X = r * cos(theta);
    float Y = r * sin(theta);

	return vec3(X, Y, 0.0);
}

vec3 pcg3dSphere(vec3 seed)
{
	const float pi = 3.1415926535;
	float azimuthal = 2.0 * pi * pcg3d(seed).x;
	float A = 2.0 * pcg3d(seed).y - 1.0;				//Using this instead of polar angle for uniform PDF

	float Z = -A;

	A *= A;
	A = sqrt(1.0 - A);

	float X = A * cos(azimuthal);
	float Y = A * sin(azimuthal);

	return vec3(X, Y, Z);
}