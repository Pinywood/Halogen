#include"Ray Tracer.h"

RayTracer::RayTracer(const int& FramebufferWidth, const int& FramebufferHeight)
	:m_RenderTexSlot(1), m_AccumulationTexSlot(2), m_FramebufferWidth(FramebufferWidth), m_FramebufferHeight(FramebufferHeight)
{
	const float AspectRatio = (float)FramebufferWidth / (float)FramebufferHeight;
	m_RTShader.SetUniform("AspectRatio", AspectRatio);
	m_RTShader.SetUniform("FramebufferWidth", m_FramebufferWidth);
	m_RTShader.SetUniform("FramebufferHeight", m_FramebufferHeight);

	m_RTShader.SetUniform("View", m_Camera.GetViewMatrix());
	m_RTShader.SetUniform("CameraPos", Vec3(m_Camera.m_Position.x, m_Camera.m_Position.y, m_Camera.m_Position.z));

	float Vertices[] =
	{				   //Tex Coords
		-1.0f, -1.0f,	0.0, 0.0,
		 1.0f, -1.0f,	1.0, 0.0,
		 1.0f,  1.0f,	1.0, 1.0,
		-1.0f,  1.0f,	0.0, 1.0
	};

	unsigned int Indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};
	VertexBufferLayout WindowBufferLayout;

	m_WindowVB.Load(Vertices, sizeof(Vertices));
	m_WindowIB.Load(Indices, 6);

	WindowBufferLayout.Push<float>(2);
	WindowBufferLayout.Push<float>(2);
	m_WindowVA.AddBuffer(m_WindowVB, WindowBufferLayout);
	m_WindowIB.Bind();

	m_SphereList.reserve(2);
	SetDefaultSettings();
}

RayTracer::~RayTracer()
{
}

void RayTracer::SetDefaultSettings()
{
	int max_depth = 60;
	float Sensor_Size = 100.0;
	float Focal_Length = 35.0;
	float Focus_Dist = 1.0;
	float F_Stop = 1.4;

	float SunIntensity = 800.0;
	float SunRadius = 0.8;
	float SunAltitude = 30.0;
	float SunAzimuthal = 0.0;
	float SkyVariation = 0.2;

	float gamma = 2.2;
	float exposure = 1.5;

	Setting(RT_Setting::Sun_Radius, SunRadius / 200.0);
	Setting(RT_Setting::Sun_Intensity, SunIntensity);
	Setting(RT_Setting::Sun_Altitude, glm::radians(SunAltitude));
	Setting(RT_Setting::Sun_Azimuthal, glm::radians(SunAzimuthal));
	Setting(RT_Setting::Sky_Variation, SkyVariation);
	Setting(RT_Setting::Max_Depth, max_depth);
	Setting(RT_Setting::Sensor_Size, Sensor_Size / 1000.0);
	Setting(RT_Setting::Focal_Length, Focal_Length / 1000.0);
	Setting(RT_Setting::Focus_Dist, Focus_Dist);
	Setting(RT_Setting::F_Stop, F_Stop);
	Setting(PostProcess_Setting::Gamma, gamma);
	Setting(PostProcess_Setting::Exposure, exposure);
}

void RayTracer::FramebufferReSize(const int& Width, const int& Height)
{
	m_FramebufferWidth = Width;
	m_FramebufferHeight = Height;
	m_RenderFB.ReSize(m_FramebufferWidth, m_FramebufferHeight);
	m_AccumulationFB.ReSize(m_FramebufferWidth, m_FramebufferHeight);
	const float AspectRatio = (float)Width / (float)Height;
	m_RTShader.SetUniform("AspectRatio", AspectRatio);
	m_RTShader.SetUniform("FramebufferWidth", m_FramebufferWidth);
	m_RTShader.SetUniform("FramebufferHeight", m_FramebufferHeight);
}

void RayTracer::AddToBuffer(const std::string& name, const Sphere& Sphere)
{
	const auto& found = m_SphereIndexMap.find(name);
	if (found != m_SphereIndexMap.end())
	{
		std::println("Attempting to add Sphere {}, already exists, try using SwapBufferObject instead", name);
		return;
	}

	m_SphereList.push_back(Sphere);
	m_SphereIndexMap[name] = m_SphereList.size() - 1;

	if (!m_Accumulating)
		return;

	m_RTShader.AddToLookUp("ModelCount", m_SphereList.size());
	m_RTShader.ReCompile();
	UploadSphere(m_SphereList.size() - 1);
	ResetAccumulation();
}

void RayTracer::SwapBufferObject(const std::string& name, const Sphere& Sphere)
{
	const auto& found = m_SphereIndexMap.find(name);
	if (found == m_SphereIndexMap.end())
	{
		std::println("Attempting to Swap Object {}, does not exist, try AddToBuffer instead", name);
		return;
	}

	int index = found->second;
	m_SphereList.at(index) = Sphere;

	if (!m_Accumulating)
		return;

	UploadSphere(index);
	ResetAccumulation();
}

void RayTracer::ClearBuffer()
{
	m_SphereList.clear();
	m_SphereIndexMap.clear();

	if (!m_Accumulating)
		return;

	m_RTShader.AddToLookUp("ModelCount", 1);
	m_RTShader.ReCompile();
	ResetAccumulation();
}

void RayTracer::AddMaterial(const std::string& name, const Material& material)
{
	const auto& found = m_MaterialIndexMap.find(name);
	if (found != m_MaterialIndexMap.end())
	{
		std::println("Attempting to add Material {}, already exists, try using SwapMaterial instead", name);
		return;
	}

	m_MaterialList.push_back(material);
	m_MaterialIndexMap[name] = m_MaterialList.size() - 1;

	if (!m_Accumulating)
		return;

	m_RTShader.AddToLookUp("MaterialCount", m_MaterialList.size());
	m_RTShader.ReCompile();
	UploadMaterial(m_MaterialList.size() - 1);
	ResetAccumulation();
}

void RayTracer::SwapMaterial(const std::string& name, const Material& material)
{
	const auto& found = m_MaterialIndexMap.find(name);
	if (found == m_MaterialIndexMap.end())
	{
		std::println("Attempting to Swap Material {}, does not exist, try AddMaterial instead", name);
		return;
	}

	int index = found->second;
	m_MaterialList.at(index) = material;

	if (!m_Accumulating)
		return;

	UploadMaterial(index);
	ResetAccumulation();
}

void RayTracer::ClearMaterials()
{
	m_MaterialList.clear();
	m_MaterialIndexMap.clear();

	if (!m_Accumulating)
		return;

	m_RTShader.AddToLookUp("MaterialCount", 1);
	m_RTShader.ReCompile();
	ResetAccumulation();
}

void RayTracer::Draw() const
{
	m_WindowVA.Bind();
	glDrawElements(GL_TRIANGLES, m_WindowIB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RayTracer::Clear(const float& Red, const float& Green, const float& Blue) const
{
	glClearColor(Red, Green, Blue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RayTracer::UploadSphere(const int& index) const
{
	std::string out = std::format("SphereList[{}]", std::to_string(index));

	const Sphere& sphere = m_SphereList[index];
	m_RTShader.SetFloat(out + ".Position", sphere.Position);
	m_RTShader.SetFloat(out + ".Radius", sphere.Radius);

	const auto& found = m_MaterialIndexMap.find(sphere.MaterialName);
	if (found == m_MaterialIndexMap.end())
	{
		for (auto& [name, sphereindex] : m_SphereIndexMap)						//Cache the inverse map
		{
			if (sphereindex == index)
				std::println("Sphere {} has material {}, does not exist!", name, sphere.MaterialName);
		}

		return;
	}

	m_RTShader.SetInt(out + ".MatIndex", found->second);
}

void RayTracer::UploadSpheres() const
{
	for (size_t i = 0; i < m_SphereList.size(); i++)
		UploadSphere(i);
}

void RayTracer::UploadMaterial(const int& index) const
{
	std::string out = std::format("MaterialList[{}]", std::to_string(index));

	const Material& material = m_MaterialList[index];
	m_RTShader.SetInt(out + ".Type", (int)material.Type);
	m_RTShader.SetFloat(out + ".Albedo", material.Albedo);
	m_RTShader.SetFloat(out + ".Roughness", material.Roughness);
	m_RTShader.SetFloat(out + ".Emission", material.Emission);
	m_RTShader.SetFloat(out + ".IOR", material.IOR);
}

void RayTracer::UploadMaterials() const
{
	for (int i = 0; i < m_MaterialList.size(); i++)
		UploadMaterial(i);
}

void RayTracer::Render() const
{
	m_WindowVA.Bind();
	m_RTShader.Use();

	for (auto& [Setting, UniformName] : SettingUniformMap)
	{
		if (!m_RTShader.CheckUniformStatus(UniformName))
			std::println("Setting {} is not set", Setting);
	}

	glDrawElements(GL_TRIANGLES, m_WindowIB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RayTracer::StartAccumulation(const unsigned int& RenderSlot, const unsigned int& AccumulationSlot)
{
	m_Accumulating = true;

	m_RenderTexSlot = RenderSlot;
	m_AccumulationTexSlot = AccumulationSlot;

	m_RenderFB.ReSize(m_FramebufferWidth, m_FramebufferHeight);
	m_AccumulationFB.ReSize(m_FramebufferWidth, m_FramebufferHeight);

	m_AccumulationShader.SetUniform("CurrentSampleImage", m_RenderTexSlot);
	m_AccumulationShader.SetUniform("Accumulated", m_AccumulationTexSlot);

	m_PostProcessShader.SetUniform("Image", m_AccumulationTexSlot);

	m_RTShader.AddToLookUp("ModelCount", m_SphereList.size());
	m_RTShader.AddToLookUp("MaterialCount", m_MaterialList.size());
	m_RTShader.ReCompile();
	UploadMaterials();
	UploadSpheres();
	ResetAccumulation();
}

void RayTracer::Accumulate()
{
	if (!m_Accumulating)
	{
		std::println("Warning: Call to Accumulate without a call to StartAccumulation");
		return;
	}

	glViewport(0, 0, m_FramebufferWidth, m_FramebufferHeight);

	m_RenderFB.Bind(m_RenderTexSlot);
	m_RTShader.SetUniform("CurrentSample", m_CurrentSample);
	Render();

	m_AccumulationFB.Bind(m_AccumulationTexSlot);
	m_AccumulationShader.SetUniform("CurrentSample", (float)m_CurrentSample);
	m_AccumulationShader.Use();
	Draw();
	m_AccumulationFB.UnBind();

	m_CurrentSample++;
}

void RayTracer::ResetAccumulation()
{
	if (!m_Accumulating)
	{
		std::println("Warning: Call to ResetAccumulation without a call to StartAccumulation");
		return;
	}

	m_CurrentSample = 0;
	m_AccumulationFB.Bind(m_AccumulationTexSlot);
	Clear();
	m_AccumulationFB.UnBind();
}

void RayTracer::PostProcess()
{
	m_RenderFB.Bind(m_RenderTexSlot);
	m_PostProcessShader.Use();

	Draw();
	m_RenderFB.UnBind();
}

unsigned char* RayTracer::GetRenderedImage() const
{
	unsigned char* Image = new unsigned char[3 * m_FramebufferWidth * m_FramebufferHeight];
	m_RenderFB.Bind(m_RenderTexSlot);
	glReadPixels(0, 0, m_FramebufferWidth, m_FramebufferHeight, GL_RGB, GL_UNSIGNED_BYTE, Image);
	m_RenderFB.UnBind();
	return Image;
}

unsigned int RayTracer::RenderedSamples() const
{
	return m_CurrentSample;
}

int RayTracer::GetFramebufferWidth() const
{
	return m_FramebufferWidth;
}

int RayTracer::GetFramebufferHeight() const
{
	return m_FramebufferHeight;
}

void RayTracer::SetRenderBlackHole(const bool& value)
{
	m_RTShader.AddToLookUp("RenderBlackHole", value);
	m_RTShader.ReCompile();
	UploadMaterials();
	UploadSpheres();
}

void RayTracer::SetBlackHolePosition(const Vec3& value)
{
	m_RTShader.SetUniform("BlackHolePosition", value);
}

void RayTracer::SetBlackHoleRadius(const float& value)
{
	m_RTShader.SetUniform("SchwarzsRadius", value);
}

void RayTracer::SetCameraPosition(const glm::vec3& Position)
{
	m_Camera.m_Position = Position;
	m_RTShader.SetUniform("CameraPos", Vec3(m_Camera.m_Position.x, m_Camera.m_Position.y, m_Camera.m_Position.z));
}

void RayTracer::SetCameraOrientation(const float& yaw, const float& pitch)
{
	m_Camera.SetOrientation(yaw, pitch);
	m_RTShader.SetUniform("View", m_Camera.GetViewMatrix());
}

void RayTracer::MoveCamera(const float& deltaX, const float& deltaY, const float& deltaZ)
{
	m_Camera.Move(deltaX, deltaY, deltaZ);
	m_RTShader.SetUniform("CameraPos", Vec3(m_Camera.m_Position.x, m_Camera.m_Position.y, m_Camera.m_Position.z));
}

void RayTracer::TurnCamera(const float& xoffset, const float& yoffset)
{
	m_Camera.Turn(xoffset, yoffset);
	m_RTShader.SetUniform("View", m_Camera.GetViewMatrix());
}

Camera RayTracer::GetCamera() const
{
	return m_Camera;
}

void RayTracer::LoadScene(const Scene& scene)
{
	Setting(RT_Setting::Sun_Radius, scene.m_SunRadius / 200.0);
	Setting(RT_Setting::Sun_Intensity, scene.m_SunIntensity);
	Setting(RT_Setting::Sun_Altitude, glm::radians(scene.m_SunAltitude));
	Setting(RT_Setting::Sun_Azimuthal, glm::radians(scene.m_SunAzimuthal));
	Setting(RT_Setting::Sky_Variation, scene.m_SkyVariation);
	Setting(RT_Setting::Max_Depth, scene.m_MaxDepth);
	Setting(RT_Setting::Sensor_Size, scene.m_SensorSize / 1000.0);
	Setting(RT_Setting::Focal_Length, scene.m_FocalLength / 1000.0);
	Setting(RT_Setting::Focus_Dist, scene.m_FocusDist);
	Setting(RT_Setting::F_Stop, scene.m_FStop);
	Setting(PostProcess_Setting::Gamma, scene.m_Gamma);
	Setting(PostProcess_Setting::Exposure, scene.m_Exposure);
	SetRenderBlackHole(scene.RenderBlackHole);
	SetBlackHolePosition(scene.BlackHolePosition);
	SetBlackHoleRadius(scene.SchwarzschildRadius);

	ClearBuffer();
	ClearMaterials();

	for (auto& [name, material] : scene.m_MaterialMap)
	{
		AddMaterial(name, material);
	}

	for (auto& [name, sphere] : scene.m_SphereMap)
	{
		AddToBuffer(name, sphere);
	}

	SetCameraOrientation(scene.m_Camera.m_Yaw, scene.m_Camera.m_Pitch);
	SetCameraPosition(scene.m_Camera.m_Position);
}
