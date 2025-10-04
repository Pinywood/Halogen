#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <unordered_map>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <print>

#include "Model.h"
#include "Camera.h"
#include "Tokenization.h"

enum class Scene_Setting
{
	Max_Bounces,
	Sun_Radius, Sun_Intensity, Sun_Altitude, Sun_Azimuthal, Sky_Variation,
	Sensor_Size, Focal_Length, Focus_Dist, F_Stop,
	Gamma, Exposure
};

const std::unordered_map<std::string, Scene_Setting> SettingMap =
{
	std::pair("Max_Bounces", Scene_Setting::Max_Bounces),
	std::pair("Sun_Radius", Scene_Setting::Sun_Radius),
	std::pair("Sun_Intensity", Scene_Setting::Sun_Intensity),
	std::pair("Sun_Altitude", Scene_Setting::Sun_Altitude),
	std::pair("Sun_Azimuthal", Scene_Setting::Sun_Azimuthal),
	std::pair("Sky_Variation", Scene_Setting::Sky_Variation),
	std::pair("Sensor_Size", Scene_Setting::Sensor_Size),
	std::pair("Focal_Length", Scene_Setting::Focal_Length),
	std::pair("Focus_Dist", Scene_Setting::Focus_Dist),
	std::pair("F_Stop", Scene_Setting::F_Stop),
	std::pair("Gamma", Scene_Setting::Gamma),
	std::pair("Exposure", Scene_Setting::Exposure)
};

static bool EqualPresent(const std::string& string, const int& Line, const std::string& filepath)
{
	bool found = TokenPresent(string, "=");
	if (!found)
	{
		std::println("{}", string);
		std::println("SCENE FILE PARSE FAILED: SYNTAX ERROR: missing '=' at line {} in {}", Line, filepath);
	}

	return found;
}

static bool LeftParenPresent(const std::string& string, const int& Line, const std::string& filepath)
{
	bool found = TokenPresent(string, "(");
	if (!found)
		std::println("SCENE FILE PARSE FAILED: SYNTAX ERROR: missing '(' at line {} in {}", Line, filepath);

	return found;
}

class Scene
{
public:
	int m_MaxBounces = 30;

	float m_SensorSize = 100.0;
	float m_FocalLength = 35.0;
	float m_FocusDist = 1.0;
	float m_FStop = 1.4;

	float m_SunIntensity = 800.0;
	float m_SunRadius = 0.8;
	float m_SunAltitude = 30.0;
	float m_SunAzimuthal = 0.0;
	float m_SkyVariation = 0.2;

	float m_Gamma = 2.2;
	float m_Exposure = 1.5;

	std::map<std::string, Sphere> m_SphereMap;
	Camera m_Camera;

public:
	Scene() = default;
	Scene(const std::string& filepath);
	void Save();
	void Save(const std::string& filepath);
	void Load(const std::string& filepath);

private:
	std::string m_Filepath;

private:
	std::string GetSphereName(const std::string& line, const int& LineNumber, const std::string& filepath);
	void ParseTargetSpheres(std::string& TargetName, const std::string& line, const int& LineNumber, const std::string& filepath);
	void ParseTargetSettings(const std::string& line, const int& LineNumber, const std::string& filepath);
	void ParseTargetCamera(const std::string& line, const int& LineNumber, const std::string& filepath);

	template<typename T>
	void Setting(const Scene_Setting& Setting, const T& value);
};

template<typename T>
void Scene::Setting(const Scene_Setting& Setting, const T& value)
{
	switch (Setting)
	{
		case Scene_Setting::Max_Bounces:
			m_MaxBounces = value;
			break;

		case Scene_Setting::Sun_Radius:
			m_SunRadius = value;
			break;

		case Scene_Setting::Sun_Intensity:
			m_SunIntensity = value;
			break;

		case Scene_Setting::Sun_Altitude:
			m_SunAltitude = value;
			break;

		case Scene_Setting::Sun_Azimuthal:
			m_SunAzimuthal = value;
			break;

		case Scene_Setting::Sky_Variation:
			m_SkyVariation = value;
			break;

		case Scene_Setting::Sensor_Size:
			m_SensorSize = value;
			break;

		case Scene_Setting::Focal_Length:
			m_FocalLength = value;
			break;

		case Scene_Setting::Focus_Dist:
			m_FocusDist = value;
			break;

		case Scene_Setting::F_Stop:
			m_FStop = value;
			break;

		case Scene_Setting::Gamma:
			m_Gamma = value;
			break;

		case Scene_Setting::Exposure:
			m_Exposure = value;
			break;
	}
}
