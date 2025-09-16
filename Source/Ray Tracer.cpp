#include"Ray Tracer.h"

RayTracer::RayTracer()
{
	float Vertices[8] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f
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
	WindowVA.AddBuffer(WindowVB, WindowBufferLayout);
	WindowIB.Bind();

	m_SphereList.reserve(2);
}

RayTracer::~RayTracer()
{
}

void RayTracer::AddToBuffer(const Sphere& Sphere)
{
	m_SphereList.push_back(Sphere);
}

void RayTracer::SwapBufferObject(const unsigned int& index, const Sphere& Sphere)
{
	m_SphereList.at(index) = Sphere;
}

void RayTracer::ClearBuffer()
{
	m_SphereList.clear();
}

void RayTracer::Clear(const float& Red, const float& Green, const float& Blue) const
{
	glClearColor(Red, Green, Blue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RayTracer::Render() const
{
	for (int i = 0; i < m_SphereList.size(); i++)
	{
		std::stringstream ss;
		ss << "SphereList[";
		ss << std::to_string(i);
		ss << "]";
		std::string out = ss.str();

		shader.SetFloat(out + ".Position", m_SphereList.at(i).Position);
		shader.SetFloat(out + ".Radius", m_SphereList.at(i).Radius);
		shader.SetFloat(out + ".Mat.BaseColor", m_SphereList.at(i).material.BaseColor);
		shader.SetFloat(out + ".Mat.Roughness", m_SphereList.at(i).material.Roughness);
		shader.SetFloat(out + ".Mat.Emission", m_SphereList.at(i).material.Emission);
	}

	WindowVA.Bind();
	shader.Use();

	for (auto& [name, uniform] : shader.GetUniformMap())
	{
		if (!shader.CheckUniformStatus(name) && name != "SphereList")
			std::println("Uniform {} is not set", name);
	}

	glDrawElements(GL_TRIANGLES, WindowIB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

