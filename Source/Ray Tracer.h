#pragma once

#include<vector>
#include<iostream>
#include <algorithm>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "VectorMath.h"
#include "Model.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "Scene.h"

enum class RT_Setting
{
	Max_Bounces,
	Sun_Radius, Sun_Intensity, Sun_Altitude, Sun_Azimuthal, Sky_Variation,
	Sensor_Size, Focal_Length, Focus_Dist, F_Stop
};

enum class PostProcess_Setting
{
	Gamma, Exposure
};

template<>
struct std::formatter<RT_Setting> : std::formatter<std::string>
{
	auto format(const RT_Setting& Setting, format_context& ctx) const
	{
		if(Setting == RT_Setting::Max_Bounces)
			return std::formatter<std::string>::format(std::format("{}", "Max_Bounces"), ctx);

		if (Setting == RT_Setting::Sun_Radius)
			return std::formatter<std::string>::format(std::format("{}", "Sun_Radius"), ctx);

		if (Setting == RT_Setting::Sun_Intensity)
			return std::formatter<std::string>::format(std::format("{}", "Sun_Intensity"), ctx);

		if (Setting == RT_Setting::Sun_Altitude)
			return std::formatter<std::string>::format(std::format("{}", "Sun_Altitude"), ctx);

		if (Setting == RT_Setting::Sun_Azimuthal)
			return std::formatter<std::string>::format(std::format("{}", "Sun_Azimuthal"), ctx);

		if (Setting == RT_Setting::Sky_Variation)
			return std::formatter<std::string>::format(std::format("{}", "Sky_Variation"), ctx);

		if (Setting == RT_Setting::Sensor_Size)
			return std::formatter<std::string>::format(std::format("{}", "Sensor_Size"), ctx);

		if (Setting == RT_Setting::Focal_Length)
			return std::formatter<std::string>::format(std::format("{}", "Focal_Length"), ctx);

		if (Setting == RT_Setting::Focus_Dist)
			return std::formatter<std::string>::format(std::format("{}", "Focus_Dist"), ctx);

		if (Setting == RT_Setting::F_Stop)
			return std::formatter<std::string>::format(std::format("{}", "F_Stop"), ctx);

		else
			return std::formatter<std::string>::format(std::format("{}", "<Invalid Setting>"), ctx);
	}
};

template<>
struct std::formatter<PostProcess_Setting> : std::formatter<std::string>
{
	auto format(const PostProcess_Setting& Setting, format_context& ctx) const
	{
		if (Setting == PostProcess_Setting::Gamma)
			return std::formatter<std::string>::format(std::format("{}", "Gamma"), ctx);

		if (Setting == PostProcess_Setting::Exposure)
			return std::formatter<std::string>::format(std::format("{}", "Exposure"), ctx);

		else
			return std::formatter<std::string>::format(std::format("{}", "<Invalid Setting>"), ctx);
	}
};

static const std::unordered_map<RT_Setting, std::string> SettingUniformMap =
{
	std::pair(RT_Setting::Max_Bounces, "max_bounces"),
	std::pair(RT_Setting::Sun_Radius, "SunRadius"),
	std::pair(RT_Setting::Sun_Intensity, "SunIntensity"),
	std::pair(RT_Setting::Sun_Altitude, "SunAltitude"),
	std::pair(RT_Setting::Sun_Azimuthal, "SunAzimuthal"),
	std::pair(RT_Setting::Sky_Variation, "SkyVariation"),
	std::pair(RT_Setting::Sensor_Size, "Sensor_Size"),
	std::pair(RT_Setting::Focal_Length, "Focal_Length"),
	std::pair(RT_Setting::Focus_Dist, "Focus_Dist"),
	std::pair(RT_Setting::F_Stop, "F_Stop"),
};

static const std::unordered_map<PostProcess_Setting, std::string> PostSettingUniformMap =
{
	std::pair(PostProcess_Setting::Gamma, "gamma"),
	std::pair(PostProcess_Setting::Exposure, "exposure")
};

class RayTracer
{
public:
	RayTracer(const int& FramebufferWidth, const int& FramebufferHeight);
	~RayTracer();
	void SetDefaultSettings();
	void FramebufferReSize(const int& Width, const int& Height);
	void Draw() const;
	void Render() const;
	void StartAccumulation(const unsigned int& RenderSlot = 1, const unsigned int& AccumulationSlot = 2);
	void Accumulate();
	void ResetAccumulation();
	void PostProcess();
	void Clear(const float& Red = 0.0f, const float& Green = 0.0f, const float& Blue = 0.0f) const;
	void AddToBuffer(const std::string& name, const Sphere& Sphere);
	void SwapBufferObject(const std::string& name, const Sphere& Sphere);
	void ClearBuffer();
	unsigned int RenderedSamples() const;

	void SetCameraPosition(const glm::vec3& Position);
	void SetCameraOrientation(const float& yaw, const float& pitch);
	void MoveCamera(const float& deltaX, const float& deltaY, const float& deltaZ);
	void TurnCamera(const float& xoffset, const float& yoffset);

	void LoadScene(const Scene& scene);

	template<typename T>
	void Setting(const RT_Setting& setting, const T& value)
	{
		const std::string& name = SettingUniformMap.at(setting);
		m_RTShader.SetUniform(name, value);

		if (!m_Accumulating)
			return;

		ResetAccumulation();
	}

	template<typename T>
	void Setting(const PostProcess_Setting& setting, const T& value)
	{
		const std::string name = PostSettingUniformMap.at(setting);
		m_PostProcessShader.SetUniform(name, value);
	}

private:
	void UploadSphere(const int& index) const;
	void UploadSpheres() const;

private:
	mutable Shader m_RTShader = Shader("res/Ray Trace.glsl");
	mutable Shader m_AccumulationShader = Shader("res/Accumulator.glsl");
	mutable Shader m_PostProcessShader = Shader("res/PostProcess.glsl");
	VertexBuffer m_WindowVB;
	IndexBuffer m_WindowIB;
	VertexArray m_WindowVA;
	Camera m_Camera;

	Framebuffer m_RenderFB;
	Framebuffer m_AccumulationFB;
	bool m_Accumulating = false;

	int m_RenderTexSlot;
	int m_AccumulationTexSlot;
	int m_FramebufferWidth;
	int m_FramebufferHeight;

	int m_CurrentSample = 0;

	std::vector<Sphere> m_SphereList;
	std::unordered_map<std::string, int> m_SphereIndexMap;
};