#pragma once

#include<vector>
#include<iostream>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "VectorMath.h"
#include "Model.h"

enum class RT_Setting
{
	Aspect_Ratio, Max_Bounces, Current_Sample,
	Sun_Radius, Sun_Intensity, Sun_Altitude, Sun_Azimuthal, Sky_Variation,
	Sensor_Size, Focal_Length, View, Camera_Position
};

const std::unordered_map<RT_Setting, std::string> SettingUniformMap =
{
	std::pair(RT_Setting::Aspect_Ratio, "AspectRatio"),
	std::pair(RT_Setting::Max_Bounces, "max_bounces"),
	std::pair(RT_Setting::Current_Sample, "CurrentSample"),
	std::pair(RT_Setting::Sun_Radius, "SunRadius"),
	std::pair(RT_Setting::Sun_Intensity, "SunIntensity"),
	std::pair(RT_Setting::Sun_Altitude, "SunAltitude"),
	std::pair(RT_Setting::Sun_Azimuthal, "SunAzimuthal"),
	std::pair(RT_Setting::Sky_Variation, "SkyVariation"),
	std::pair(RT_Setting::Sensor_Size, "u_Sensor_Size"),
	std::pair(RT_Setting::Focal_Length, "u_Focal_Length"),
	std::pair(RT_Setting::View, "View"),
	std::pair(RT_Setting::Camera_Position, "CameraPos"),
};

class RayTracer
{
public:
	RayTracer();
	~RayTracer();
	void Render() const;
	void Clear(const float& Red = 0.2f, const float& Green = 0.2f, const float& Blue = 0.2f) const;
	void AddToBuffer(const Sphere& Sphere);
	void SwapBufferObject(const unsigned int& index, const Sphere& Sphere);
	void ClearBuffer();

	template<typename T>
	void Setting(const RT_Setting& setting, const T& value)
	{
		const std::string& name = SettingUniformMap.at(setting);
		shader.SetUniform(name, value);
	}

private:
	void UploadSpheres() const;

private:
	mutable Shader shader = Shader("res/Ray Trace.glsl");
	VertexBuffer WindowVB;
	IndexBuffer WindowIB;
	VertexArray WindowVA;

	std::vector<Sphere> m_SphereList;
};