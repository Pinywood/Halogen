#include "Scene.h"

Scene::Scene(const std::string& filepath)
{
	Load(filepath);
}

void Scene::Load(const std::string& filepath)
{
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss;

	enum class Target
	{
		None, Spheres, Settings, Camera
	};

	if (!stream.is_open())
	{
		std::println("Failed to Load Scene: {}", filepath);
		return;
	}

	m_Filepath = filepath;

	Target target = Target::None;
	std::string TargetName;
	size_t LineNumber = 0;

	while (getline(stream, line))
	{
		LineNumber++;

		if (line.find("Spheres:") != std::string::npos)
		{
			target = Target::Spheres;
			LineNumber++;
			getline(stream, line);
		}

		else if (line.find("Settings:") != std::string::npos)
		{
			target = Target::Settings;
			LineNumber++;
			getline(stream, line);
		}

		else if (line.find("Camera:") != std::string::npos)
		{
			target = Target::Camera;
			LineNumber++;
			getline(stream, line);
		}

		if (target == Target::Spheres)
			ParseTargetSpheres(TargetName, line, LineNumber, filepath);


		else if (target == Target::Settings)
			ParseTargetSettings(line, LineNumber, filepath);


		else if (target == Target::Camera)
			ParseTargetCamera(line, LineNumber, filepath);
	}
}

void Scene::Save()
{
	Save(m_Filepath);
}

void Scene::Save(const std::string& filepath)
{
	std::ofstream stream(filepath);

	std::println(stream, "Camera:");
	std::println(stream, "\tPosition = ({}, {}, {})", m_Camera.m_Position.x, m_Camera.m_Position.y, m_Camera.m_Position.z);
	std::println(stream, "\tYaw = {}", m_Camera.m_Yaw);
	std::println(stream, "\tPitch = {}", m_Camera.m_Pitch);
	std::print(stream, "\n");

	std::println(stream, "Spheres:");
	for (auto& [name, sphere] : m_SphereMap)
	{
		std::println(stream, "\t{}:", name);
		std::println(stream, "\t\t\t\t\tPosition = ({}, {}, {})", sphere.Position.x, sphere.Position.y, sphere.Position.z);
		std::println(stream, "\t\t\t\t\tRadius = {}", sphere.Radius);
		std::println(stream, "\t\t\t\t\tType = {}", sphere.material.Type);
		std::println(stream, "\t\t\t\t\tAlbedo = ({}, {}, {})", sphere.material.Albedo.x, sphere.material.Albedo.y, sphere.material.Albedo.z);
		std::println(stream, "\t\t\t\t\tRoughness = {}", sphere.material.Roughness);

		if (sphere.material.Emission != 0.0)
			std::println(stream, "\t\t\t\t\tEmission = {}", sphere.material.Emission);

		if (sphere.material.Type == BSDFType::Glass)
			std::println(stream, "\t\t\t\t\tIOR = {}", sphere.material.IOR);

		std::print(stream, "\n");
	}

	std::println(stream, "Settings:");
	std::println(stream, "\tMax_Bounces = {}", m_MaxBounces);
	std::println(stream, "\tSun_Radius = {}", m_SunRadius);
	std::println(stream, "\tSun_Intensity = {}", m_SunIntensity);
	std::println(stream, "\tSun_Altitude = {}", m_SunAltitude);
	std::println(stream, "\tSun_Azimuthal = {}", m_SunAzimuthal);
	std::println(stream, "\tSky_Variation = {}", m_SkyVariation);
	std::println(stream, "\tSensor_Size = {}", m_SensorSize);
	std::println(stream, "\tFocal_Length = {}", m_FocalLength);
	std::println(stream, "\tFocus_Dist = {}", m_FocusDist);
	std::println(stream, "\tF_Stop = {}", m_FStop);
	std::println(stream, "\tExposure = {}", m_Exposure);
}

std::string Scene::GetSphereName(const std::string& line, const int& LineNumber, const std::string& filepath)
{
	std::string name;

	char c;
	for (int i = 0; i < line.find(":"); i++)
	{
		c = line.at(i);

		if (isalnum(c) || c == '_')
			name.push_back(c);
	}

	if (m_SphereMap.find(name) != m_SphereMap.end())
	{
		std::println("SCENE FILE PARSE FAILED: at line {} in {}", LineNumber, filepath);
		std::println("Sphere {} already exists\n", name);
		return std::string("");
	}

	return name;
}

void inline Scene::ParseTargetSpheres(std::string& TargetName, const std::string& line, const int& LineNumber, const std::string& filepath)
{
	if (line.find(":") != std::string::npos)
	{
		TargetName = GetSphereName(line, LineNumber, filepath);
		if (TargetName.empty())
			std::println("SCENE FILE PARSE FAILED: No target name at line {} in {}", LineNumber, filepath);
	}

	if (line.find_first_not_of(' ') != std::string::npos && TargetName.empty())
		std::println("SCENE FILE PARSE FAILED: No target name at line {} in {}", LineNumber, filepath);
	
	else if (TokenPresentAfter(line, "Position", ":"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		if (!LeftParenPresent(line, LineNumber, filepath))
			return;
		auto found = line.find("(");

		Vec3 Position;

		found += 1;
		Position.x = std::stof(line.substr(found));

		found = line.find(",") + 1;
		Position.y = std::stof(line.substr(found));

		found = line.find(",", found + 1) + 1;
		Position.z = std::stof(line.substr(found));

		m_SphereMap[TargetName].Position = Position;
	}

	else if (TokenPresentAfter(line, "Radius", ":"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("=");
		float Radius = std::stof(line.substr(found + 1));
		m_SphereMap[TargetName].Radius = Radius;
	}

	else if (TokenPresentAfter(line, "Type", ":"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("=");

		std::string ShaderType;
		for (int i = found + 1; i < line.length(); i++)
		{
			char c = line.at(i);

			if (isalnum(c))
				ShaderType.push_back(c);
		}

		if (ShaderType == "DiffuseType")
			m_SphereMap[TargetName].material.Type = BSDFType::Diffuse;

		else if (ShaderType == "GlassType")
			m_SphereMap[TargetName].material.Type = BSDFType::Glass;
	}

	else if (TokenPresentAfter(line, "Albedo", ":"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		if (!LeftParenPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("(");

		Vec3 Color;

		found += 1;
		Color.x = std::stof(line.substr(found));

		found = line.find(",") + 1;
		Color.y = std::stof(line.substr(found));

		found = line.find(",", found + 1) + 1;
		Color.z = std::stof(line.substr(found));

		m_SphereMap[TargetName].material.Albedo = Color;
	}

	else if (TokenPresentAfter(line, "Roughness", ":"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("=");
		float Roughness = std::stof(line.substr(found + 1));
		m_SphereMap[TargetName].material.Roughness = Roughness;
	}

	else if (TokenPresentAfter(line, "Emission", ":"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("=");
		float Emission = std::stof(line.substr(found + 1));
		m_SphereMap[TargetName].material.Emission = Emission;
	}

	else if (TokenPresentAfter(line, "IOR", ":"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("=");
		float IOR = std::stof(line.substr(found + 1));
		m_SphereMap[TargetName].material.IOR = IOR;
	}
}

void inline Scene::ParseTargetSettings(const std::string& line, const int& LineNumber, const std::string& filepath)
{
	if (line.find_first_not_of(' ') == std::string::npos)
		return;

	if (!EqualPresent(line, LineNumber, filepath))
		return;

	auto found = line.find("=");

	char c;
	std::string SettingName;
	for (int i = 0; i < found; i++)
	{
		c = line.at(i);
		if (isalnum(c) || c == '_')
			SettingName.push_back(c);
	}

	if (line.find("(") == std::string::npos && line.find(",") == std::string::npos)
	{
		float value = std::stof(line.substr(found + 1));
		Setting(SettingMap.at(SettingName), value);
	}

	else
		std::println("SCENE FILE PARSE FAILED: Value is not a float at line {} in {}", LineNumber, filepath);
}

void inline Scene::ParseTargetCamera(const std::string& line, const int& LineNumber, const std::string& filepath)
{
	if (TokenPresent(line, "Yaw"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("=");
		m_Camera.SetYaw(std::stof(line.substr(found + 1)));
	}

	else if (TokenPresent(line, "Pitch"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		auto found = line.find("=");
		m_Camera.SetPitch(std::stof(line.substr(found + 1)));
	}

	else if (TokenPresent(line, "Position"))
	{
		if (!EqualPresent(line, LineNumber, filepath))
			return;

		if (!LeftParenPresent(line, LineNumber, filepath))
			return;
		auto found = line.find("(");

		Vec3 Position;

		found += 1;
		Position.x = std::stof(line.substr(found));

		found = line.find(",") + 1;
		Position.y = std::stof(line.substr(found));

		found = line.find(",", found + 1) + 1;
		Position.z = std::stof(line.substr(found));

		m_Camera.m_Position = glm::vec3(Position.x, Position.y, Position.z);
	}
}