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
	int max_bounces = 30;
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
	Setting(RT_Setting::Max_Bounces, max_bounces);
	Setting(RT_Setting::Sensor_Size, Sensor_Size / 1000.0);
	Setting(RT_Setting::Focal_Length, Focal_Length / 1000.0);
	Setting(RT_Setting::Focus_Dist, Focus_Dist);
	Setting(RT_Setting::F_Stop, F_Stop);
	Setting(PostProcess_Setting::Gamma, gamma);
	Setting(PostProcess_Setting::Exposure, exposure);
}

void RayTracer::FramebufferReSize(const int& Width, const int& Height)
{
	ResetAccumulation();
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
	int index = m_SphereIndexMap.at(name);
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

	m_RTShader.AddToLookUp("ModelCount", m_SphereList.size());
	m_RTShader.ReCompile();
	UploadSpheres();
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

	m_RTShader.SetFloat(out + ".Position", m_SphereList.at(index).Position);
	m_RTShader.SetFloat(out + ".Radius", m_SphereList.at(index).Radius);
	m_RTShader.SetInt(out + ".Mat.Type", (int)m_SphereList.at(index).material.Type);
	m_RTShader.SetFloat(out + ".Mat.Albedo", m_SphereList.at(index).material.Albedo);
	m_RTShader.SetFloat(out + ".Mat.Roughness", m_SphereList.at(index).material.Roughness);
	m_RTShader.SetFloat(out + ".Mat.Emission", m_SphereList.at(index).material.Emission);
	m_RTShader.SetFloat(out + ".Mat.IOR", m_SphereList.at(index).material.IOR);
}

void RayTracer::UploadSpheres() const
{
	for (size_t i = 0; i < m_SphereList.size(); i++)
	{
		UploadSphere(i);
	}
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
	m_RTShader.ReCompile();
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

unsigned int RayTracer::RenderedSamples() const
{
	return m_CurrentSample;
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

void RayTracer::LoadScene(const Scene& scene)
{
	int max_bounces = scene.m_MaxBounces;
	float Sensor_Size = scene.m_SensorSize;
	float Focal_Length = scene.m_FocalLength;
	float Focus_Dist = scene.m_FocusDist;
	float F_Stop = scene.m_FStop;

	float SunIntensity = scene.m_SunIntensity;
	float SunRadius = scene.m_SunRadius;
	float SunAltitude = scene.m_SunAltitude;
	float SunAzimuthal = scene.m_SunAzimuthal;
	float SkyVariation = scene.m_SkyVariation;

	float gamma = scene.m_Gamma;
	float exposure = scene.m_Exposure;

	Setting(RT_Setting::Sun_Radius, SunRadius / 200.0);
	Setting(RT_Setting::Sun_Intensity, SunIntensity);
	Setting(RT_Setting::Sun_Altitude, glm::radians(SunAltitude));
	Setting(RT_Setting::Sun_Azimuthal, glm::radians(SunAzimuthal));
	Setting(RT_Setting::Sky_Variation, SkyVariation);
	Setting(RT_Setting::Max_Bounces, max_bounces);
	Setting(RT_Setting::Sensor_Size, Sensor_Size / 1000.0);
	Setting(RT_Setting::Focal_Length, Focal_Length / 1000.0);
	Setting(RT_Setting::Focus_Dist, Focus_Dist);
	Setting(RT_Setting::F_Stop, F_Stop);
	Setting(PostProcess_Setting::Gamma, gamma);
	Setting(PostProcess_Setting::Exposure, exposure);

	ClearBuffer();

	for (auto& [name, sphere] : scene.m_SphereMap)
	{
		AddToBuffer(name, sphere);
	}

	SetCameraOrientation(scene.m_Camera.m_Yaw, scene.m_Camera.m_Pitch);
	SetCameraPosition(scene.m_Camera.m_Position);
}
