#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;
	unsigned int m_CurrentAttributeIndex;

public:
	VertexArray();
	~VertexArray();
	void AddBuffer(const VertexBuffer& Buffer, const VertexBufferLayout& BufferLayout);
	void Bind() const;
	void UnBind() const;
};