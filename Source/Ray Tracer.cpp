#include"Ray Tracer.h"

RayTracer::RayTracer(const int& FramebufferWidth, const int& FramebufferHeight)
	:RenderTexSlot(1), AccumulationTexSlot(2), FramebufferWidth(FramebufferWidth), FramebufferHeight(FramebufferHeight)
{
	const float AspectRatio = (float)FramebufferWidth / (float)FramebufferHeight;
	RTShader.SetUniform("AspectRatio", AspectRatio);

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

	WindowVB.Load(Vertices, sizeof(Vertices));
	WindowIB.Load(Indices, 6);

	WindowBufferLayout.Push<float>(2);
	WindowBufferLayout.Push<float>(2);
	WindowVA.AddBuffer(WindowVB, WindowBufferLayout);
	WindowIB.Bind();

	m_SphereList.reserve(2);
}

RayTracer::~RayTracer()
{
}

void RayTracer::FramebufferReSize(const int& Width, const int& Height)
{
	ResetAccumulation();
	RenderFB.ReSize(Width, Height);
	AccumulationFB.ReSize(Width, Height);
	const float AspectRatio = (float)Width / (float)Height;
	RTShader.SetUniform("AspectRatio", AspectRatio);
}

void RayTracer::AddToBuffer(const Sphere& Sphere)
{
	m_SphereList.push_back(Sphere);
	UploadSpheres();
	ResetAccumulation();
}

void RayTracer::SwapBufferObject(const unsigned int& index, const Sphere& Sphere)
{
	m_SphereList.at(index) = Sphere;
	UploadSpheres();
	ResetAccumulation();
}

void RayTracer::ClearBuffer()
{
	m_SphereList.clear();
	UploadSpheres();
	ResetAccumulation();
}

void RayTracer::Draw() const
{
	WindowVA.Bind();
	glDrawElements(GL_TRIANGLES, WindowIB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RayTracer::Clear(const float& Red, const float& Green, const float& Blue) const
{
	glClearColor(Red, Green, Blue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RayTracer::UploadSpheres() const
{
	for (size_t i = 0; i < m_SphereList.size(); i++)
	{
		std::stringstream ss;
		ss << "SphereList[";
		ss << std::to_string(i);
		ss << "]";
		std::string out = ss.str();

		RTShader.SetFloat(out + ".Position", m_SphereList.at(i).Position);
		RTShader.SetFloat(out + ".Radius", m_SphereList.at(i).Radius);
		RTShader.SetFloat(out + ".Mat.BaseColor", m_SphereList.at(i).material.BaseColor);
		RTShader.SetFloat(out + ".Mat.Roughness", m_SphereList.at(i).material.Roughness);
		RTShader.SetFloat(out + ".Mat.Emission", m_SphereList.at(i).material.Emission);
	}
}

void RayTracer::Render() const
{
	WindowVA.Bind();
	RTShader.Use();

	for (auto& [name, uniform] : RTShader.GetUniformMap())
	{
		if (!RTShader.CheckUniformStatus(name) && name != "SphereList")
			std::println("Setting {} is not set", InvSettingUniformMap.at(name));
	}

	glDrawElements(GL_TRIANGLES, WindowIB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RayTracer::StartAccumulation(const unsigned int& RenderSlot, const unsigned int& AccumulationSlot)
{
	RenderTexSlot = RenderSlot;
	AccumulationTexSlot = AccumulationSlot;

	RenderFB.ReSize(FramebufferWidth, FramebufferHeight);
	AccumulationFB.ReSize(FramebufferWidth, FramebufferHeight);

	ResetAccumulation();
}

void RayTracer::Accumulate()
{
	RenderFB.Bind(RenderTexSlot);
	RTShader.SetUniform("CurrentSample", CurrentSample);
	Render();

	AccumulationFB.Bind(AccumulationTexSlot);
	AccumulationShader.SetUniform("CurrentSampleImage", RenderTexSlot);
	AccumulationShader.SetUniform("Accumulated", AccumulationTexSlot);
	AccumulationShader.SetUniform("CurrentSample", (float)CurrentSample);
	AccumulationShader.Use();
	Draw();
	AccumulationFB.UnBind();

	CurrentSample++;
}

void RayTracer::ResetAccumulation()
{
	CurrentSample = 0;
	AccumulationFB.Bind(AccumulationTexSlot);
	Clear();
}

unsigned int RayTracer::RenderedSamples() const
{
	return CurrentSample;
}