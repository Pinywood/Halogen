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
	void SetUniform(const std::string& name, const T& value)
	{
		shader.SetUniform(name, value);
	}

private:
	mutable Shader shader = Shader("res/Ray Trace.glsl");
	VertexBuffer WindowVB;
	IndexBuffer WindowIB;
	VertexArray WindowVA;

	std::vector<Sphere> m_SphereList;
};