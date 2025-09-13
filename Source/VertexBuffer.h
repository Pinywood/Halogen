#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>

class VertexBuffer
{
private:
	unsigned int m_RendererID;

public:
	VertexBuffer() = default;
	VertexBuffer(const void* data, size_t size);
	void Load(const void* data, size_t size);
	~VertexBuffer();
	void Bind() const;
	void Unbind() const;
};