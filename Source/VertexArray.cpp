#include "VertexArray.h"

VertexArray::VertexArray()
	:m_CurrentAttributeIndex(0)
{
	glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& Buffer, const VertexBufferLayout& BufferLayout)
{
	Bind();
	Buffer.Bind();
	const auto& AttributeArray = BufferLayout.GetAttributeArray();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < AttributeArray.size(); i++)
	{
		const auto& Attribute = AttributeArray[i];
		glVertexAttribPointer(i + m_CurrentAttributeIndex, Attribute.count, Attribute.type, Attribute.normalized, BufferLayout.GetStride(), (const void*)offset);
		glEnableVertexAttribArray(i + m_CurrentAttributeIndex);
		offset += Attribute.count * Attribute.GetSize();
	}

	m_CurrentAttributeIndex += AttributeArray.size();
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::UnBind() const
{
	glBindVertexArray(0);
}
