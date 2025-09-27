uniform float Sensor_Size;
uniform float Focal_Length;
uniform vec3 CameraPos;

uniform int CurrentSample;
uniform int max_bounces;
uniform int FramebufferWidth;
uniform int FramebufferHeight;
uniform float AspectRatio;
uniform mat3 View;
const int ModelCount = 1;
uniform Sphere SphereList[ModelCount];

uniform float SunRadius;										//Should be in [0, 1], it is the cos of the angular radius
uniform float SunIntensity;
uniform float SunAltitude;
uniform float SunAzimuthal;
uniform float SkyVariation;