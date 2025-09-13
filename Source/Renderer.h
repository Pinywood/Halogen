#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Clear(const float& Red = 0.2f, const float& Green = 0.2f, const float& Blue = 0.2f) const;
	void Draw(const VertexArray& VA, const unsigned int& count, const Shader& Shader) const;
};